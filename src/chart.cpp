// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution..

#include "chart.h"

std::ostream& gb2gc::operator<<(std::ostream& os, const color& color)
{  // format color as #rrggbb
   os << '#' << std::hex << color.r << color.g << color.b;
   return os;
}

std::ostream&
gb2gc::operator<<(std::ostream& os, gb2gc::googlechart_options::position pos)
{
   switch (pos)
   {
   case googlechart_options::position::left:
      os << "left";
      break;
   case googlechart_options::position::right:
      os << "right";
      break;
   case googlechart_options::position::top:
      os << "top";
      break;
   case googlechart_options::position::bottom:
      os << "bottom";
      break;
   case googlechart_options::position::none:
   default:
      os << "none";
      break;
   }
   return os;
}

std::ostream&
gb2gc::operator<<(std::ostream& os, googlechart_options::curve curve)
{
   switch (curve)
   {
   case googlechart_options::curve::function:
      os << "function";
      break;
   case googlechart_options::curve::none:
   default:
      os << "none";
      break;
   }
   return os;
}

std::ostream&
gb2gc::operator<<(std::ostream& os, googlechart::visualization type)
{
   switch (type)
   {
   case googlechart::visualization::scatter:
      os << "ScatterChart";
      break;
   case googlechart::visualization::line:
      os << "LineChart";
      break;
   case googlechart::visualization::histogram:
      os << "Histogram";
      break;
   case googlechart::visualization::bar:
      os << "BarChart";
      break;
   default:
      throw std::invalid_argument("invalid Google Chart visualization type");
   }
   return os;
}

void gb2gc::detail::write_axis(std::ostream& os, const format& /*fmt*/, 
   size_t /*level*/, const axis& axis)
{
    os << "{";
    bool write = false;
    if (!axis.title.empty())
    {
        if (write) os << ",";
        os << " title: '" << axis.title << '\'';
        write = true;
    }
    if (axis.min_value.index() != 0)
    {
        if (write) os << ",";
        os << " minValue: " << axis.min_value;
        write = true;
    }
    if (axis.max_value.index() != 0)
    {
        if (write) os << ",";
        os << " maxValue: " << axis.max_value;
        write = true;
    }
    os << " }";
}

void gb2gc::detail::write_options(std::ostream& os, const format& fmt, 
   size_t level, const googlechart_options& opt)
{
   const indent ind{ fmt, level };
   const indent ind_opt{ fmt, level + 1 };
   os << ind << "var options = {\n";
   os << ind_opt << "hAxis: ";
   write_axis(os, fmt, level, opt.horizontal_axis);
   os << ",\n";
   os << ind_opt << "vAxis: ";
   write_axis(os, fmt, level, opt.vertical_axis);
   os << ",\n";
   if (!opt.title.empty())
      os << ind_opt << "title: '" << opt.title << "',\n";
   os << ind_opt << "legend: { position: '" << opt.legend << "' },\n";
   if (!opt.colors.empty())
      os << ind_opt << "colors: [ '#01beff', '#3af2a2' ],\n";
   if (opt.curve_type != googlechart_options::curve::none)
      os << ind_opt << "curveType: '" << opt.curve_type << "',\n";
   if (!opt.font_name.empty())
      os << ind_opt << "fontName: '" << opt.font_name << "',\n";
   if (opt.data_opacity != 1.0f)
      os << ind_opt << "dataOpacity: " << opt.data_opacity << ",\n";
   if (opt.interpolate_nulls)
      os << ind_opt << "interpolateNulls: " << opt.interpolate_nulls << ",\n";
   if (opt.point_size != 0.0f)
      os << ind_opt << "pointSize: " << opt.point_size << ",\n";
   os << ind << "};\n";
}

template<>
void gb2gc::write<gb2gc::data_set>(std::ostream& os, const format& fmt, 
   size_t level, const data_set& ds)
{
   const indent ind{ fmt, level };
   const indent ind_label{ fmt, level + 1 };
   os << ind << "var data = google.visualization.arrayToDataTable([\n";
   os << ind_label << '[';

   {	// format series
      auto it = ds.col_begin();
      if (it != ds.col_end())
         os << '\'' << it->name() << '\'';
      while (++it != ds.col_end())
         os << ", '" << it->name() << '\'';
      os << "],\n";
   }

   {	// format values
      for (auto it = ds.row_begin(); it != ds.row_end(); )
      {
         os << ind_label << it;
         if (++it == ds.row_end())
            break;
         os << ",\n";
      }
   }

   os << "]);\n";
}