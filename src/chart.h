// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifndef GB2GC_CHART_H
#define GB2GC_CHART_H

#include "dom.h"
#include "data_set.h"

#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <random>
#include <array>
#include <ctime>

namespace gb2gc
{
   ////////////////////////////////////////////////////////////////////////////
   // color

   struct color
   {
      static_assert(CHAR_BIT == 8, "Requires platform where char is a byte");

      unsigned char r;
      unsigned char g;
      unsigned char b;
   };

   inline color make_color(unsigned char r, unsigned char g, unsigned char b) noexcept
   {
      return color{ r, g, b };
   }

   std::ostream& operator<<(std::ostream& os, const color& color);

   // axis

   struct axis
   {
      std::string title;
      variant     min_value;
      variant     max_value;
   };

   // googlechart_dom_options

   struct googlechart_dom_options
   {
      static constexpr unsigned default_width = 900;
      static constexpr unsigned default_height = 500;

      unsigned width = default_width;
      unsigned height = default_height;
      std::string div = "chart_div";
   };

   // googlechart_options

   struct googlechart_options
   {
      enum class position
      {
         none,
         left,
         right,
         top,
         bottom
      };

      enum class curve
      {
         none,
         function
      };

      std::string        title;
      std::string        font_name;
      position           legend = position::none;
      curve              curve_type;
      std::vector<color> colors;
      float              data_opacity = 1.0f;
      float              point_size = 0.0f;
      bool               interpolate_nulls = false;
      // colors: ['green']
      // histogram: { bucketSize: 10000000 }
      axis horizontal_axis;
      axis vertical_axis;
   };

   std::ostream& operator<<(std::ostream& os, googlechart_options::position pos);
   std::ostream& operator<<(std::ostream& os, googlechart_options::curve curve);

   namespace detail
   {
      template<class Tuple, size_t Index>
      struct expand_format_tuple
      {
         static_assert(Index < std::tuple_size<Tuple>::value, "Index out of range");

         static inline void format(std::ostream& os, const Tuple& t)
         {
            expand_format_tuple<Tuple, Index - 1>::format(os, t);
            os << ", " << std::get<Index>(t);
         }
      };

      template<class Tuple>
      struct expand_format_tuple<Tuple, 0>
      {
         static inline void format(std::ostream& os, const Tuple& t)
         {
            os << std::get<0>(t);
         }
      };

      void write_axis(std::ostream& os, const format& fmt, size_t level, const axis& axis);
      void write_options(std::ostream& os, const format& fmt, size_t level, const googlechart_options& opt);
   }

   // Specialization for tuple data
   template<class Tuple>
   inline void format_data_as_csv_row_array(std::ostream& os, const Tuple& t)
   {
      detail::expand_format_tuple<Tuple, std::tuple_size<Tuple>::value - 1>::format(os, t);
   }

   template<class DataSet>
   void write(std::ostream& os, const format& fmt, size_t level, const DataSet& transformer)
   {
      const indent ind{ fmt, level };
      const indent ind_label{ fmt, level + 1 };
      os << ind << "var data = google.visualization.arrayToDataTable([\n";
      os << ind_label << '[';

      {	// format series
         const auto& series = transformer.series();
         auto it = series.begin();
         if (it != series.end())
            os << '\'' << (*it) << '\'';
         while (++it != series.end())
            os << ", '" << (*it) << '\'';
      }

      {	// format values
         for (auto it = transformer.data().begin(); it != transformer.data().end(); ++it)
         {
            os << "],\n" << ind_label << '[';
            format_data_as_csv_row_array<typename DataSet::value_type>(os, *it);
         }
      }

      os << "]]);\n";
   }

   template<>
   void write<data_set>(std::ostream& os, const format& fmt, size_t level, const data_set& ds);

   class googlechart
   {
   public:
      enum class visualization
      {
         histogram,
         scatter,
         line,
         bar,
      };

      googlechart_options options;
      //googlechart_data data;
      visualization type = visualization::histogram;
      //std::string chart_div = "chart_div";

      template<class DataSet>
      void write_html(std::ostream& os, const DataSet& transformer,
         const googlechart_dom_options& dom_options = googlechart_dom_options{})
      {
         element html("html");

         auto& head = html.add_element("head");
         head.add_element("script")
            .add_attribute("type", "text/javascript")
            .add_attribute("src", "https://www.gstatic.com/charts/loader.js");
         head.add_element("script")
            .add_attribute("type", "text/javascript")
            .set_content(make_convertible(*this, transformer, dom_options.div));

         auto& body = html.add_element("body");
         /*body.add_element("h1")
            .set_content("Auto-generated charts");
         body.add_element("p")
            .set_content("Generation timestamp: " + local_date_time());*/
         body.add_element("div")
            .set_comment("This div element will hold the generated chart")
            .add_attribute("id", dom_options.div)
            .add_attribute("style", "width: " + std::to_string(dom_options.width) +
               "px; height: " + std::to_string(dom_options.height) + "px;");

         write_dom(os, html);
      }

      template<class DataTransformer>
      void write_html_file(const char* path, const DataTransformer& transformer,
         const googlechart_dom_options& dom_options = googlechart_dom_options{})
      {
         std::ofstream file;
         file.open(path, std::ios::out);
         if (!file)
            throw std::exception();
         write_html(file, transformer, dom_options);
         file.close();
      }

   private:
      const std::string local_date_time() const
      {
         time_t now = time(0);
         struct tm tstruct;
         char buf[80];
         //tstruct = *localtime(&now);
         localtime_s(&tstruct, &now);
         strftime(buf, sizeof(buf), "%Y-%m-%dT%XZ%z", &tstruct);
         return buf;
      }
   };

   std::ostream& operator<<(std::ostream& os, googlechart::visualization type);

   template<class DataSet>
   void write(std::ostream& os, const format& fmt, size_t level,
      const googlechart& gc, const DataSet& transformer, const std::string& chart_div)
   {
      const indent ind{ fmt, level };
      const indent ind_func{ fmt, level + 1 };
      os << ind << "google.charts.load(\"current\", {packages:[\"corechart\"]});\n";
      os << ind << "google.charts.setOnLoadCallback(drawChart);\n";
      os << ind << "function drawChart() {\n";
      write(os, fmt, level + 1, transformer);
      os << '\n';
      detail::write_options(os, fmt, level + 1, gc.options);
      os << '\n';
      os << ind_func << "var chart = new google.visualization."
         << gc.type << "(document.getElementById('"
         << chart_div << "'));\n";
      os << ind_func << "chart.draw(data, options);\n";
      os << ind << "}";
   }

   struct googlechart_content
   {
      const googlechart& content;
   };

   template<class DataTransformer>
   static element::convertible make_convertible(
      const googlechart& chart, const DataTransformer& transformer, const std::string& chart_div)
   {
      return [&](std::ostream& os, const format& fmt, size_t level)
      {
         write(os, fmt, level, chart, transformer, chart_div);
      };
   }

} // namespace gb2gc

#endif // GB2GC_CHART_H