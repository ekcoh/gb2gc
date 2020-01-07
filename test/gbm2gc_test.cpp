#include <gtest/gtest.h>
#include "gbm2gc.h"

#include <algorithm>

using namespace gbm2gc;

class gbm2gc_generator_test : public ::testing::Test
{
public:
	void SetUp()
	{
		
	}

	void TearDown()
	{
		//std::remove(out_file);
	}

	static constexpr char* out_file = "out_file.html";

	options opt;
	static const char* valid_args_required_first[];
	static const char* valid_args_required_last[];
};

TEST_F(gbm2gc_generator_test, run__should_be_successful__if_line_chart_and_valid_input)
{
	const char* args[] =
	{
		"gbm2gc.exe",
		"-i",
		"simple_bm.json",
		"-o",
		"line_chart.html",
		"-c", "line"
	};

	EXPECT_EQ(gbm2gc::run(7, args), 0);
}

TEST_F(gbm2gc_generator_test, run__should_be_successful__if_line_chart_and_valid_input_with_selector)
{
	const char* args[] =
	{
		"gbm2gc.exe",
		"-i",
		"simple_bm.json",
		"-o",
		"line_chart.html",
		"-c", "line",
		"-l", "right",
		"-s", "name/2", "bytes_per_second",
		"-x", "Name Param 2",
		"-y", "Bytes per second"
	};

	EXPECT_EQ(gbm2gc::run(16, args), 0);
}


TEST_F(gbm2gc_generator_test, run__should_be_successful__if_bar_chart_and_valid_input)
{
	const char* args[] =
	{
		"gbm2gc.exe",
		"-c", "bar",
		"-i",
		"window_functions.txt",
		"-o",
		"bar_chart.html",
		"-t",
		"Bar chart test title",
		"-x", "X-axis",
		"-y", "Y-axis",
		"-l", "right"
	};

	EXPECT_EQ(gbm2gc::run(15, args), 0);
}

//TEST_F(gbm2gc_generator_test, run__should_be_successful__gaze_scenario)
//{
//	const char* args[] =
//	{
//		"gbm2gc.exe",
//		"-i",
//		"gaze_scenario.txt",
//		"-f", "BM_fixation_scenario/64/*/50/50/90/10/*",
//		"-o", "gaze.html",
//		"-c", "line",
//		"-l", "right",
//		"-s", "name/2", "utilization",
//		"-x", "Scale Ratio",
//		"-y", "Cache Utilization",
//		"-t", "Cache Utilization As a Function of Scale Ratio For Varying Validity Duration"
//	};
//
//	EXPECT_EQ(gbm2gc::run(20, args), 0);
//}