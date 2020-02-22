// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest/gtest.h>

#include "chart.h"
#include "variant.h"

using namespace gb2gc;

std::string chart1;
std::string chart2;

class gb2gc_chart_test : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        read_file(chart1, "chart1.html");
        read_file(chart2, "chart2.html");
    }

    static void TearDownTestCase()
    {
        chart1.resize(0);
    }

    void SetUp()
    {   // These can be moved to SetUpTestCase when a new release of Google Test has been released
        // See issue: 
        ASSERT_FALSE(chart1.empty()) << "Failed to read required input file";
        ASSERT_FALSE(chart2.empty()) << "Failed to read required input file";
    }

    void TearDown()
    {
        //std::remove(path);
    }

    static void read_file(std::string& dst, const std::string& filename)
    {
        std::ifstream t(filename);
        std::string str((std::istreambuf_iterator<char>(t)),
            std::istreambuf_iterator<char>());
        dst = std::move(str);
    }

    const char* path = "chart.html";
    std::string buffer;
};

TEST_F(gb2gc_chart_test, write_html_file__should_create_html_document__if_valid_int_variant_data_set)
{
    data_set ds({ "X", "Y", "Z" });
    
    const auto n = 10;
    for (auto i = 0u; i < n; ++i)
        ds.add_row(i, 1 + i * 2, 2 + i * 2);

    googlechart gc;
    gc.type = googlechart::visualization::line;
    gc.options.title = "tuple_data_chart";
    gc.options.legend = googlechart_options::position::bottom;
    gc.options.data_opacity = 0.5f;
    gc.write_html_file(path, ds);

    read_file(buffer, path);

    EXPECT_STREQ(chart1.c_str(), buffer.c_str());
}

TEST_F(gb2gc_chart_test, write_html_file__should_create_html_document__if_valid_double_variant_data_set)
{
    data_set ds({ "X", "Y", "Z" });

    const auto n = 10;
    for (auto i = 0u; i < n; ++i)
    {
        const auto x = static_cast<double>(i);
        ds.add_row(x, 1.0 + x * 2.0, 2.0 + x * 2.0);
    }

    auto x = to_string(gb2gc::variant(1.0));
    const auto len = x.length();
    ASSERT_GE(len, 2);
    auto precision = len - 2;
    ASSERT_EQ(precision, 6);

    googlechart gc;
    gc.type = googlechart::visualization::line;
    gc.options.title = "rounding, legend and axes";
    gc.options.legend = googlechart_options::position::right;
    gc.options.horizontal_axis.title = "X";
    gc.options.vertical_axis.title = "Y";
    gc.options.data_opacity = 0.5f;
    gc.write_html_file(path, ds);

    read_file(buffer, path);

    EXPECT_STREQ(chart2.c_str(), buffer.c_str());
}

TEST_F(gb2gc_chart_test, write_axis__should_write_empty_braces__if_title_not_set)
{
    std::stringstream ss;
    gb2gc::format fmt;

    gb2gc::axis axis;
    gb2gc::detail::write_axis(ss, fmt, 0, axis);
    EXPECT_STREQ(ss.str().c_str(), "{}");
}

TEST_F(gb2gc_chart_test, write_axis__should_write_title__if_title_is_set)
{
    std::stringstream ss;
    gb2gc::format fmt;

    gb2gc::axis axis;
    axis.title = "xyz";
    gb2gc::detail::write_axis(ss, fmt, 0, axis);
    EXPECT_STREQ(ss.str().c_str(), "{ title: 'xyz' }");
}
