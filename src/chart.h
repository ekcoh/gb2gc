// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifdef _MSC_VER
#pragma once    // Improves build time on MSVC
#endif

#ifndef GB2GC_CHART_H
#define GB2GC_CHART_H

#include <array>
#include <ctime>
#include <fstream>
#include <functional>
#include <random>
#include <string>
#include <vector>

#include "dom.h"
#include "data_set.h"

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

   ////////////////////////////////////////////////////////////////////////////
   // axis

   struct axis
   {
      std::string title;
      variant     min_value;
      variant     max_value;
   };

   ////////////////////////////////////////////////////////////////////////////
   // googlechart_dom_options

   struct googlechart_dom_options
   {
      static constexpr unsigned default_width = 900;
      static constexpr unsigned default_height = 500;

      unsigned width = default_width;
      unsigned height = default_height;
      std::string div = "chart_div";
      bool include_meta_timestamp = false;
   };

   ////////////////////////////////////////////////////////////////////////////
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
      void write_axis(std::ostream& os, const format& fmt, size_t level, const axis& axis);
      void write_options(std::ostream& os, const format& fmt, size_t level, const googlechart_options& opt);
      void write_data_set(std::ostream& os, const format& fmt, size_t level, const data_set& ds);
   }

   ////////////////////////////////////////////////////////////////////////////
   // googlechart

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
      visualization type = visualization::histogram;

      template<class DataSet>
      void write_html(std::ostream& os, const DataSet& transformer,
         const googlechart_dom_options& dom_options = googlechart_dom_options{})
      {
         element html("html");

         auto& head = html.add_element("head");
         if (dom_options.include_meta_timestamp)
         {
             head.add_element("meta")
                 .add_attribute("name", "timestamp")
                 .add_attribute("timestamp", local_date_time());
         }
         head.add_element("script")
            .add_attribute("type", "text/javascript")
            .add_attribute("src", "https://www.gstatic.com/charts/loader.js");
         head.add_element("script")
            .add_attribute("type", "text/javascript")
            .set_content(make_convertible(*this, transformer, dom_options.div));

         auto& body = html.add_element("body");
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
       std::string local_date_time() const;
   };

   std::ostream& operator<<(std::ostream& os, googlechart::visualization type);

   void write(std::ostream& os, const format& fmt, size_t level,
       const googlechart& gc, const data_set& data_set, const std::string& chart_div);

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