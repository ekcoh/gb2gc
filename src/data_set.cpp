// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include "data_set.h"

std::ostream& gb2gc::operator<<(
   std::ostream& os, const gb2gc::data_set::const_row_iterator& rit)
{
    auto it = rit.begin();
    const auto end = rit.end();
    os << '[';
    if (it != end)
    {
        os << *it;
        ++it;
        for (; it != end; ++it)
            os << ',' << *it;
    }
    os << ']';
    return os;
}

std::ostream& gb2gc::operator<<(
   std::ostream& os, const gb2gc::data_set& ds)
{
    const auto cols = ds.cols();
    const auto rows = ds.rows();
    os << '[';
    for (auto col = 0u; col < cols; ++col)
    {
        if (col != 0) os << ", ";
        os << ds.get_col(col).name();
    }
    os << "]\n";
    for (auto row = 0u; row < rows; ++row)
    {
        os << '[';
        for (auto col = 0u; col < cols; ++col)
        {
            if (col != 0) os << ", ";
            os << ds.get_col(col)[row];
        }
        os << "]\n";
    }
    return os;
}