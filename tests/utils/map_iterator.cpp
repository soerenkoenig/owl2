#include "owl/utils/map_iterator.hpp"
#include "catch/catch.hpp"
#include <iostream>

namespace test
{
  TEST_CASE( "map_iterator", "[utils]" )
  {
    using namespace owl::utils;
    std::vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7};
  
    auto sqr = [](int x){ return x*x; };
    auto it = make_map_iterator(sqr, numbers.begin());
  
    CHECK(*it == 0);
    CHECK(*(++it) == 1);
    CHECK(*(++it) == 4);
  
    auto squared_numbers = map_range(sqr, numbers);
    int  x = squared_numbers[3];
    CHECK(squared_numbers[3] == 9);
  
    std::vector<float> floats ={1.0f, 2.0f, 3.0f,4.0f};
    std::vector<int> numbers2 = {3,2,1,0};
    auto pred = [&](int i) ->auto&{ return floats[i];};
    auto range = owl::utils::map_range(pred, numbers2);
    CHECK(range[0] == 4.0f);
    range[0] = 2.0f;
    CHECK(floats[3] == 2.0f);
  }
}

