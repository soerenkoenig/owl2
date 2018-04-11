//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once
#include <utility>
#include <iterator>
#include <algorithm>
#include <type_traits>
#include "owl/utils/iterator_range.hpp"

namespace owl
{
  namespace utils
  {
    template <typename Iterator>
    class adjacent_iterator
    {
    public:
    
      using ref = typename std::iterator_traits<Iterator>::reference;
    
      struct value_type
      {
        ref prev;
        ref current;
        ref next;
      };
    
      using iterator_category =  std::forward_iterator_tag;
      using difference_type = typename std::iterator_traits<Iterator>::difference_type;
      using pointer = void;
      using reference = void;
      using size_type = std::size_t;
  
      adjacent_iterator(Iterator first, Iterator last)
        : first_(first)
        , last_(last)
        , current_(std::next(first))
        , next_(std::next(std::next(first)))
        , prev_(std::distance(first,last) < 3 ? last :first)
      {
      }

      bool operator==(const adjacent_iterator& other) const
      {
        return prev_ == other.prev_;
      }
    
      bool operator!=(const adjacent_iterator& other) const
      {
        return !(*this == other);
      }

      adjacent_iterator& operator++()
      {
        ++prev_;
        current_ = next_++;
        if(next_ == last_)
          next_ = first_;
      
        return *this;
      }
    
      adjacent_iterator& operator++(int)
      {
        auto temp = *this;
        operator++();
        return temp;
      }

      value_type operator*() const
      {
          return value_type{*prev_, *current_, *next_};
      }

    private:
      Iterator first_;
      Iterator last_;
      Iterator current_;
      Iterator next_;
      Iterator prev_;
    };
  
    template <typename Range>
    auto make_adjacent_range(Range&& range)
    {
      using iterator = decltype(std::begin(range));
      iterator first = std::begin(range);
      iterator last = std::end(range);
      return make_iterator_range(adjacent_iterator<iterator>(first,last), adjacent_iterator<iterator>(last,last));
    }
  }
}
