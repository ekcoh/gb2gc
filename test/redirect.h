// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#ifdef _MSC_VER
#pragma once    // Improves build time on MSVC
#endif

#ifndef GB2GC_REDIRECT_H
#define GB2GC_REDIRECT_H

#include <ostream>

// Simple ostream redirect RAII guard
struct redirect_guard
{
   redirect_guard(std::ostream& target, std::streambuf * new_buffer)
      : target(target), old(target.rdbuf(new_buffer))
   { }

   ~redirect_guard()
   {
      target.rdbuf(old);
   }

private:
   std::ostream&    target;
   std::streambuf * old;
};

#endif // GB2GC_REDIRECT_H