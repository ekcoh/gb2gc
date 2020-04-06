// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest/gtest.h>
#include "data_set.h"

#include <vector>
#include <array>
#include <unordered_map>

using namespace gb2gc;

class gb2gc_data_set_test : public ::testing::Test { };

TEST_F(gb2gc_data_set_test, is_iterator__should_return_true__if_type_is_iterator)
{
   EXPECT_TRUE((gb2gc::is_iterator< std::vector<int>::iterator >::value));
   EXPECT_TRUE((gb2gc::is_iterator< std::array<short, 5>::iterator >::value));
   EXPECT_TRUE((gb2gc::is_iterator< std::unordered_map<long, int>::iterator >::value));
   EXPECT_FALSE((gb2gc::is_iterator< std::vector<int> > ::value));
}

TEST_F(gb2gc_data_set_test, ctor__should_create_empty_data_set__if_default_constructed)
{
   data_set ds;

   EXPECT_EQ(ds.cols(), 0u);
   EXPECT_EQ(ds.rows(), 0u);
   EXPECT_TRUE(ds.empty());
}

TEST_F(gb2gc_data_set_test, usage__replace_with_better_tests)
{
   data_set ds;
   ds.resize_cols(3);
   ds.add_row(0, 1, 2);
   ds.add_row(1, 3, 4);
   ds.add_row(2, 5, 6);

   auto it = ds.row_begin();

   EXPECT_FALSE(it == ds.row_end());
   EXPECT_EQ(it[0].get<int>(), 0);
   EXPECT_EQ(it[1].get<int>(), 1);
   EXPECT_EQ(it[2].get<int>(), 2);

   ++it;
   EXPECT_FALSE(it == ds.row_end());
   EXPECT_EQ(it[0].get<int>(), 1);
   EXPECT_EQ(it[1].get<int>(), 3);
   EXPECT_EQ(it[2].get<int>(), 4);

   ++it;
   EXPECT_FALSE(it == ds.row_end());
   EXPECT_EQ(it[0].get<int>(), 2);
   EXPECT_EQ(it[1].get<int>(), 5);
   EXPECT_EQ(it[2].get<int>(), 6);

   ++it;
   EXPECT_TRUE(it == ds.row_end());
}

