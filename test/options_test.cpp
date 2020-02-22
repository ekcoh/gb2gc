// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest/gtest.h>
#include "gb2gc.h"
#include "redirect.h"

using namespace gb2gc;

class gb2gc_options_test : public ::testing::Test
{
public:
   void SetUp()
   {
      // Currently empty
   }

   options opt;
   static const char* valid_args_required_first[];
   static const char* valid_args_required_last[];
};

const char* gb2gc_options_test::valid_args_required_last[] =
{
   "gb2gc",
   "-h", "720",
   "-w", "480",
   "-t", "my title",
   "-i", "input_file",
   "-o", "ouput_file",
   "-c", "scatter"
};

const char* gb2gc_options_test::valid_args_required_first[] =
{
   "gb2gc",
   "-c", "line",
   "-i", "input_file",
   "-o", "output_file",
   "-h", "720",
   "-w", "480",
   "-t", "my title",
   "-l", "right",
   "-s", "name/2", "cpu_time"
};

TEST_F(gb2gc_options_test, parse__should_be_successful__if_valid_args)
{
   EXPECT_EQ(opt.parse(18, gb2gc_options_test::valid_args_required_first), 0);

   EXPECT_EQ(opt.in_file(), "input_file");
   EXPECT_EQ(opt.out_file(), "output_file");
   EXPECT_EQ(opt.chart_options().title, "my title");
   EXPECT_EQ(opt.chart_type(), gb2gc::googlechart::visualization::line);
   EXPECT_EQ(opt.dom_options().width, 480u);
   EXPECT_EQ(opt.dom_options().height, 720u);
   ASSERT_EQ(opt.selectors().size(), 2u);
   EXPECT_EQ(opt.selectors()[0].key(), "name");
   EXPECT_TRUE(opt.selectors()[0].is_parameterized());
   EXPECT_EQ(opt.selectors()[1].key(), "cpu_time");
   EXPECT_FALSE(opt.selectors()[1].is_parameterized());
}

TEST_F(gb2gc_options_test, parse__should_succeed__if_only_specifying_required_args)
{
   EXPECT_EQ(opt.parse(7, gb2gc_options_test::valid_args_required_first), 0);

   EXPECT_EQ(opt.in_file(), "input_file");
   EXPECT_EQ(opt.out_file(), "output_file");
   EXPECT_EQ(opt.chart_type(), gb2gc::googlechart::visualization::line);
   EXPECT_EQ(opt.chart_options().title, "");
   EXPECT_EQ(opt.dom_options().width, gb2gc::googlechart_dom_options::default_width);
   EXPECT_EQ(opt.dom_options().height, gb2gc::googlechart_dom_options::default_height);
   ASSERT_EQ(opt.selectors().size(), 3u);
   EXPECT_EQ(opt.selectors()[0].key(), "name");
   EXPECT_FALSE(opt.selectors()[0].is_parameterized());
   EXPECT_EQ(opt.selectors()[1].key(), "cpu_time");
   EXPECT_FALSE(opt.selectors()[1].is_parameterized());
   EXPECT_EQ(opt.selectors()[2].key(), "real_time");
   EXPECT_FALSE(opt.selectors()[2].is_parameterized());
}

TEST_F(gb2gc_options_test, parse__should_fail__if_zero_args)
{
   EXPECT_NE(opt.parse(0, gb2gc_options_test::valid_args_required_first), 0);
}

TEST_F(gb2gc_options_test, parse__should_fail__if_nullptr_argv)
{
   EXPECT_NE(opt.parse(0, nullptr), 0);
}

TEST_F(gb2gc_options_test, parse__should_fail__if_missing_required_arg)
{
   EXPECT_NE(opt.parse(3, gb2gc_options_test::valid_args_required_first), 0);
}

// TODO Setup stream redirect to verify error messages

