//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once
#include <iterator>
#include <type_traits>
#include <algorithm>
#include "owl/utils/iterator_range.hpp"

namespace owl
{
  namespace utils
  {
    template<typename Range, typename OutputIterator>
    inline OutputIterator copy(const Range& rng, OutputIterator out)
    {
      return std::copy(std::begin(rng),std::end(rng), out);
    }
  
    template<typename Range>
    inline Range& sort(Range& rng)
    {
      std::sort(std::begin(rng), std::end(rng));
      return rng;
    }
  
    template<typename Range, typename Pred>
    inline Range& sort(Range& rng, Pred&& less)
    {
      std::sort(std::begin(rng), std::end(rng), std::forward<Pred>(less));
      return rng;
    }
  
    template<typename Range, typename T>
    inline auto find(Range&& rng, const T& value)
    {
      return std::find(std::begin(rng), std::end(rng), value);
    }
  
    template<typename Range, typename Pred>
    inline auto find_if(Range&& rng, Pred&& cond)
    {
      return std::find_if(std::begin(rng), std::end(rng), std::forward<Pred>(cond));
    }
  
    template<typename InputRange, typename OutIter, typename BinaryOperation>
    inline auto transform(const InputRange& rng, OutIter&& it, BinaryOperation&& binary_op)
    {
      return std::transform(std::begin(rng),
        std::end(rng),std::forward<OutIter>(it),
        std::forward<BinaryOperation>(binary_op));
    }
  
    template< typename Range, class T >
    constexpr void fill( Range&& rng, const T& value)
    {
      std::fill(std::begin(rng), std::end(rng), value);
    }
  
    template<typename Range, typename Pred>
    inline auto all_of(const Range& rng, Pred&& cond)
    {
      return std::all_of(std::begin(rng), std::end(rng),std::forward<Pred>(cond));
    }
  
    template<typename Range, typename Pred>
    inline auto none_of(const Range& rng, Pred&& cond)
    {
      return std::none_of(std::begin(rng), std::end(rng),std::forward<Pred>(cond));
    }
  
     template<typename Range, typename Pred>
    inline auto any_of(const Range& rng, Pred&& cond)
    {
      return std::any_of(std::begin(rng), std::end(rng),std::forward<Pred>(cond));
    }
  
    template<typename Range, typename Pred>
    inline auto count_if(const Range& rng, Pred&& cond)
    {
      return std::count_if(std::begin(rng), std::end(rng),std::forward<Pred>(cond));
    }
  }
}
