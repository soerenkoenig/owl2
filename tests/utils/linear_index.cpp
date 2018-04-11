
#include "owl/utils/linear_index.hpp"
#include "catch/catch.hpp"

namespace test
{
  TEST_CASE( "linear_index", "[utils]")
  {
    using namespace owl::utils;

    CHECK(linear_index<3,4,5,6>(0,0,0,0) == 0);
    CHECK(linear_index<3>(0) == 0);

    CHECK(linear_index<3,4>(0,0) == 0);
    //3 rows x 4 cols   at row = 1, col = 0)
    CHECK(linear_index<3,4>(1,0) == 1);
    CHECK(linear_index<3,4>(2,0) == 2);
    CHECK(linear_index<3,4>(0,1) == 3);
    CHECK(linear_index<3,4>(0,2) == 6);
  }
}


