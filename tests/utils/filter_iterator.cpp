#include "owl/utils/filter_iterator.hpp"
#include "owl/utils/range_algorithm.hpp"
#include "owl/utils/filter_iterator.hpp"
#include "owl/utils/handle.hpp"
#include "catch/catch.hpp"

namespace test
{
  TEST_CASE( "filter_iterator", "[utils]" )
  {
    using namespace owl::utils;
    std::vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7};
  
    auto is_even = [](int x){ return x % 2 == 0;};
    auto even_numbers = filter(is_even, numbers);
    CHECK(even_numbers.size() == 4);
    CHECK(all_of(even_numbers, is_even));
  
  }
}
