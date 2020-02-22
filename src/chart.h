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

// TODO Consider https://stackoverflow.com/questions/41893055/add-subtitle-in-areachart-in-google-chart

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
      // hAxis: {title: 'Age', minValue : 0, maxValue : 15},
      // vAxis : {title: 'Weight', minValue : 0, maxValue : 15},
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

   template<class T>
   struct default_transformer
   {
      void operator()(std::ostream& os, const T& value, size_t)
      {
         os << value;
      }
   };

   //template<class T1, class... Rest>
   //struct tuple_transformer 
   //{ 
   //	void operator()(std::ostream& os, const std::tuple<T1, Rest...>& value, size_t series)
   //	{
   //		return tuple_transformer<Rest>().operator(os, value, series);
   //	}
   //};



   /*template<class Tuple, typename = std::enable_if<std::tuple_size<Tuple>::value == 1>::type>
   struct tuple_transformer
   {
      void operator()(std::ostream& os, const Tuple& value, size_t series)
      {
         switch (series)
         {
         case 0: os << std::get<0>(value); break;
         default:
            throw std::exception();
         }
      }
   };*/

   template<class Tuple, size_t Index>
   struct tuple_formatter
   {
      static inline void format(std::ostream& os, const Tuple& value, size_t series)
      {
         if (series == Index)
         {
            os << std::get<Index>(value);
            return;
         }
         tuple_formatter<Tuple, Index - 1>::format(os, value, series);
      }
   };

   template<class Tuple>
   struct tuple_formatter<Tuple, 0>
   {
      size_t begin() const { return 0; }
      size_t end() const { return 3; }

      static inline void format(std::ostream& os, const Tuple& value, size_t series)
      {
         os << std::get<0>(value);
      }
   };

   template<class Tuple>
   struct tuple_transformer
   {
      size_t begin() const { return 0; }
      size_t end() const { return 3; }

      void operator()(std::ostream& os, const Tuple& value, size_t series)
      {
         tuple_formatter<Tuple, std::tuple_size<Tuple>::value - 1>::format(os, value, series);
      }
   };

   template<class Container>
   tuple_transformer<typename Container::value_type> make_tuple_transformer(const Container&)
   {
      return tuple_transformer<typename Container::value_type>();
   }

   template<class Container, bool Fixed>
   class series_data_set_base
   {
   protected:
      using series_container_type = Container;
   public:
      using series_iterator = typename series_container_type::iterator;
      using const_series_iterator = typename series_container_type::const_iterator;

      const_series_iterator begin() const { return series_.begin(); }
      const_series_iterator end() const { return series_.end(); }
      series_iterator begin() { return series_.begin(); }
      series_iterator end() { return series_.end(); }
   private:
      series_container_type series_;
   };

   struct series
   {
   public:
      explicit series(const char* name) : name_(name) { }
      explicit series(const std::string& name) : name_(name) { }
      virtual ~series() { }
      const std::string& name() const { return name_; }
   private:
      std::string name_;
   };

   template<class T>
   struct typed_series : public series
   {
   public:
      explicit typed_series(const char* name) : series(name) { }
      explicit typed_series(const std::string& name) : series(name) { }
   };

   //class dynamic_series_data_set
   //{
   //public:
   //	using container = std::vector<series>;
   //	const container& series() const { return series_; }
   //	container& series() { return series_; }
   //private:
   //	container series_;
   //};

   //class dynamic_data_view
   //{
   //public:

   //};

   //enum class value_type_t
   //{
   //	string,
   //	number
   //};

   //struct value_t
   //{
   //	union value 
   //	{
   //		std::string s;
   //		int i;
   //		unsigned ui;
   //		double d;
   //		long double ld;
   //		long l;
   //		long long ll;
   //	};
   //	value_type_t type;
   //};

   //class dynamic_dataset_base
   //{

   //};

   template<class... ColumnTypes>
   class fixed_dataset_base
   {
   public:
      using value_type = std::tuple<ColumnTypes...>;
      using series_container_type = std::array<std::string, std::tuple_size<value_type>::value>;

   protected:
      fixed_dataset_base()
      {
         std::generate(series_.begin(), series_.end(),
            [n = 0]() mutable { return "series" + std::to_string(n++); });
      }

      //fixed_dataset_base(std::initializer_list<const char*>&& list)
      //	: series_(std::forward<std::initializer_list<const char*>>(list))
      //{

      //}

      //template<class... Args>
      //fixed_dataset_base(Args&&... args) : series_({ std::forward<Args>(args)... }) { }

   public:
      fixed_dataset_base(const fixed_dataset_base&) = delete;
      fixed_dataset_base& operator=(const fixed_dataset_base&) = delete;
      virtual ~fixed_dataset_base() { }

      series_container_type& series() { return series_; }
      const series_container_type& series() const { return series_; }

   private:
      series_container_type series_;
   };

   template<class... ColumnTypes>
   class tuple_data_set : public fixed_dataset_base<ColumnTypes...>
   {
   public:
      //using value_type = std::tuple<ColumnTypes...>;
      using data_container_type = std::vector<value_type>;
      //using series_container_type = std::array<std::string, std::tuple_size<value_type>::value>;

      tuple_data_set() : fixed_dataset_base() { }

      //tuple_data_set(std::initializer_list<std::string>&& list)
      //	: fixed_dataset_base(std::forward<std::initializer_list<std::string>>(list))
      //{ }

      //template<class... Args> 
      //tuple_data_set(Args&&... args) 
      //	: fixed_dataset_base<Args...>(std::forward<Args>(args)...)
      //{ }

      virtual ~tuple_data_set() = default;
      tuple_data_set(tuple_data_set&&) = default;
      tuple_data_set& operator=(tuple_data_set&&) = default;

      data_container_type& data() { return values_; }
      const data_container_type& data() const { return values_; }

   private:
      data_container_type   values_;
   };

   struct column_series
   {
      virtual void format(std::ostream& os, size_t pos) = 0;
   };

   class column_data_set
   {
      std::vector<column_series> series;
   };

   template<class TupleContainer>
   class tuple_data_view
   {
   public:
      using value_type = typename TupleContainer::value_type;
      using data_container_type = const TupleContainer&;



      data_container_type& container_;
   };

   template<class Container>
   class tuple_adapter_data_view : public fixed_dataset_base<typename Container::value_type>
   {
   public:
      using value_type = std::tuple<typename Container::value_type>;
      using data_container_type = Container;

      virtual ~tuple_adapter_data_view() = default;
      tuple_adapter_data_view(tuple_adapter_data_view&&) = default;
      tuple_adapter_data_view& operator=(tuple_adapter_data_view&&) = default;

      data_container_type& data() { return values_; }
      const data_container_type& data() const { return values_; }

      data_container_type& container_;
   };

   template<class Container>
   tuple_data_view<Container> make_data_view(const Container& container)
   {
      return tuple_data_view<Container>{ container };
   }

   template<class Container>
   tuple_adapter_data_view<Container> make_adapter_data_view(const Container& container)
   {
      return tuple_adapter_data_view<Container>{ container };
   }

} // namespace gb2gc

#endif // GB2GC_CHART_H