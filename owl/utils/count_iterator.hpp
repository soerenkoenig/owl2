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
#include <algorithm>
#include <type_traits>
#include "owl/utils/iterator_range.hpp"

namespace owl
{
  namespace utils
  {
    template <typename Integer>
    class count_iterator
    {
    public:
      using iterator_category = typename std::random_access_iterator_tag;
      using value_type = Integer;
      using difference_type = decltype(std::declval<Integer>() - std::declval<Integer>());
      using pointer = void;
      using reference = value_type;
      using const_reference = const reference;
      using size_type = std::size_t;
      
      count_iterator() = default;
      
      count_iterator(Integer integer)
        : current_{integer}
      {
      }
      
      auto operator*() const
      {
        return current_;
      }
      
      auto operator[](difference_type n) const
      {
        return current_ + n;
      }
      
      count_iterator& operator++()
      {
        ++current_;
        return *this;
      }
      
      count_iterator& operator--()
      {
        --current_;
        return *this;
      }
      
      count_iterator operator++(int)
      {
        auto tmp = *this;
        operator++();
        return tmp;
      }
      
      count_iterator operator--(int)
      {
        auto tmp = *this;
        operator--();
        return tmp;
      }
      
      count_iterator& operator+=(difference_type n)
      {
        current_ += n;
        return *this;
      }
      
      count_iterator& operator-=(difference_type n)
      {
        current_ -= n;
        return *this;
      }
      
      count_iterator operator+(difference_type n) const
      {
        auto it = *this;
        it += n;
        return it;
      }
      
      difference_type operator-(const count_iterator &other) const
      {
        return current_ - other.current_;
      }
      
      count_iterator operator-(difference_type n) const
      {
        auto it = *this;
        it -= n;
        return it;
      }
      
      bool operator==(const count_iterator &other) const
      {
        return current_ == other.current_;
      }
      
      bool operator!=(const count_iterator &other) const
      {
        return !operator==(other);
      }
      
      bool operator<(const count_iterator &other) const
      {
        return current_ < other.current_;
      }
      
      bool operator>(const count_iterator &other) const
      {
        return current_ > other.current_;
      }
      
      bool operator<=(const count_iterator &other) const
      {
        return current_ <= other.current_;
      }
      
      bool operator>=(const count_iterator &other) const
      {
        return current_ >= other.current_;
      }
    private:
      Integer current_;
    };
    
    template <typename Integer>
    count_iterator<Integer>
    operator+(typename count_iterator<Integer>::difference_type n,
              const count_iterator<Integer> &rhs)
    {
      return rhs + n;
    }
    
    template <typename Integer>
    count_iterator<Integer>
    operator-(typename count_iterator<Integer>::difference_type n,
              const count_iterator<Integer> &rhs)
    {
      return count_iterator<Integer>(n - *rhs);
    }
    
    template <typename Integer>
    count_iterator<Integer> make_count_iterator(Integer integer)
    {
      return count_iterator<Integer>(integer);
    }
    
    template <typename Integer>
    auto make_counting_range(Integer first, Integer last)
    {
      return make_iterator_range(make_count_iterator(first), make_count_iterator(last));
    }
  }
}
