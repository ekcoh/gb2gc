// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest/gtest.h>

#include "chart.h"
#include "variant.h"

using namespace gb2gc;

std::string line;
std::string bar;
std::string histogram;

class gb2gc_chart_test : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        read_file(line, "line.html");
        read_file(bar, "bar.html");
        read_file(histogram, "histogram.html");

        ASSERT_FALSE(line.empty()) << "Failed to read required input file";
        ASSERT_FALSE(bar.empty()) << "Failed to read required input file";
        ASSERT_FALSE(histogram.empty()) << "Failed to read required input file";
    }

    static void TearDownTestCase()
    {
        line.resize(0);
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

TEST_F(gb2gc_chart_test, write_html_file__should_create_line_chart_html_document__if_valid_int_variant_data_set)
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

    EXPECT_STREQ(line.c_str(), buffer.c_str());
}

TEST_F(gb2gc_chart_test, write_html_file__should_create_bar_chart_html_document__if_valid_double_variant_data_set)
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
    ASSERT_GE(len, 2u);
    auto precision = len - 2;
    ASSERT_EQ(precision, 6u);

    googlechart gc;
    gc.type = googlechart::visualization::bar;
    gc.options.title = "rounding, legend and axes";
    gc.options.legend = googlechart_options::position::right;
    gc.options.horizontal_axis.title = "X";
    gc.options.vertical_axis.title = "Y";
    gc.options.data_opacity = 0.5f;
    gc.write_html_file(path, ds);

    read_file(buffer, path);

    EXPECT_STREQ(bar.c_str(), buffer.c_str());
}

TEST_F(gb2gc_chart_test, write_html_file__should_create_histogram_chart_html_document__if_valid_double_variant_data_set)
{
    data_set ds({ "X", "Y" });

    const auto n = 10;
    for (auto i = 0u; i < n; ++i)
    {
        const auto x = static_cast<double>(i);
        ds.add_row(x, 1.0 + x * 2.0);
    }

    auto x = to_string(gb2gc::variant(1.0));
    const auto len = x.length();
    ASSERT_GE(len, 2u);
    auto precision = len - 2;
    ASSERT_EQ(precision, 6u);

    googlechart gc;
    gc.type = googlechart::visualization::histogram;
    gc.options.horizontal_axis.min_value = -5;
    gc.options.horizontal_axis.max_value = 10;
    gc.options.vertical_axis.min_value = -5;
    gc.options.vertical_axis.max_value = 10;
    gc.options.title = "rounding, legend and axes";
    gc.options.legend = googlechart_options::position::left;
    gc.options.data_opacity = 0.5f;
    gc.write_html_file(path, ds);

    read_file(buffer, path);

    EXPECT_STREQ(histogram.c_str(), buffer.c_str());
}

TEST_F(gb2gc_chart_test, write_axis__should_write_empty_braces__if_title_not_set)
{
    std::stringstream ss;
    gb2gc::format fmt;

    gb2gc::axis axis;
    gb2gc::detail::write_axis(ss, fmt, 0, axis);
    EXPECT_STREQ(ss.str().c_str(), "{ }");
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

TEST_F(gb2gc_chart_test, write_axis__should_write_range_limites__if_min_max_is_set)
{
    std::stringstream ss;
    gb2gc::format fmt;

    gb2gc::axis axis;
    axis.min_value = -5;
    axis.max_value = 15;
    gb2gc::detail::write_axis(ss, fmt, 0, axis);
    EXPECT_STREQ(ss.str().c_str(), "{ minValue: -5, maxValue: 15 }");
}