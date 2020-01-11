// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifndef GB2GC_CHART_VARIANT_H
#define GB2GC_CHART_VARIANT_H

#include <nonstd/variant.hpp>

#include <iostream>
#include <string>
#include <sstream>
#include <exception>

namespace gb2gc
{
   using null_type = nonstd::monostate;

   inline constexpr nonstd::monostate null_value() { return {}; }

   using variant = nonstd::variant<
      null_type,           // 0
      short,               // 1
      unsigned short,      // 2
      int,                 // 3
      unsigned int,        // 4
      long,                // 5
      unsigned long,       // 6
      long long,           // 7
      unsigned long long,  // 8
      float,               // 9
      double,              // 10
      long double,         // 11
      std::string>;        // 12

   inline std::ostream& operator<<(std::ostream & os, gb2gc::variant const & v)
   {
      switch (v.index())
      {
      case 0:  os << "'null'"; break;
      case 1:  os << std::to_string(nonstd::get<1>(v)); break;
      case 2:  os << std::to_string(nonstd::get<2>(v)); break;
      case 3:  os << std::to_string(nonstd::get<3>(v)); break;
      case 4:  os << std::to_string(nonstd::get<4>(v)); break;
      case 5:  os << std::to_string(nonstd::get<5>(v)); break;
      case 6:  os << std::to_string(nonstd::get<6>(v)); break;
      case 7:  os << std::to_string(nonstd::get<7>(v)); break;
      case 8:  os << std::to_string(nonstd::get<8>(v)); break;
      case 9:  os << std::to_string(nonstd::get<9>(v)); break;
      case 10: os << std::to_string(nonstd::get<10>(v)); break;
      case 11: os << std::to_string(nonstd::get<11>(v)); break;
      case 12: os << '\'' << nonstd::get<12>(v) << '\''; break;
      default:
         throw std::out_of_range("variant index out of range");
      }
      return os;
   }

   inline std::string to_string(const gb2gc::variant& value)
   {
      std::stringstream ss;
      ss << value;
      return ss.str();
   }

} // namespace gb2gc

#endif // GB2GC_CHART_VARIANT_H