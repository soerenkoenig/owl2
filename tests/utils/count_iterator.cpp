#include "owl/utils/count_iterator.hpp"
#include "catch/catch.hpp"

namespace test
{
  TEST_CASE( "count_iterator", "[utils]" )
  {
    using namespace owl::utils;
    
    count_iterator<int> Iter1;
    count_iterator<int> Iter2(42);
    CHECK(*Iter2 == 42);
    count_iterator<int> Iter3(Iter2);
    CHECK(*Iter3 == 42);
    Iter1 = Iter2;
    CHECK(*Iter1 == 42);
    
    CHECK(Iter1 == Iter2);
    CHECK_FALSE(Iter1 != Iter1);
    CHECK(*Iter1 == 42);
    ++Iter1;
    CHECK(*Iter1 == 43);
    auto Iter4 = Iter1++;
    CHECK(*Iter4 == 43);
    CHECK(*Iter1 == 44);
    --Iter1;
    CHECK(*Iter1 == 43);
    auto Iter5 = Iter1--;
    CHECK(*Iter5 == 43);
    CHECK(*Iter1 == 42);
    CHECK(Iter1[1] == 43);
    
    CHECK(Iter5 + 3 == count_iterator<int>(46));
    CHECK(3 + Iter5 == count_iterator<int>(46));
    CHECK(Iter5 - 4 == count_iterator<int>(39));
    CHECK(50 - Iter5 == count_iterator<int>(7));
   
    CHECK(Iter1 < Iter5);
    CHECK(Iter1 <= Iter5);
    CHECK(Iter5 >= Iter1);
    CHECK(Iter5 > Iter1);
    CHECK(Iter1 <= Iter1);
    CHECK(Iter5 >= Iter5);
    std::swap(Iter1,Iter5);
    CHECK(*Iter1 == 43);
    CHECK(*Iter5 == 42);
    
    std::vector<int> numbers = {0, 1, 2, 3, 4, 5, 6, 7};
    auto numbers2 = make_counting_range(0, 8);
    CHECK(std::equal(numbers.begin(),numbers.end(), numbers2.begin(), numbers2.end()));
    
    
  }
}

