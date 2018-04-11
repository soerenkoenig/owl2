//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <limits>
#include <iterator>
#include <tuple>
#include <iostream>

#include "owl/export.hpp"

namespace owl
{
  namespace utils
  {
    template <typename Tag, typename Index = std::size_t>
    class handle
    {
    public:
      using index_type = Index;
      using difference_type = decltype(std::declval<Index>() - std::declval<Index>());
      
      constexpr explicit handle(const index_type& index = invalid_index())
        : index_{index}
      {
      }
      
      handle(const handle&) = default;
      handle(handle&&) = default;
      
      handle& operator=(const handle&) = default;
      handle& operator=(handle&&) = default;
      
      inline const std::size_t& index() const
      {
        return index_;
      }
      
      inline bool is_valid() const
      {
        return index_ != invalid_index();
      }
      
      inline void invalidate()
      {
        index_ = invalid_index();
      }
    
      inline handle& operator+=(const difference_type& n)
      {
        index_ += n;
        return *this;
      }
    
      inline handle& operator-=(const difference_type& n)
      {
        index_ += n;
        return *this;
      }
    
      inline handle& operator++()
      {
        ++index_;
        return *this;
      }
      
      inline handle& operator--()
      {
        --index_;
        return *this;
      }
      
      inline handle operator++(int)
      {
        auto tmp = *this;
        operator++();
        return tmp;
      }
      
      inline handle operator--(int)
      {
        auto tmp = *this;
        operator--();
        return tmp;
      }
    
      inline auto operator-(const handle& other) const
      {
        return index_ - other.index_;
      }
    
      inline auto operator+(const difference_type& other) const
      {
        return handle{index_ + other};
      }
    
    
      inline bool operator==(const handle& other) const
      {
        return index_ == other.index_;
      }
      
      inline bool operator!=(const handle& other) const
      {
        return index_ != other.index_;
      }
      
      inline bool operator<(const handle& other) const
      {
        return index_ < other.index_;
      }
      
      inline bool operator>(const handle& other) const
      {
        return  index_ > other.index_;
      }
      
      inline bool operator<=(const handle& other) const
      {
        return index_ <= other.index_;
      }
      
      inline bool operator>=(const handle& other) const
      {
        return  index_ >= other.index_;
      }
    
      constexpr static handle invalid()
      {
        return handle(invalid_index());
      }
      
    private:
      constexpr static index_type invalid_index()
      {
        return std::numeric_limits<index_type>::max();
      }
      
      index_type index_;
    };
  
    template <typename Tag, typename Index>
    std::ostream& operator<<(std::ostream& out,const handle<Tag,Index>& h)
    {
      if(h.is_valid())
        return out << h.index();
      return out << "invalid";
    }
  
  
    struct OWL_API default_deref
    {
      template <typename T>
      T operator()(T&& val) const
      {
        return std::forward<T>(val);
      }
    };
  
    struct OWL_API default_next
    {
      template <typename T>
      T operator()(T&& val) const
      {
        return ++std::forward<T>(val);
      }
    };
  
    template <typename Handle, typename Next, typename Deref = default_deref>
    class handle_iterator
    {
    public:
      using iterator_category = typename std::forward_iterator_tag;
      using value_type = decltype(std::declval<Deref>()(std::declval<Handle>()));
      using difference_type = decltype(std::declval<std::size_t>() - std::declval<std::size_t>());
      using pointer = void;
      using reference = void;
      using size_type = std::size_t;
    
    
      template<typename N, typename D>
      handle_iterator(const Handle& current, N&& n, D&& d)
        : current_{current}
        , next(std::forward<N>(n))
        , deref(std::forward<D>(d))
      {
      }
    
      handle_iterator(const handle_iterator& other) = default;
    
      handle_iterator& operator=(const handle_iterator& other) = default;
    
      auto operator*() const
      {
        return deref(current_);
      }
    
      handle_iterator& operator++()
      {
        current_ = next(current_);
        return *this;
      }
    
      handle_iterator operator++(int)
      {
        auto tmp = *this;
        operator++();
        return tmp;
      }
  
      bool operator==(const handle_iterator &other) const
      {
        return current_ == other.current_;
      }
    
      bool operator!=(const handle_iterator &other) const
      {
        return current_ != other.current_;
      }
    
    private:
      Handle current_;
      Next next;
      Deref deref;
    };
  
    template <typename Handle, typename Next, typename Deref>
    handle_iterator<Handle, Next, Deref> make_handle_iterator(Handle current,
       Next next = default_next{}, Deref deref = default_deref() )
    {
      return handle_iterator<Handle, Next, Deref>(current, next, deref);
    }
  
    template <typename Handle, typename Next = default_next, typename Deref = default_deref>
    auto make_handle_iterator_range(Handle first, Handle last, Next next = default_next{}, Deref deref = default_deref())
    {
      return make_iterator_range(make_handle_iterator(first, next, deref),
        make_handle_iterator(last, next, deref));
    }
  
    template <typename Handle, typename Next, typename Deref = default_deref>
    class handle_circulator
    {
    public:
      using iterator_category = typename std::forward_iterator_tag;
      using value_type = decltype(std::declval<Deref>()(std::declval<Handle>()));
      using difference_type = decltype(std::declval<std::size_t>() - std::declval<std::size_t>());
      using pointer = void;
      using reference = value_type;
      using size_type = std::size_t;
    
    
      template<typename N, typename D>
      handle_circulator(const Handle& current, N&& n, D&& d, std::size_t lab_count = 0)
        : current_{current}
        , initial_{current}
        , next(std::forward<N>(n))
        , deref(std::forward<D>(d))
        , lab_count_{lab_count}
      {
      }
      
      handle_circulator(const handle_circulator& other) = default;
    
      handle_circulator& operator=(const handle_circulator& other) = default;
    
      auto operator*() const
      {
        return deref(current_);
      }
    
      handle_circulator& operator++()
      {
        current_ = next(current_);
        if(current_ == initial_)
          ++lab_count_;
        return *this;
      }
    
      handle_circulator operator++(int)
      {
        auto tmp = *this;
        operator++();
        return tmp;
      }
  
      bool operator==(const handle_circulator &other) const
      {
        return std::tie(current_, lab_count_) == std::tie(other.current_, other.lab_count_);
      }
    
      bool operator!=(const handle_circulator &other) const
      {
        return std::tie(current_, lab_count_) != std::tie(other.current_, other.lab_count_);
      }
  
    private:
      Handle current_;
      Handle initial_;
      Next next;
      Deref deref;
      std::size_t lab_count_;
    };
  
    template <typename Handle, typename Next, typename Deref>
    handle_circulator<Handle, Next, Deref> make_handle_circulator(Handle current,
      std::size_t lab_count = 0, Next next = default_next{}, Deref deref = default_deref() )
    {
      return handle_circulator<Handle, Next, Deref>(current, next, deref, lab_count);
    }
  
    template <typename Handle, typename Next = default_next, typename Deref = default_deref>
    auto make_handle_circulator_range(Handle first, std::size_t lab_count_first,
      Handle last, std::size_t lab_count_last = 1, Next next = default_next{}, Deref deref = default_deref())
    {
      return make_iterator_range(make_handle_circulator(first, lab_count_first, next, deref),
        make_handle_circulator(last, lab_count_last, next, deref));
    }
  
     template <typename Handle, typename Next = default_next, typename Deref = default_deref>
    auto make_handle_circulator_range(Handle first, Next next = default_next{}, Deref deref = default_deref())
    {
      return make_iterator_range(make_handle_circulator(first, first.is_valid() ? 0 : 1, next, deref),
        make_handle_circulator(first, 1, next, deref));
    }
  }
}


namespace std
{
  template<typename Tag, typename Index>
  struct hash<owl::utils::handle<Tag, Index>>
  {
    std::size_t operator()(const owl::utils::handle<Tag, Index>& value) const
    {
      static hash<Index> hasher;
      return hasher(value.index());
    }
  };
}
