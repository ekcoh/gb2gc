// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest/gtest.h>
#include "gb2gc.h"

#include <algorithm>

using namespace gb2gc;

class gb2gc_generator_test : public ::testing::Test
{
public:
   void SetUp()
   {
      file = "out_file" + std::to_string(++file_name_no);
   }

   void TearDown()
   {
      std::remove(file.c_str());
   }

   bool file_exists()
   {
      std::ifstream f(file);
      return f.good();
   }

   options         opt;
   std::string     file;
   static unsigned file_name_no;
};

unsigned gb2gc_generator_test::file_name_no = 0u;

TEST_F(gb2gc_generator_test, run__should_be_successful__if_line_chart_and_valid_input)
{
   const char* args[] =
   {
      "gb2gc.exe",
      "-i",
      "benchmark1.json",
      "-o",
      file.c_str(),
      "-c", "line"
   };

   EXPECT_EQ(gb2gc::run(7, args), 0);
   EXPECT_TRUE(file_exists());
}

TEST_F(gb2gc_generator_test, run__should_be_successful__if_line_chart_and_valid_input_with_selector)
{
   const char* args[] =
   {
      "gb2gc.exe",
      "-i",
      "benchmark1.json",
      "-o",
      file.c_str(),
      "-c", "line",
      "-l", "right",
      "-s", "name/2", "bytes_per_second",
      "-x", "Name Param 2",
      "-y", "Bytes per second"
   };

   EXPECT_EQ(gb2gc::run(16, args), 0);
   EXPECT_TRUE(file_exists());
}


TEST_F(gb2gc_generator_test, run__should_be_successful__if_bar_chart_and_valid_input)
{
   const char* args[] =
   {
      "gb2gc.exe",
      "-c", "bar",
      "-i",
      "benchmark2.json",
      "-o",
      file.c_str(),
      "-t",
      "Bar chart test title",
      "-x", "X-axis",
      "-y", "Y-axis",
      "-l", "right"
   };

   EXPECT_EQ(gb2gc::run(15, args), 0);
   EXPECT_TRUE(file_exists());
}

