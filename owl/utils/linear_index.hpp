//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright (c) 2018 Sören König. All rights reserved.

#pragma once

#include <cassert>
#include <cstddef>

namespace owl
{
  namespace utils
  {

    template <std::size_t N0>
    constexpr std::size_t linear_index(std::size_t n0)
    {
      assert(n0 < N0);
      return n0;
    }

    /**
     *
     * @tparam N0
     * @tparam Ns
     * @tparam Indices
     * @param n0 head index
     * @param ns tail indices
     * @return linear index position
     */
    template <std::size_t N0, std::size_t... Ns, typename... Indices >
    constexpr std::size_t linear_index(std::size_t n0, Indices... ns)
    {
      assert(n0 < N0);
      return n0 + N0 * linear_index<Ns...>(ns...);
    }
  }
}

