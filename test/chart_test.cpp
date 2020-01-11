#include <gtest/gtest.h>

#include "chart.h"

using namespace gb2gc;

class chart_test : public ::testing::Test
{
	void SetUp()
	{ }
};

TEST_F(chart_test, xxx)
{
   const char path[] = "mytchart.html";

   // dummy data
   std::vector<std::tuple<double, double, double>> data;
   std::vector<std::tuple<double, double>> data1;
   std::vector<std::tuple<double, double>> data2;

   // create tuple data set
   tuple_data_set<double, double, double> tuple_data;// { "X", "Y", "Z" });
   tuple_data.series()[0] = "X";
   tuple_data.series()[1] = "Y";
   tuple_data.series()[2] = "Z";

   //std::tuple<double, double> t;
   //auto x = std::get<0>(t);

   // generate random data
   const auto n = 20;
   std::default_random_engine rand;
   std::normal_distribution<double> dist(0, 10);
   for (auto i = 0u; i < n; ++i)
   {
      const auto x = i;
      const auto y = dist(rand);
      const auto z = dist(rand);
      tuple_data.data().push_back(std::make_tuple(i, y, z));
      data.push_back(std::make_tuple(i, y, z));
      data1.push_back(std::make_tuple(i, y));
      data1.push_back(std::make_tuple(i, z));
   }

   auto view = make_data_view(data);

   googlechart gc;
   gc.type = googlechart::visualization::line;
   gc.options.title = "Probing iterations";
   gc.options.legend = googlechart_options::position::bottom;
   gc.options.data_opacity = 0.5f;
   gc.write_html_file(path, tuple_data);
   //gc.write_html_file(path, make_tuple_transformer(data));

   //system(path);
}