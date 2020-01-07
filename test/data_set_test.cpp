#include <gtest/gtest.h>
#include "data_set.h"

using namespace gb2gc;

class data_set_test : public ::testing::Test
{
	void SetUp()
	{ }
};

TEST_F(data_set_test, ctor__should_create_empty_data_set__if_default_constructed)
{
	data_set ds;

	EXPECT_EQ(ds.cols(), 0u);
	EXPECT_EQ(ds.rows(), 0u);
	EXPECT_TRUE(ds.empty());
}

TEST_F(data_set_test, usage__replace_with_better_tests)
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
