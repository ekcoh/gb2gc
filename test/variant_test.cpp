#include <gtest/gtest.h>

#include "variant.h"

using namespace gb2gc;

template<class Char, class Traits = std::char_traits<Char>, class Allocator = std::allocator<Char>>
void reset(std::basic_stringstream<Char, Traits, Allocator>& ss)
{
   std::basic_stringstream<Char, Traits, Allocator> temp; // require default constructible
   ss.swap(temp);
}

class gbm2gc_variant_test : public ::testing::Test
{ };

TEST_F(gbm2gc_variant_test,
   to_string__should_convert_variant_to_string__if_valid)
{
   EXPECT_EQ(to_string(null_value()), "'null'");
   EXPECT_EQ(to_string(variant(1234u)), "1234");
}

template<class T>
void assert_string_conversion(std::stringstream& ss, T value)
{
   ss << variant(T(value));
   EXPECT_EQ(ss.str(), std::to_string(T(value)));
   reset(ss);
}

TEST_F(gbm2gc_variant_test, 
   output_stream_operator__should_convert_variant_to_string__if_valid)
{
   std::stringstream ss;

   ss << variant();
   EXPECT_EQ(ss.str(), "'null'");
   reset(ss);

   assert_string_conversion<unsigned short>(ss, 123U);
   assert_string_conversion<unsigned int>(ss, 123U);
   assert_string_conversion<unsigned long>(ss, 123UL);
   assert_string_conversion<unsigned long long>(ss, 123ULL);

   assert_string_conversion<short>(ss, 123);
   assert_string_conversion<int>(ss, 123);
   assert_string_conversion<long>(ss, 123L);
   assert_string_conversion<long long>(ss, 123LL);

   assert_string_conversion<float>(ss, 1.23456789f);
   assert_string_conversion<double>(ss, 1.23456789);
   assert_string_conversion<long double>(ss, 1.23456789L);
}