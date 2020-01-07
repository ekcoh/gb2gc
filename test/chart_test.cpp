#include <gtest/gtest.h>

#include "chart.h"

using namespace gb2gc;

class chart_test : public ::testing::Test
{
	void SetUp()
	{ }
};

TEST_F(chart_test, ctor__should_create_empty_data_set__if_default_constructed)
{
	data_set ds;

	std::ostringstream oss;

	googlechart gc;
	//gc.write_html(oss, ds);
}
