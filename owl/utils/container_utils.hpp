//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <valarray>
#include <array>
#include <set>
#include <unordered_set>
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <forward_list>
#include <deque>
#include <string>
#include <iterator>

#include "owl/utils/template_utils.hpp"

namespace owl
{
  namespace utils
  {
    DEFINE_HAS_MEMBER(has_const_iterator, T::const_iterator, typename T::const_iterator);
    DEFINE_HAS_MEMBER(has_iterator, T::iterator, typename T::iterator);
    DEFINE_HAS_SIGNATURE(has_begin, T::begin, typename T::const_iterator (T::*)() const);
    DEFINE_HAS_SIGNATURE(has_end, T::end, typename T::const_iterator (T::*)() const);
  
    DEFINE_HAS_SIGNATURE(has_resize, T::resize, void (T::*)(typename T::size_type));
  
    template <typename T>
    struct is_basic_string : std::false_type {};

    template <typename... Ts>
    struct is_basic_string<std::basic_string<Ts...>> : std::true_type {};

    template <typename T>
    struct is_array : std::false_type {};

    template <typename... Ts>
    struct is_array<std::array<Ts...>> : std::true_type {};

    template <typename T>
    struct is_vector : std::false_type {};

    template <typename... Ts>
    struct is_vector<std::vector<Ts...>> : std::true_type {};

    template <typename T>
    struct is_list : std::false_type {};

    template <typename... Ts>
    struct is_list<std::list<Ts...>> : std::true_type {};

    template <typename T>
    struct is_forward_list : std::false_type {};

    template <typename... Ts>
    struct is_forward_list<std::forward_list<Ts...>> : std::true_type {};

    template <typename T>
    struct is_deque : public std::false_type {};

    template <typename... Ts>
    struct is_deque<std::deque<Ts...>> : std::true_type {};

    template <typename T>
    struct is_map : public std::false_type {};

    template <typename... Ts>
    struct is_map<std::map<Ts...>> : std::true_type {};

    template <typename T>
    struct is_multimap : std::false_type {};

    template <typename... Ts>
    struct is_multimap<std::multimap<Ts...>> : std::true_type {};

    template <typename T>
    struct is_unordered_map : std::false_type {};

    template <typename... Ts>
    struct is_unordered_map<std::unordered_map<Ts...>> : std::true_type {};

    template <typename T>
    struct is_unordered_multimap : std::false_type {};

    template <typename... Ts>
    struct is_unordered_multimap<std::unordered_multimap<Ts...>> : public std::true_type {};

    template <typename T>
    struct is_set : std::false_type {};

    template <typename... Ts>
    struct is_set<std::set<Ts...>> : std::true_type {};

    template <typename T>
    struct is_multiset : std::false_type {};

    template <typename... Ts>
    struct is_multiset<std::multiset<Ts...>> : std::true_type {};

    template <typename T>
    struct is_unordered_set : std::false_type {};

    template <typename... Ts>
    struct is_unordered_set<std::unordered_set<Ts...>> : std::true_type {};

    template <typename T>
    struct is_unordered_multiset : std::false_type {};

    template <typename... Ts>
    struct is_unordered_multiset<std::unordered_multiset<Ts...>> : std::true_type {};

    template <typename T>
    using is_continuous_container =
    std::conditional_t< is_array<T>::value || is_vector<T>::value || is_basic_string<T>::value, std::true_type, std::false_type>;

    template <typename T>
    using is_sequence_container = std::conditional_t<is_deque<T>::value || is_forward_list<T>::value || is_list<T>::value || is_continuous_container<T>::value, std::true_type, std::false_type>;

    template <typename T>
    using is_unordered_associative_container = std::conditional_t< is_unordered_map<T>::value || is_unordered_multimap<T>::value || is_unordered_set<T>::value || is_unordered_multiset<T>::value,
      std::true_type, std::false_type>;

    template <typename T>
    using is_ordered_associative_container = std::conditional_t<is_map<T>::value || is_multimap<T>::value ||
      is_set<T>::value || is_multiset<T>::value, std::true_type, std::false_type>;

    template <typename T>
    using is_associative_container = std::conditional_t<is_ordered_associative_container<T>::value ||
      is_unordered_associative_container<T>::value, std::true_type, std::false_type>;



    /*template <typename T>
    struct is_container : std::integral_constant<bool, has_begin<T>::value && has_end<T>::value> {};*/
  
    template <typename T>
    struct is_container : is_iterable<T>{};
  
    template <typename T, std::size_t N>
    struct is_container<T[N]> : std::true_type {};

    template <typename T>
    struct is_container<std::valarray<T>> : std::true_type {};
  
    template <typename Container, typename = std::enable_if_t<is_container<Container>::value> >
    struct container_traits
    {
      typedef decltype(std::begin(std::declval<Container>())) iterator;
      typedef decltype(std::begin(std::declval<const Container>())) const_iterator;
      typedef decltype(std::size(std::declval<const Container>())) size_type;
      typedef typename std::iterator_traits<iterator>::difference_type difference_type;
      typedef typename std::iterator_traits<iterator>::value_type value_type;
    };
  
    template <typename T, std::size_t N >
    struct container_traits<T[N], void>
    {
      typedef T* iterator;
      typedef const T* const_iterator;
      typedef std::size_t size_type;
      typedef std::ptrdiff_t difference_type;
      typedef T value_type;
    };
    
    namespace detail
    {
      template<class Function, std::size_t... Indices>
      constexpr std::array<typename std::result_of<Function(std::size_t)>::type, sizeof...(Indices)>
      make_array_helper(Function f, std::index_sequence<Indices...>)
      {
        return {{f(Indices)...}};
      }
    }
      
    template <std::size_t N, class Function>
    constexpr std::array<typename std::result_of<Function(std::size_t)>::type, N>
    make_array(Function f)
    {
      return detail::make_array_helper(f, std::make_index_sequence<N>{});
    }
  }
}
