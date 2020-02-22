// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifndef gb2gc_H
#define gb2gc_H

#include "chart.h"

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

namespace gb2gc
{
   static constexpr int ERROR_NO_ERROR = 0;
   static constexpr int ERROR_INVALID_ARGUMENT = 1;

   std::vector<std::string> split(const std::string& s, char delimiter);

   template<class T>
   struct span final
   {
      using iterator_type = T * ;

      iterator_type begin() { return values; }
      const iterator_type begin() const { return values; }
      iterator_type end() { return &values[count]; }
      const iterator_type end() const { return &values[count]; }

      inline const T& operator[](size_t index) const noexcept
      {
         assert(index < count && "span index out of range");
         return values[index];
      }

      T*     values;
      size_t count;
   };

   template<class T>
   inline span<T> make_span(T* ptr, size_t n)
   {
      return span<T>{ ptr, n };
   }

   // Provides the means of selecting data
   class selector final
   {
   public:
      // Constructs a selector that selects data with a regular key or of the form
      //  'BM_Identifier/<index>' where 'BM_Identifier' is the name of the benchmark
      // and <index> is the index of the benchmark parameter to be selected.
      selector(const std::string& key);

      // Selects data from the given benchmark
      gb2gc::variant operator()(const nlohmann::json& benchmark) const;

      const std::string& key() const;
      bool is_parameterized() const;

      // Returns the associated parameter index if this selector is parameterized,
      // i.e. is_parameterized() returns true, else is undefined behavior.
      unsigned param_index() const;

   private:
      std::string key_;
      unsigned param_index_;
   };

   // Provides the means of parsing and reading command-line options.
   class options
   {
   public:
      options();
      int parse(int argc, const char* argv[]);

      const std::string& in_file() const;
      const std::string& out_file() const;

      bool has_filter() const;
      const std::string& filter() const;

      const gb2gc::googlechart_dom_options& dom_options() const;
      const gb2gc::googlechart_options& chart_options() const;
      const gb2gc::googlechart::visualization chart_type() const;
      const std::vector<selector>& selectors() const;

   private:
      void print_usage(const char* cmd);
      int show_error(const std::string& message, const char* cmd = nullptr);

      int parse_size(unsigned& dst, const char* arg);
      int parse_chart_type(const char* arg);
      int parse_legend(const char* arg);
      int parse_filter(const char* arg);
      int parse_selector(const span<const char*>& args);

      std::string in_file_;
      std::string out_file_;

      std::string filter_;

      gb2gc::googlechart_options gc_options_;
      gb2gc::googlechart_dom_options gc_dom_options_;
      gb2gc::googlechart::visualization gc_type_;
      std::vector<selector> selectors_;
      static const std::vector<selector> default_selectors;
   };

   // Based on given options, parses and formats the benchmark into chart-compatible data-set
   gb2gc::data_set parse_data(const options& options, const nlohmann::json& bm_result);

   // Writes the given data-set as a chart based on given options 
   void write_chart(const options& options, const gb2gc::data_set& data_set);

   // Parses a google benchmark data file
   nlohmann::json parse_json(const std::string& file);

   // Runs the Google benchmark converter based on command-line arguments and returns
   // a system-specific error code.
   int run(int argc, const char* argv[]);

} // namespace gb2gc

#endif // gb2gc_H