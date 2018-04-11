//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright (c) 2018 Sören König. All rights reserved.
//

#pragma once

#include <iterator>

#include "owl/export.hpp"

namespace owl
{
  namespace utils
  {
    template<typename ForwardIterator, typename T>
    void lin_space(ForwardIterator first, ForwardIterator last, T first_val, T last_val)
    {
      auto N = std::distance(first, last);
      if(N == 1)
      {
        *first = last_val;
        return ;
      }
      auto diff = last_val - first_val;

      for(decltype(N) i = 0; i < N; i++)
      {
        *first++ = first_val + i * diff / (static_cast<T>(N) - static_cast<T>(1));
      }
    }
  }
}



