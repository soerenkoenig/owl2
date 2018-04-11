#include <cmath>

#include "owl/math/angle.hpp"
#include "owl/math/approx.hpp"
#include "catch/catch.hpp"

namespace test
{
  TEST_CASE( "angle", "[math]" )
  {
    using namespace owl::math;

    CHECK(is_acute(degrees<float>(40.0f)));
    CHECK(is_obtuse(degrees<float>(100.0f)));
    CHECK(approx(cos(degrees<float>(90))).margin(0.0001) == 0);
    CHECK(approx(sin(degrees<float>(90))).margin(0.0001) == 1);
  }
}
