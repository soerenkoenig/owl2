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

namespace owl
{
  namespace utils
  {
    template <typename Iterator>
    using is_random_access_iterator = std::conditional_t<
      std::is_same<
        typename std::iterator_traits<Iterator>::iterator_category,
        std::random_access_iterator_tag>::value,
      std::true_type,
      std::false_type>;
    
    /**
     An iterator adapter class which modifies the step length of its internal iterator.
     StepSize the number of steps applied to the internal iterator
     */
    template<typename Iterator, std::size_t StepSize>
    class step_iterator
    {
      static_assert(is_random_access_iterator<Iterator>::value,
        "step_iterator only supports random access iterators");

    public:
      using base_iterator_type = Iterator;
      using iterator_category = typename std::iterator_traits<Iterator>::iterator_category;
      using value_type = typename std::iterator_traits<Iterator>::value_type;
      using difference_type = typename std::iterator_traits<Iterator>::difference_type;
      using pointer = typename std::iterator_traits<Iterator>::pointer;
      using reference = typename std::iterator_traits<Iterator>::reference;
      using size_type = std::size_t;
  
      constexpr static size_type step_size()
      {
        return StepSize;
      }
    
      step_iterator() = default;
    
      explicit step_iterator(base_iterator_type base)
        : base_(base)
      {
      }
  
      template<typename Iterator2, std::size_t StepSize2>
      step_iterator(const step_iterator<Iterator2, StepSize2>& other)
        : base_(other.base_)
      {
      }
  
      template<typename Iterator2, std::size_t StepSize2>
      step_iterator& operator=(const step_iterator<Iterator, StepSize2>& other)
      {
        if(this == &other)
          return *this;
        base_ = other.base_;
        return *this;
      }
    
      template<typename Iterator2>
      step_iterator& operator=(Iterator2 it)
      {
        base_ = it;
        return *this;
      }
  
      base_iterator_type base() const
      {
        return base_;
      }
  
      auto operator*() const
      {
        return *base_;
      }
  
      auto operator->() const
      {
        return std::addressof(operator*());
      }
  
      auto operator[](difference_type n) const
      {
        auto it = *this;
        std::advance(it, n);
        return *it;
      }
  
      step_iterator& operator++()
      {
        std::advance(base_, step_size());
        return *this;
      }
    
      step_iterator& operator--()
      {
        std::advance(base_, -step_size());
        return *this;
      }
  
      step_iterator operator++(int)
      {
        auto tmp = *this;
        operator++();
        return tmp;
      }
  
      step_iterator operator--(int)
      {
        auto tmp = *this;
        operator--();
        return tmp;
      }
    
      step_iterator& operator+=(difference_type n)
      {
        std::advance(base_, step_size() * n);
        return *this;
      }
  
      step_iterator& operator-=(difference_type n)
      {
        std::advance(base_, -step_size() * n);
        return *this;
      }
  
      step_iterator operator+(difference_type n) const
      {
        auto it = *this;
        it += n;
        return it;
      }
  
      step_iterator operator-(difference_type n) const
      {
        auto it = *this;
        it -= n;
        return it;
      }
    
    private:
      base_iterator_type base_;
    };
  
    template<std::size_t StepSize, typename Iterator>
    auto make_step_iterator(Iterator&& base)
    {
      return step_iterator<std::remove_reference_t<Iterator>, StepSize>(std::forward<Iterator>(base));
    }
  
    template<typename Iterator, std::size_t StepSize>
    step_iterator<Iterator, StepSize> operator+(
      typename step_iterator<Iterator, StepSize>::difference_type n,
      const step_iterator<Iterator, StepSize>& it)
    {
      auto ret = it;
      ret += n;
      return ret;
    }
  
    template<typename Iterator, std::size_t StepSize>
    typename step_iterator<Iterator, StepSize>::difference_type operator-(
       const step_iterator<Iterator, StepSize>& lhs,
       const step_iterator<Iterator, StepSize>& rhs)
    {
      return std::distance(lhs.base(),rhs.base()) / lhs.stepsize();
    }
  
    template<typename Iterator1, typename Iterator2, std::size_t StepSize>
    bool operator==(const step_iterator<Iterator1, StepSize>& lhs,
      const step_iterator<Iterator2, StepSize>& rhs)
    {
      return lhs.base() == rhs.base();
    }
  
    template<typename Iterator1, typename Iterator2, std::size_t StepSize>
    bool operator!=(const step_iterator<Iterator1, StepSize>& lhs,
      const step_iterator<Iterator2, StepSize>& rhs)
    {
      return lhs.base() != rhs.base();
    }
 
    template<typename Iterator, std::size_t StepSize>
    bool operator==(const Iterator& lhs, const step_iterator<Iterator, StepSize>& rhs)
    {
      return lhs == rhs.base();
    }
  
    template<typename Iterator, std::size_t StepSize>
    bool operator!=(const Iterator& lhs, const step_iterator<Iterator, StepSize>& rhs)
    {
      return lhs != rhs.base();
    }
  
    template<typename Iterator, std::size_t StepSize>
    bool operator==(const step_iterator<Iterator, StepSize>& lhs,
      const Iterator& rhs)
    {
      return lhs.base() == rhs;
    }
  
    template<typename Iterator, std::size_t StepSize>
    bool operator!=(const step_iterator<Iterator, StepSize>& lhs,
      const Iterator& rhs)
    {
      return lhs.base() != rhs;
    }
  
    template<typename Iterator1, typename Iterator2, std::size_t StepSize>
    bool operator<(const step_iterator<Iterator1, StepSize>& lhs,
      const step_iterator<Iterator2, StepSize>& rhs)
    {
      if(StepSize > 0)
        return lhs.base() < rhs.base();
      else
        return lhs.base() > rhs.base();
    }
  
    template<typename Iterator1, typename Iterator2, std::size_t StepSize>
    bool operator<=(const step_iterator<Iterator1, StepSize>& lhs,
      const step_iterator<Iterator2, StepSize>& rhs)
    {
      if(StepSize > 0)
        return lhs.base() <= rhs.base();
      else
        return lhs.base() >= rhs.base();
    }
  
    template<typename Iterator1, typename Iterator2, std::size_t StepSize>
    bool operator>(const step_iterator<Iterator1, StepSize>& lhs,
      const step_iterator<Iterator2, StepSize>& rhs)
    {
      if(StepSize > 0)
        return lhs.base() > rhs.base();
      else
        return lhs.base() < rhs.base();
    }
  
    template<typename Iterator1, typename Iterator2, std::size_t StepSize>
    bool operator>=(const step_iterator<Iterator1, StepSize>& lhs,
      const step_iterator<Iterator2, StepSize>& rhs)
    {
      if(StepSize > 0)
        return lhs.base() >= rhs.base();
      else
        return lhs.base() <= rhs.base();
    }
  }
}
