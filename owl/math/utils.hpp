//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <type_traits>

namespace owl
{
  namespace math
  {
    template <typename Scalar>
    constexpr Scalar sqr(Scalar s)
    {
      return s * s;
    }
  
    inline std::size_t next_pow_of_2(std::size_t x)
    {
      --x;
      x |= x >> 1;
      x |= x >> 2;
      x |= x >> 4;
      x |= x >> 8;
      x |= x >> 16;
      ++x;
      return x;
    }

    inline bool is_pow_of_2(std::size_t x)
    {
      return x > 0 && !(x & (x - 1));
    }
  
  }
}


