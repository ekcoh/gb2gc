#include <gtest/gtest.h>
#include "gbm2gc.h"
#include "redirect.h"

using namespace gbm2gc;

class gbm2gc_options_test : public ::testing::Test
{
public:
	void SetUp()
	{
		
	}

	options opt;
	static const char* valid_args_required_first[];
	static const char* valid_args_required_last[];
};

const char* gbm2gc_options_test::valid_args_required_last[] =
{
	"gbm2gc.exe",
	"-h", "720",
	"-w", "480",
	"-t", "my title",
	"-i", "input_file",
	"-o", "ouput_file",
	"-c", "scatter"
};

const char* gbm2gc_options_test::valid_args_required_first[] =
{
	"gbm2gc.exe",
	"-c", "line",
	"-i", "input_file",
	"-o", "output_file",
	"-h", "720",
	"-w", "480",
	"-t", "my title",
	"-l", "right",
	"-s", "name/2", "cpu_time"
};

TEST_F(gbm2gc_options_test, parse__should_be_successful__if_valid_args)
{
	EXPECT_EQ(opt.parse(18, gbm2gc_options_test::valid_args_required_first), 0);

	EXPECT_EQ(opt.in_file(), "input_file");
	EXPECT_EQ(opt.out_file(), "output_file");
	EXPECT_EQ(opt.chart_options().title, "my title");
	EXPECT_EQ(opt.chart_type(), html::googlechart::visualization::line);
	EXPECT_EQ(opt.dom_options().width, 480u);
	EXPECT_EQ(opt.dom_options().height, 720u);
	ASSERT_EQ(opt.selectors().size(), 2u);
	EXPECT_EQ(opt.selectors()[0].key(), "name");
	EXPECT_TRUE(opt.selectors()[0].is_parameterized());
	EXPECT_EQ(opt.selectors()[1].key(), "cpu_time");
	EXPECT_FALSE(opt.selectors()[1].is_parameterized());
}

TEST_F(gbm2gc_options_test, parse__should_succeed__if_only_specifying_required_args)
{
	EXPECT_EQ(opt.parse(7, gbm2gc_options_test::valid_args_required_first), 0);

	EXPECT_EQ(opt.in_file(), "input_file");
	EXPECT_EQ(opt.out_file(), "output_file");
	EXPECT_EQ(opt.chart_type(), html::googlechart::visualization::line);
	EXPECT_EQ(opt.chart_options().title, "");
	EXPECT_EQ(opt.dom_options().width, html::googlechart_dom_options::default_width);
	EXPECT_EQ(opt.dom_options().height, html::googlechart_dom_options::default_height);
	ASSERT_EQ(opt.selectors().size(), 3u); 
	EXPECT_EQ(opt.selectors()[0].key(), "name");
	EXPECT_FALSE(opt.selectors()[0].is_parameterized());
	EXPECT_EQ(opt.selectors()[1].key(), "cpu_time");
	EXPECT_FALSE(opt.selectors()[1].is_parameterized());
	EXPECT_EQ(opt.selectors()[2].key(), "real_time");
	EXPECT_FALSE(opt.selectors()[2].is_parameterized());
}

TEST_F(gbm2gc_options_test, parse__should_fail__if_zero_args)
{
	EXPECT_NE(opt.parse(0, gbm2gc_options_test::valid_args_required_first), 0);
}

TEST_F(gbm2gc_options_test, parse__should_fail__if_nullptr_argv)
{
	EXPECT_NE(opt.parse(0, nullptr), 0);
}

TEST_F(gbm2gc_options_test, parse__should_fail__if_missing_required_arg)
{
	EXPECT_NE(opt.parse(3, gbm2gc_options_test::valid_args_required_first), 0);
}

// TODO Setup stream redirect to verify error messages

