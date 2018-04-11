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
    template <typename Predicate, typename Iterator>
    class filter_iterator
    {
    public:
      using base_iterator_type = Iterator;
      using predicate_type = Predicate;
      using iterator_category = typename std::forward_iterator_tag;
      using value_type = typename std::iterator_traits<Iterator>::value_type;
      using difference_type = typename std::iterator_traits<Iterator>::difference_type;
      using pointer = typename std::iterator_traits<Iterator>::pointer;
      using const_pointer = const typename std::iterator_traits<Iterator>::pointer;
      using reference = typename std::iterator_traits<Iterator>::reference;
      using const_reference = const typename std::iterator_traits<Iterator>::reference;
      using size_type = std::size_t;
    
      filter_iterator() = default;
    
      explicit filter_iterator(predicate_type predicate,
        const base_iterator_type &iter, const base_iterator_type& end)
        : predicate_(predicate)
        , current_(iter)
        , end_(end)
      {
        ensure_predicate();
      }
    
      filter_iterator(base_iterator_type base, base_iterator_type end)
        : current_(base)
        , end_(end)
      {
        ensure_predicate();
      }
  
      template<typename Predicate2, typename Iterator2>
      filter_iterator(const filter_iterator<Predicate2, Iterator2>& other)
        : predicate_(other.predicate_)
        , current_(other.current_)
        , end_(other.end_)
      {
      }
  
      template<typename Predicate2, typename Iterator2>
      filter_iterator& operator=(const filter_iterator<Predicate2, Iterator2>& other)
      {
        if(this == &other)
          return *this;
        current_ = other.current_;
        end_ = other.end_;
        predicate_ = other.predicate_;
        return *this;
      }
    
      inline const predicate_type& predicate() const
      {
        return predicate_;
      }
  
      inline const base_iterator_type& base() const
      {
        return current_;
      }
    
      filter_iterator end() const
      {
        return filter_iterator(predicate_, end_, end_);
      }
    
      reference operator*()
      {
        return *current_;
      }
  
      const_reference operator*() const
      {
        return *current_;
      }
    
      pointer operator->()
      {
        std::addressof(operator*());
      }
  
      const_pointer operator->() const
      {
        std::addressof(operator*());
      }
    
      filter_iterator& operator++()
      {
        if (current_ == end_)
          return *this;
        ++current_;
        ensure_predicate();
        return *this;
      }

      filter_iterator operator++(int)
      {
        filter_iterator temp(*this);
        this->operator++();
        return temp;
      }

      bool operator==(const filter_iterator& rhs) const
      {
        return current_ == rhs._current;
      }

      bool operator!=(const filter_iterator& rhs) const
      {
        return current_ != rhs.current_;
      }
  
    private:
      void ensure_predicate()
      {
        while (current_ != end_ && !predicate_(*current_))
          ++current_;
      }
    
      predicate_type predicate_;
      base_iterator_type current_;
      base_iterator_type end_;
    };
  
  
    template <class Predicate, class Iterator>
    filter_iterator<Predicate, Iterator>
    make_filter_iterator(Predicate&& f, Iterator&& x, Iterator&& end)
    {
      return filter_iterator<Predicate, Iterator>(std::forward<Predicate>(f),
        std::forward<Iterator>(x), std::forward<Iterator>(end));
    }

    template<typename Predicate, typename Iterator>
    auto filter(Iterator&& first, Iterator&& one_past_last)
    {
      auto f = make_filter_iterator<Predicate>(std::forward<Iterator>(first),
        std::forward<Iterator>(one_past_last));
      return make_iterator_range(f, f.end());
    }
  
    template<typename Predicate, typename Range, typename = std::enable_if_t<is_container<std::decay_t<Range>>::value>>
    auto filter(Range&& range)
    {
      auto f = make_filter_iterator<Predicate>(std::begin(range), std::end(range));
      return make_iterator_range(f, f.end());
    }

    template<typename Predicate, typename Range, typename = std::enable_if_t< is_container<std::decay_t<Range>>::value>>
    auto filter(Predicate &&pred, Range&& range)
    {
      auto f = make_filter_iterator(std::forward<Predicate>(pred),
        std::begin(range), std::end(range));
      return make_iterator_range(f, f.end());
    }
  }
}

