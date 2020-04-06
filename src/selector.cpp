// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include "gb2gc.h"
#include "variant.h"

namespace {
    static constexpr auto param_none = (std::numeric_limits<unsigned>::max)();
}

gb2gc::selector::selector(const std::string& name) :
   key_(name), param_index_(param_none)
{
   auto splits = split(name, '/');
   if (splits.size() == 2)
   {
      key_ = splits[0];
      param_index_ = std::stol(splits[1]);
   }
}

gb2gc::variant
gb2gc::selector::operator()(const nlohmann::json& benchmark) const
{
   auto node = benchmark.find(key_);
   if (node == benchmark.end())
      throw std::runtime_error("Benchmark do not have any key '" + key_ + "'");
   if (node->is_string())
   {
      // extract benchmark parameter if parameterized selector
      if (is_parameterized())
      {
         auto splits = split(node->get<std::string>(), '/');
         auto param = splits[param_index_];
         auto param_value = std::stold(param);
         return gb2gc::variant(param_value);
      }

      // strip prefix
      auto s = node->get<std::string>();
      const auto prefix = std::string("BM_");
      if (s.rfind(prefix, 0) == 0)
      {
         const auto plen = prefix.length();
         return gb2gc::variant(s.substr(plen, s.length() - plen));
      }

      return gb2gc::variant(s);
   }
   return gb2gc::variant(node->get<long double>());
}

const std::string&
gb2gc::selector::key() const
{
   return key_;
}

bool
gb2gc::selector::is_parameterized() const
{
   return param_index_ != param_none;
}

unsigned
gb2gc::selector::param_index() const
{
   return param_index_;
}

std::vector<std::string>
gb2gc::split(const std::string& s, char delimiter)
{
   std::vector<std::string> tokens;
   if (!s.empty())
   {
      std::string token;
      std::istringstream tokenStream(s);
      while (std::getline(tokenStream, token, delimiter))
         tokens.push_back(token);
   }
   return tokens;
}
