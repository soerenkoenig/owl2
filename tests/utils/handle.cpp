#include <unordered_map>

#include "owl/utils/handle.hpp"
#include "catch/catch.hpp"

namespace test
{
  struct test_tag{};
  using test_handle = owl::utils::handle<test_tag>;

  TEST_CASE( "handle", "[utils]" )
  {
    using namespace owl::utils;

    test_handle h;
    CHECK_FALSE(h.is_valid());
    std::hash<test_handle> hasher;

  }
}




