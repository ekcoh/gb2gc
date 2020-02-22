// Copyright(C) 2019 - 2020 Håkan Sidenvall <ekcoh.git@gmail.com>.
// This file is subject to the license terms in the LICENSE file found in the 
// root directory of this distribution.

#include <gtest/gtest.h>

#include "dom.h" // Subject under test (SUT)

using namespace gb2gc;

class gb2gc_dom_test : public ::testing::Test
{ 
public:
    gb2gc_dom_test() : root("root") { }

   void given_basic_dom()
   {
      auto& child1 = root.add_element("child1");
      child1.add_attribute("attrib1", "value1");
      auto& child2 = root.add_element("child2").set_comment("The second child");
      child2.add_value("MyValue");
   }

   element root;
};

TEST_F(gb2gc_dom_test, children__should_return_all_children)
{
   EXPECT_TRUE(root.children().empty());
   
   given_basic_dom();
   EXPECT_EQ(root.children().size(), 2u);
   EXPECT_EQ(root.children()[0].name(), "child1");
   EXPECT_EQ(root.children()[1].name(), "child2");
}

TEST_F(gb2gc_dom_test, attributes__should_return_all_attributes)
{
   given_basic_dom();

   EXPECT_TRUE(root.attributes().empty());
   const auto child1 = root.find_child("child1");
   const auto child2 = root.find_child("child2");
   ASSERT_NE(child1, root.children().end());
   ASSERT_NE(child2, root.children().end());
   ASSERT_EQ(child1->attributes().size(), 1);
   EXPECT_EQ(child1->attributes()[0], gb2gc::element::attribute("attrib1", "value1"));
   EXPECT_EQ(child2->attributes().size(), 0);
}

TEST_F(gb2gc_dom_test, write_dom__should_convert_dom_to_formatted_string__if_valid)
{
   given_basic_dom();

   const auto expected = 
      "<root>\n"
      "  <child1 attrib1=\"value1\"></child1>\n"
      "  <!-- The second child -->\n"
      "  <child2>\n"
      "    MyValue\n"
      "  </child2>\n"
      "</root>\n";

   std::stringstream ss;
   write_dom(ss, root);

   EXPECT_STREQ(ss.str().c_str(), expected);
}

TEST_F(gb2gc_dom_test, write_dom__should_use_passed_format_and_level__if_valid)
{
   given_basic_dom();

   const auto expected =
      "    <root>\n"
      "        <child1 attrib1=\"value1\"></child1>\n"
      "        <child2>\n"
      "            MyValue\n"
      "        </child2>\n"
      "    </root>\n";

   format fmt;
   fmt.strip_comments = true;
   fmt.indentation = 4;

   std::stringstream ss;
   write_dom(ss, root, fmt, 1);

   EXPECT_STREQ(ss.str().c_str(), expected);
}

