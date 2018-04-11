
//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <functional>
#include <algorithm>
#include <iterator>
#include <tuple>

namespace owl
{
  namespace utils
  {
    //returns hash value from std::hash if specialization for T exists
    template <typename T>
    std::size_t hash_value(const T& v)
    {
      static std::hash<T> hasher;
      return hasher(v);
    }
    
    //helper function to combine multiple hash values into a single one (see e.g. implementation of hash_value for ranges)
    template<typename T>
    inline void hash_combine(std::size_t & seed, const T& value)
    {
      seed ^= hash_value(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
    
    //returns hash value for a range of values using HashCombine
    template<typename Iterator>
    std::size_t hash_value(Iterator first, Iterator last, std::size_t seed = 0)
    {
      std::for_each(first, last, [&seed](auto&& x) { hash_combine(seed, x); });
      return seed;
    }
  }
}


namespace std
{
  //provide std::hash specialication for std::tuple types by combining hashes of all tuple elements
  template <typename ... Args>
  struct hash< std::tuple<Args...> >
  {
    std::size_t operator()(const std::tuple<Args...>& tuple) const
    {
      return hash_impl(tuple, std::index_sequence_for<Args...>());
    }
    
  private:
    template <std::size_t... Is>
    static bool hash_impl(std::tuple<Args...>& tuple, std::index_sequence<Is...>)
    {
      std::size_t seed = 0;
      (hash_combine(seed, std::get<Is>(tuple)), ...);
      return seed;
    }
  };
  
  //provide std::hash specialication for std::pair types by combining hashes of first and second
  template <typename T1, typename T2>
  struct hash<std::pair<T1, T2> >
  {
    std::size_t operator()(const std::pair<T1, T2>& Pair) const
    {
      std::size_t seed = 0;
      owl::utils::hash_combine(seed, Pair.first);
      owl::utils::hash_combine(seed, Pair.second);
      return seed;
    }
  };
}
