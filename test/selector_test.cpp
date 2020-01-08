#include <gtest/gtest.h>

#include "gbm2gc.h"

using namespace gbm2gc;

const char* benchmark1 = "benchmark1.json";

class gbm2gc_selector_test : public ::testing::Test
{ };

TEST_F(gbm2gc_selector_test, 
	is_parameterized__should_be_true__if_parameterized)
{
	EXPECT_TRUE(selector("BM_MyBenchmark/1").is_parameterized());
}

TEST_F(gbm2gc_selector_test, 
	is_parameterized__should_be_false__if_not_parameterized)
{
	EXPECT_FALSE(selector("BM_MyBenchmark").is_parameterized());
}

TEST_F(gbm2gc_selector_test, 
	param_index__should_correspond_to_param_index_in_key__if_parameterized)
{
	EXPECT_EQ(selector("BM_SetInsert/1").param_index(), 1u);
	EXPECT_EQ(selector("BM_SetInsert/2").param_index(), 2u);
}

TEST_F(gbm2gc_selector_test, 
	function_operator__should_return_indexed_parameter_value__if_valid_benchmark)
{
	auto bm_json = parse_json(benchmark1);
	auto benchmarks = bm_json.find("benchmarks");

	selector s1("name/1");
	auto s1_param_value = s1(*benchmarks->begin());
	EXPECT_EQ(s1_param_value.get<long double>(), 1024.0L);

	selector s2("name/2");
	auto s2_param_value = s2(*benchmarks->begin());
	EXPECT_EQ(s2_param_value.get<long double>(), 1.0L);
}

TEST_F(gbm2gc_selector_test,
	function_operator__should_return_stripped_parameter_value__if_valid_benchmark_and_key_is_name)
{
	auto bm_json = parse_json(benchmark1);
	auto benchmarks = bm_json.find("benchmarks");

	selector s1("name");
	auto s1_param_value = s1(*benchmarks->begin());
	EXPECT_EQ(s1_param_value.get<std::string>(), std::string("SetInsert/1024/1/50"));
}

TEST_F(gbm2gc_selector_test,
	function_operator__should_return_parameter_value__if_valid_benchmark_and_regular_key)
{
	auto bm_json = parse_json(benchmark1);
	auto benchmarks = bm_json.find("benchmarks");

	selector s1("cpu_time");
	auto s1_param_value = s1(*benchmarks->begin());
	EXPECT_EQ(s1_param_value.get<long double>(), 29836.0L);
}

TEST_F(gbm2gc_selector_test,
	function_operator__should_throw__if_non_existent_key)
{
	auto bm_json = parse_json(benchmark1);
	auto benchmarks = bm_json.find("benchmarks");

	selector s("non_existent/1");
	EXPECT_THROW(s(*benchmarks->begin()), std::runtime_error);
}
