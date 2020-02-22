// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <iostream>
#include <map>
#include <set>
#include <unordered_set>

#include <nlohmann/json.hpp>

#include "chart.h"
#include "gb2gc.h"

int gb2gc::run(int argc, const char* argv[])
{
   gb2gc::options options;
   auto err = options.parse(argc, argv);
   if (err)
      return err;
   write_chart(options, parse_data(options, parse_json(options.in_file())));
   return 0; // success
}

nlohmann::json gb2gc::parse_json(const std::string& file)
{
   // Due to an issue in google benchmark it will not encode backslashes
   // correctly which forces us to load json as string and do proper escaping
   // before attempting to parse it. Stream API would be preferrable instead.

   std::ifstream in(file, std::fstream::in);
   if (!in)
   {
      std::cerr << "Error: File non-existent or failed to open file: " << file << "\n";
      return 1;
   }

   std::string content(
      (std::istreambuf_iterator<char>(in)),
      (std::istreambuf_iterator<char>()));
   in.close();
   std::replace(content.begin(), content.end(), '\\', '/');

   return nlohmann::json::parse(content);
}

std::string make_title(const std::string title)
{
   // Early return if title is empty
   if (title.empty())
      return title;

   // Special handling of Google benchmark standard values
   if (title == std::string("cpu_time"))
      return "CPU time";

   // Replace underscore with space
   std::string copy = title;
   for (auto& c : copy)
   {
      if (c == '_')
         c = ' ';
   }

   // Convert to upper-case on initial letter
   if (!isupper(copy[0]))
      copy[0] = (char)toupper((int)copy[0]);

   return copy;
}

struct series
{
   std::string name;
   std::vector<nlohmann::json::const_iterator> benchmarks;
};

struct series_object
{
   std::vector<series> series;
};

// Default key selector differentiating on name
gb2gc::selector default_selector("name");

// Find key selector
gb2gc::selector* find_key(std::vector<gb2gc::selector>& selectors)
{
   auto key = std::find_if(selectors.begin(), selectors.end(),
      [](const gb2gc::selector& s) { return s.is_parameterized(); });
   if (key == selectors.end())
      return &default_selector;
   return &*key;
}

std::vector<std::string>
split_attribute(const nlohmann::json::const_iterator bm, const std::string& attribute)
{
   const auto name = bm->find(attribute);
   if (name == bm->end())
      throw std::exception(); // TODO Carry message?!
   if (!name->is_string())
      throw std::exception(); // TODO Carry message?!
   return gb2gc::split(name->get<std::string>(), '/');
}

bool accept(const nlohmann::json::const_iterator bm, const std::vector<std::string>& filter_splits)
{
   if (!filter_splits.empty())
   {
      const auto splits = split_attribute(bm, "name");
      for (auto i = 0u; i < splits.size() && i < filter_splits.size(); ++i)
      {
         const auto& filter_value = filter_splits[i];
         if (filter_value != "*" && filter_value != splits[i])
            return false; // no match
      }
   }
   return true; // match
}

series_object make_series(const nlohmann::json::const_iterator bm_begin,
   const nlohmann::json::const_iterator bm_end,
   const std::vector<gb2gc::selector>& selectors,
   const std::string& filter)
{
   series_object so;

   std::vector<std::string> filter_splits = gb2gc::split(filter, '/');

   std::string row;
   for (auto it = bm_begin; it != bm_end; ++it)
   {
      if (!accept(it, filter_splits))
         continue;

      for (auto s : selectors)
      {
         if (s.is_parameterized())
         {
            const auto splits = split_attribute(it, "name");
            for (auto i = 0u; i < splits.size(); ++i)
            {
               //if (s.param_index() == i)
               //{
               //	continue;
               //}

               if (!row.empty())
                  row += "/";
               if (s.param_index() == i)
                  row += "*";
               else
                  row += std::move(splits[i]);
            }
         }
      }

      auto sit = std::find_if(so.series.begin(), so.series.end(),
         [&](const series& s) { return s.name == row; });
      if (sit == so.series.end())
      {
         so.series.emplace_back(series{ row, std::vector<nlohmann::json::const_iterator>({ it }) });
         //so.series.emplace_back(std::make_pair(
         //	row, std::vector<nlohmann::json::const_iterator>({ it })));
      }
      else
      {
         sit->benchmarks.emplace_back(it);
      }

      //auto sit = so.s2bm.find(row);
      //if (so.s2bm.find(row) == so.s2bm.end())
      //{
      //	so.s2bm.insert(std::make_pair(
      //		row, std::vector<nlohmann::json::const_iterator>({ it })));
      //}
      //else
      //{
      //	sit->second.emplace_back(it);
      //}

      // Clear row string for next iteration
      row.clear();
   }

   return so;
}

gb2gc::data_set gb2gc::parse_data(const options& options, const nlohmann::json& bm_result)
{
   gb2gc::data_set ds;

   auto benchmarks = bm_result.find("benchmarks");
   if (benchmarks == bm_result.end())
   {
      std::cerr << "Error: Could not find 'benchmarks' element in given JSON source.\n";
      return ds; // TODO Exception
   }

   // Get selectors from options or populate with default selectors if not specified
   const std::vector<selector>& selectors = options.selectors();
   //if (selectors.empty())
   //{
   //	selectors.push_back(selector("name"));      // X (key)
   //	selectors.push_back(selector("cpu_time"));  // Y
   //	selectors.push_back(selector("real_time")); // Z
   //}

   // Make series from available benchmarks and selector. 
   // This basically creates a map of {series, benchmarks} based on set of
   // selectors where each unique name makes an individual key using name selectors
   // as wildcards for pattern matching.
   auto so = make_series(benchmarks->begin(), benchmarks->end(), selectors, options.filter());

   // Use selectors to create columns in data set representing series
   // and extract all distinct key values from series benchmarks and resize
   // the data set based on number of rows
   std::vector<gb2gc::variant> distinct_key_values;
   ds.add_column("Key");
   for (auto& series : so.series)
   {
      for (auto i = 1u; i < selectors.size(); ++i)
      {
         ds.add_column(series.name + " " + selectors[i].key());
      }

      for (auto& bm : series.benchmarks)
      {
         auto key_value = selectors[0](*bm);
         if (std::find(distinct_key_values.begin(), distinct_key_values.end(), key_value) == distinct_key_values.end())
            distinct_key_values.emplace_back(key_value);
      }
   }
   //std::sort(distinct_key_values.begin(), distinct_key_values.end());
   ds.resize_rows(distinct_key_values.size());

   for (auto row_index = 0u; row_index < distinct_key_values.size(); ++row_index)
   {
      // Insert key (x) value
      ds.get_col(0)[row_index] = distinct_key_values[row_index];

      // Insert other values
      auto column_index = 1u;
      for (auto& series : so.series)
      {
         // Find benchmark corresponding to current row
         auto find = std::find_if(series.benchmarks.begin(), series.benchmarks.end(),
            [&](const nlohmann::json::const_iterator& it)
         {
            return selectors[0](*it) == distinct_key_values[row_index];
         });
         if (find == series.benchmarks.end())
            continue; // TODO Handle

         for (auto i = 1u; i < selectors.size(); ++i)
         {
            ds.get_col(column_index)[row_index] = selectors[i](*(*find));
            ++column_index;
         }
      }
   }

   return ds;
}

void gb2gc::write_chart(const options& options, const gb2gc::data_set& data_set)
{
   // Generate chart
   gb2gc::googlechart gc;
   gc.options = options.chart_options();
   gc.type = options.chart_type();

   if (options.chart_type() == gb2gc::googlechart::visualization::bar)
      std::swap(gc.options.vertical_axis, gc.options.horizontal_axis);

   gb2gc::googlechart_dom_options dom_options = options.dom_options();

   std::string chart_div = options.out_file();
   std::replace(chart_div.begin(), chart_div.end(), '\\', '/');
   auto path_splits = split(chart_div, '/');
   auto filename_splits = split(path_splits[path_splits.size() - 1], '.');
   chart_div = filename_splits[0] + "_div";
   dom_options.div = chart_div;

   gc.write_html_file(options.out_file().c_str(), data_set, dom_options);
}
