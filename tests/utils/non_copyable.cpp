#include <type_traits>

#include "owl/utils/non_copyable.hpp"
#include "catch/catch.hpp"

namespace test
{
  class test : owl::utils::non_copyable
  {
  };

  TEST_CASE( "non_copyable", "[utils]" )
  {
    CHECK_FALSE(std::is_copy_assignable<test>::value);
    CHECK_FALSE(std::is_copy_constructible<test>::value);
  }
}


