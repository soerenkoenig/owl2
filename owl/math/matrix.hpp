//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <array>
#include <cmath>
#include <numeric>
#include <iostream>
#include <algorithm>
#include <random>
#include <type_traits>

#include "owl/utils/linear_index.hpp"
#include "owl/utils/iterator_range.hpp"
#include "owl/utils/step_iterator.hpp"
#include "owl/utils/container_utils.hpp"
#include "owl/utils/random_utils.hpp"
#include "owl/math/approx.hpp"

namespace owl
{
  namespace math
  {
    namespace detail
    {
      template <typename T, std::size_t N>
      constexpr T delta(std::size_t i)
      {
          return i == N ? (T)1 : 0;
      }
  
      template <typename T>
      constexpr T zero(std::size_t)
      {
          return 0;
      }
  
      template <typename T>
      constexpr T one(std::size_t)
      {
          return (T)1;
      }
      
      template <typename T>
      constexpr T max(std::size_t)
      {
        return std::numeric_limits<T>::max();
      }
      
      template <typename T>
      constexpr T lowest(std::size_t)
      {
        return std::numeric_limits<T>::lowest();
      }

      template <typename T, std::size_t N>
      constexpr T eye(std::size_t i)
      {
          return (i % (N + 1)) == 0 ? (T)1 : 0;
      }
    }
  
    template <typename Scalar, std::size_t Rows, std::size_t Cols>
    class matrix
    {
    public:
    static_assert(Rows * Cols > 0, "invalid matrix dimension");
    static_assert(std::is_arithmetic_v<Scalar> && !std::is_same_v<Scalar, bool>, "invalid scalar type");
    
    using container_type = std::array<Scalar, Rows * Cols>;
    using size_type = typename container_type::size_type;
    using difference_type = typename container_type::difference_type;
    using iterator = typename container_type::iterator;
    using const_iterator = typename container_type::const_iterator;
    using reverse_iterator = typename container_type::reverse_iterator;
    using const_reverse_iterator = typename container_type::const_reverse_iterator;
    using value_type = typename container_type::value_type;
    using reference = typename container_type::reference;
    using const_reference = typename container_type::const_reference;
    using pointer = typename container_type::pointer;
    using const_pointer = typename container_type::const_pointer;
    
    using row_iterator = utils::step_iterator<iterator,Rows>;
    using const_row_iterator =  utils::step_iterator<const_iterator,Rows>;
    
    using column_iterator = iterator;
    using const_column_iterator = const_iterator;
    
    using row_range = owl::utils::iterator_range<row_iterator>;
    using const_row_range = owl::utils::iterator_range<const_row_iterator>;
    
    using column_range = owl::utils::iterator_range<column_iterator>;
    using const_column_range = owl::utils::iterator_range<const_column_iterator>;
    
    template <typename S1, typename S2 = value_type>
    using enable_if_scalar_t = std::enable_if_t<std::is_convertible<S1, S2>::value>;
    
    template <typename S1, std::size_t R = Rows, std::size_t C = Cols, typename S2 = value_type>
    using result_matrix_t = matrix<decltype(std::declval<S1>()*std::declval<S2>()),R, C>;
    
    constexpr static size_type size()
    {
      return Rows * Cols;
    }
    
    constexpr static size_type ncols()
    {
      return Cols;
    }
    
    constexpr static size_type nrows()
    {
      return Rows;
    }
    
    constexpr static bool is_square()
    {
      return Rows == Cols;
    }
   
    constexpr static bool is_square(std::size_t n)
    {
      return Rows == n && Cols == n;
    }
    
    constexpr static bool is_vector()
    {
       return Rows == 1 || Cols == 1;
    }
    
    constexpr static bool is_vector(std::size_t n)
    {
      return (Rows == 1 && Cols == n) || (Rows == n && Cols == 1);
    }
    
    constexpr static size_type linear_index(size_type row_index, size_type column_index)
    {
      return utils::linear_index<Rows,Cols>(row_index, column_index);
    }
    
    template <size_t I, typename = std::enable_if_t<(I < size()) && is_vector()> >
    static constexpr matrix ident()
    {
      return matrix{owl::utils::make_array<size()>(detail::delta<Scalar, I>)};
    }
    
    template <bool Dummy = true, typename = std::enable_if_t<Dummy && is_vector() && (size() > 0)>  >
    static constexpr matrix identity_x()
    {
      return ident<0>();
    }
    
    template <bool Dummy = true, typename = std::enable_if_t<Dummy && is_vector() && (size() > 1)>  >
    static constexpr matrix identity_y()
    {
      return ident<1>();
    }
    
    template <bool Dummy = true, typename = std::enable_if_t<Dummy && is_vector() && (size() > 2)>  >
    static constexpr matrix identity_z()
    {
      return ident<2>();
    }
    
    template <bool Dummy = true, typename = std::enable_if_t<Dummy && is_vector() && (size() > 3)>  >
    static constexpr matrix identity_w()
    {
      return ident<3>();
    }
    
    static constexpr matrix zero()
    {
      return matrix{owl::utils::make_array<size()>(detail::zero<Scalar>)};
    }
    
    static constexpr matrix one()
    {
      return matrix{owl::utils::make_array<size()>(detail::one<Scalar>)};
    }
      
    static constexpr matrix max()
    {
       return matrix{owl::utils::make_array<size()>(detail::max<Scalar>)};
    }
    
    static constexpr matrix lowest()
    {
      return matrix{owl::utils::make_array<size()>(detail::lowest<Scalar>)};
    }
    
    static constexpr matrix identity()
    {
      return matrix{owl::utils::make_array<size()>(detail::eye<Scalar,Rows>)};
    }
    
    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }
    
    const_iterator cbegin() const { return data_.cbegin(); }
    const_iterator cend() const { return data_.cend(); }
    
    reverse_iterator rbegin() { return data_.rbegin(); }
    reverse_iterator rend() { return data_.rend(); }
    
    const_reverse_iterator rbegin() const { return data_.crbegin(); }
    const_reverse_iterator rend() const { return data_.crend(); }
    
    const_reverse_iterator crbegin() const { return data_.crbegin(); }
    const_reverse_iterator crend() const { return data_.crend(); }
    
    iterator iterator_at(size_type row_index, size_type column_index)
    {
      return begin() + linear_index(row_index, column_index);
    }
    
    const_iterator iterator_at(size_type row_index, size_type column_index) const
    {
      return cbegin() + linear_index(row_index, column_index);
    }
    
    row_iterator row_begin(size_type row_index)
    {
      return row_iterator{iterator_at(row_index, 0)};
    }
    
    row_iterator row_end(size_type row_index)
    {
      return row_iterator{iterator_at(row_index, ncols())};
    }
    
    const_row_iterator row_begin(size_type r) const
    {
      return const_row_iterator{iterator_at(r, 0)};
    }
    
    const_row_iterator row_end(size_type r) const
    {
      return const_row_iterator{iterator_at(r, ncols())};
    }
    
    const_row_iterator row_cbegin(size_type r) const
    {
      return const_row_iterator{iterator_at(r, 0)};
    }
    
    const_row_iterator row_cend(size_type r) const
    {
     return const_row_iterator{iterator_at(r, ncols())};
    }
    
    column_iterator column_begin(size_type c)
    {
      return column_iterator{iterator_at(0, c)};
    }
    
    row_iterator column_end(size_type c)
    {
      return column_iterator{iterator_at(nrows(), c)};
    }
    
    const_column_iterator column_begin(size_type c) const
    {
       return const_column_iterator{iterator_at(0, c)};
    }
    
    const_column_iterator column_end(size_type c) const
    {
       return const_column_iterator{iterator_at(nrows(), c)};
    }
    
    const_column_iterator column_cbegin(size_type c) const
    {
      return const_column_iterator{iterator_at(0, c)};
    }
    
    const_column_iterator column_cend(size_type c) const
    {
       return const_column_iterator{iterator_at(nrows(), c)};
    }
    
    row_range row(size_type r)
    {
      return owl::utils::make_iterator_range(row_begin(r), row_end(r));
    }
    
    const_row_range row(size_type r) const
    {
      return owl::utils::make_iterator_range(row_begin(r), row_end(r));
    }
    
    column_range column(size_type c)
    {
      return owl::utils::make_iterator_range(column_begin(c), column_end(c));
    }
    
    const_column_range column(size_type c) const
    {
      return owl::utils::make_iterator_range(column_begin(c), column_end(c));
    }
    
    constexpr matrix() = default;
    
    template<typename M = matrix, typename = std::enable_if_t<M::is_vector()>>
    matrix(std::initializer_list<Scalar> list)
    {
      std::copy(list.begin(), list.end(), data_.begin());
    }

    constexpr matrix(std::array<Scalar, Rows * Cols> arr)
      : data_{arr}
    {
    }
    
    template<typename M = matrix, typename S, typename... Args, typename = std::enable_if_t<M::is_vector()>>
    explicit matrix(S&& a, Args&&... args)
      : data_{{static_cast<value_type>(std::forward<S>(a)), static_cast<value_type>(std::forward<Args>(args))...}}
    {
      static_assert(sizeof...(Args) == 0 || sizeof...(Args) + 1 == size(), "incorrect number of arguments");
    }
    
    matrix(matrix& other) = default;
    
    matrix(const matrix& other) = default;
    
    matrix(matrix&& other) = default;
    
    template <typename S2>
    matrix(const matrix<S2, Rows, Cols>& other)
    {
      for(size_type i = 0; i < Rows;++i)
        for(size_type j = 0; j < Cols; ++j)
          operator()(i, j) = other(i, j);
    }
    
    matrix& operator=(const matrix& other) = default;
    
    matrix& operator=(matrix&& other) = default;
    
    template <typename S2>
    matrix& operator=(const matrix<S2, Rows, Cols>& other)
    {
      for(size_type i = 0; i < Rows;++i)
        for(size_type j = 0; j < Cols; ++j)
          operator()(i, j) = other(i, j);
      return *this;
    }
    
    //component accessors
    template<typename M = matrix,
      typename = std::enable_if_t<M::is_vector() && (size() > 0)>>
    value_type& x()
    {
      return data_[0];
    }
    
    template<typename M = matrix,
      typename = std::enable_if_t<M::is_vector() && (size() > 0)>>
    const value_type& x() const
    {
      return data_[0];
    }
    
    template<typename M = matrix,
      typename = std::enable_if_t<M::is_vector() && (size() > 1)>>
    value_type& y()
    {
      return data_[1];
    }
    
    template<typename M = matrix,
      typename = std::enable_if_t<M::is_vector() && (size() > 1)>>
    const value_type& y() const
    {
      return data_[1];
    }
    
    template<typename M = matrix,
    typename = std::enable_if_t<M::is_vector() && (size() > 2)>>
    value_type& z()
    {
      return data_[2];
    }
    
    template<typename M = matrix,
    typename = std::enable_if_t<M::is_vector() && (size() > 2)>>
    const value_type& z() const
    {
      return data_[2];
    }
    
    template<typename M = matrix,
    typename = std::enable_if_t<M::is_vector() && (size() > 3)>>
    value_type& w()
    {
      return data_[3];
    }
    
    template<typename M = matrix,
    typename = std::enable_if_t<M::is_vector() && (size() > 3)>>
    const value_type& w() const
    {
      return data_[3];
    }
    
    value_type& front()
    {
      return data_.front();
    }
    
    const value_type& front() const
    {
      return data_.front();
    }
    
    value_type& back()
    {
      return data_.back();
    }
    
    const value_type& back() const
    {
      return data_.back();
    }
    
    reference operator[](size_type pos)
    {
      return data_[pos];
    }
    
    const_reference operator[](size_type pos) const
    {
       return data_[pos];
    }
    
    reference operator()(size_type pos)
    {
      return data_[pos];
    }
    
    const_reference operator()(size_type pos) const
    {
      return data_[pos];
    }
    
    reference operator()(size_type r, size_type c)
    {
      return data_[linear_index(r, c)];
    }
    
    const_reference operator()(size_type r, size_type c) const
    {
      return data_[linear_index(r, c)];
    }
    
    template< typename S, std::size_t N, std::size_t M>
    friend bool operator==(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return lhs.data_ == rhs.data_;
    }
    
    template< typename S, std::size_t N, std::size_t M>
    friend bool operator!=(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return lhs.data_ != rhs.data_;
    }
    
    template< typename S, std::size_t N, std::size_t M>
    friend bool operator<(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return lhs.data_ < rhs.data_;
    }
    
    template<typename S, std::size_t N, std::size_t M>
    friend bool operator<=(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return lhs.data_ <= rhs.data_;
    }
    
    template<typename S, std::size_t N, std::size_t M>
    friend bool operator>(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return lhs.data_ > rhs.data_;
    }
    
    template <typename S, std::size_t N, std::size_t M, typename T>
    friend bool operator>=(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return lhs.data_ >= rhs.data_;
    }
    
    constexpr static bool empty()
    {
      return container_type::empty();
    }
    
    pointer data()
    {
      return data_.data();
    }
    
    const_pointer data() const
    {
      return data_.data();
    }
    
    template <typename S, typename = enable_if_scalar_t<S>>
    matrix& operator*=(S&& s)
    {
      for(auto& elem : data_)
        elem *= s;
      return *this;
    }
    
    template <typename S,typename = enable_if_scalar_t<S>>
    matrix operator*(S&& s) const
    {
      auto ans = *this;
      ans *= s;
      return ans;
    }
    
    template <typename S2>
    matrix& operator+=(const matrix<S2, Rows, Cols>& other)
    {
      std::transform(begin(), end(), other.begin(), begin(), std::plus<value_type>());
      return *this;
    }
    
    template<typename S>
    auto operator+(const matrix<S, Rows, Cols>& other) const
    {
      result_matrix_t<S> res;
      std::transform(begin(), end(), other.begin(), res.begin(), std::plus<value_type>());
      return res;
    }
    
    template <typename S2>
    matrix& operator-=(const matrix<S2, Rows, Cols>& other)
    {
      std::transform (begin(), end(), other.begin(), begin(), std::minus<value_type>());
      return *this;
    }
    
    template<typename S>
    auto operator-(const matrix<S, Rows, Cols>& other) const
    {
      result_matrix_t<S> res;
      std::transform(begin(), end(), other.begin(), res.begin(), std::minus<value_type>());
      return res;
    }
    
    auto operator-() const
    {
      return *this * -1;
    }
    
    matrix operator+() const
    {
     return *this;
    }
    
    template <typename S2, std::size_t Cols2, std::size_t R = Rows, std::size_t C = Cols, typename = std::enable_if_t<R != 1 && C != 1>>
    auto operator*(const matrix<S2, Cols, Cols2>& other) const
    {
      result_matrix_t<S2, Rows, Cols2> prod{};
  
      for(size_type i = 0; i < Rows; ++i)
        for(size_type k = 0; k < Cols2; ++k)
          for(size_type j = 0; j < Cols; ++j)
            prod(i, k) += operator()(i, j) * other(j, k);
      
      return prod;
    }
    
    template <typename S2, std::size_t R = Rows, std::size_t C = Cols, typename = std::enable_if_t<R == 1 && C != 1>>
    auto operator*(const matrix<S2, Cols, 1>& other) const
    {
      decltype(std::declval<Scalar>() * std::declval<S2>()) prod{};
  
      for(size_type j = 0; j < Cols; ++j)
        prod += operator()(j) * other(j);
    
      return prod;
    }
    
    template <typename S2, std::size_t Cols2, std::size_t R = Rows, typename = std::enable_if_t<R == 1>>
    auto operator*(const matrix<S2, Cols, Cols2>& other) const
    {
      result_matrix_t<S2, 1, Cols2> prod{};
  
      for(size_type k = 0; k < Cols2; ++k)
        for(size_type j = 0; j < Cols; ++j)
          prod(k) += operator()(j) * other(j, k);
  
      return prod;
    }
    
    template <typename S2, std::size_t Cols2, std::size_t C = Cols,
     typename = std::enable_if_t<C == 1>, typename = void>
    auto operator*(const matrix<S2, 1, Cols2>& other) const
    {
      result_matrix_t<S2, Rows, Cols2> prod{};
  
      for(size_type i = 0; i < Rows; ++i)
        for(size_type k = 0; k < Cols2; ++k)
            prod(i, k) += operator()(i) * other(k);
    
      return prod;
    }
    
    
    template <typename S2, std::size_t C = Cols, size_t R = Rows, typename = std::enable_if_t<C != 1 && R != 1>, typename = void>
    auto operator*(const matrix<S2, Cols, 1>& other) const
    {
      result_matrix_t<S2, Rows, 1> prod{};
  
      for(size_type i = 0; i < Rows; ++i)
          for(size_type j = 0; j < Cols; ++j)
            prod(i) += operator()(i, j) * other(j);
    
        return prod;
    }
    
    
    template <typename S, typename = enable_if_scalar_t<S>>
    matrix& operator/=(S&& s)
    {
      for(auto& elem: data_)
        elem /= s;
      return *this;
    }
    
    template <typename S, typename = enable_if_scalar_t<S>>
    matrix operator/(S&& s)
    {
      auto ans = *this;
      ans /= s;
      return ans;
    }
    
    template<typename M = matrix, typename = std::enable_if_t<M::is_vector()>>
    value_type sqr_length() const
    {
      return std::inner_product(cbegin(), cend(), cbegin(), value_type{});
    }
    
    template<typename M = matrix, typename = std::enable_if_t<M::is_vector()>>
    value_type length() const
    {
      return std::sqrt(sqr_length());
    }
    
    template<typename M = matrix, typename = std::enable_if_t<M::is_vector()>>
    void normalize()
    {
      value_type l = length();
      if(l != 0)
      {
        l = 1.0 / l;
        *this *= l;
      }
    }
      
    template<typename M = matrix, typename = std::enable_if_t<M::is_vector()>>
    matrix normalized() const
    {
      matrix m = *this;
      m.normalize();
      return m;
    }
    
    matrix<Scalar, Cols, Rows> transposed() const
    {
      matrix<Scalar,Cols,Rows> m;
      for(size_type i = 0; i < Rows;++i)
        for(size_type j = 0; j < Cols; ++j)
          m(j, i) = operator()(i, j);
  
      return m;
    }
    
    template<typename M = matrix, typename = std::enable_if_t<M::is_square()>>
    void transpose()
    {
      *this = transposed();
    }
    
    private:
      container_type data_;
    };
    
    
    namespace detail
    {
      template<typename  S, std::size_t N, std::size_t M>
      struct comma_initializer
      {
        using matrix_type = matrix<S, N, M>;
        
        template <typename S2>
        comma_initializer(matrix_type& m, S2&& s)
          : mat_(m), row_{0}, col_{0}
        {
          *this, (std::forward<S2>(s));
        }
        
        ~comma_initializer()
        {
          assert(row_ + 1  == mat_.nrows() && col_  == mat_.ncols());
        }
        
        template <typename Container, typename = std::enable_if_t<utils::is_container<std::decay_t<Container>>::value> >
        comma_initializer& operator,(Container&& c)
        {
          for(auto&& elem : c)
            *this, elem;
          return *this;
        }
        
        template <typename S2, typename = typename matrix_type::template enable_if_scalar_t<S2>, typename = void>
        comma_initializer& operator,(S2&& s)
        {
          if(col_ == mat_.ncols())
          {
            col_ = 0;
            ++row_;
            assert(row_ < mat_.nrows());
          }
          mat_(row_, col_++) = std::forward<S2>(s);
          return *this;
        }
        
        matrix_type& mat_;
        std::size_t row_;
        std::size_t col_;
      };
    }
    
    template<typename  S, std::size_t N, std::size_t M, typename S2>
    auto operator<<(matrix<S, N, M>& lhs, S2&& value)
    {
      return detail::comma_initializer<S, N, M>(lhs, std::forward<S2>(value));
    }
  
    template<typename S1, typename  S2, std::size_t N, std::size_t M,
      typename = typename matrix<S2, N, M>:: template enable_if_scalar_t<S1, S2>>
    matrix<S2, N, M> operator*(S1&& lhs, matrix<S2, N, M> rhs)
    {
      return rhs *= lhs;
    }
  
  
    template <typename Scalar, std::size_t Dim>
    using square_matrix = matrix<Scalar, Dim, Dim>;
  
    template <typename Scalar, std::size_t Dim>
    using vector = matrix<Scalar, Dim,1>;
  
    template <typename Scalar, std::size_t Dim>
    using row_vector = matrix<Scalar, 1, Dim>;
  
    template <typename Scalar>
    using vector2 = vector<Scalar, 2>;
  
    template <typename Scalar>
    using vector3 = vector<Scalar, 3>;
  
    template <typename Scalar>
    using vector4 = vector<Scalar, 4>;
  
    using vector2f = vector2<float>;
    using vector3f = vector3<float>;
    using vector4f = vector4<float>;
  
    using matrix22f = square_matrix<float, 2>;
  
    using matrix23f = matrix<float, 2, 3>;
    using matrix32f = matrix<float, 3, 2>;
  
    using matrix33f = square_matrix<float, 3>;

    using matrix34f = matrix<float, 3, 4>;
    using matrix43f = matrix<float, 4, 3>;
  
    using matrix44f = square_matrix<float, 4>;
  
  
    template< typename  S, std::size_t N, std::size_t M,
    typename = std::enable_if_t< matrix<S, N, M>::is_vector()> >
    matrix<S,N,M> normalize(const matrix<S, N, M>& v)
    {
      return v.normalized();
    }
  
    template< typename  S, std::size_t N, std::size_t M,
      typename = std::enable_if_t< matrix<S, N, M>::is_vector()> >
    S sqr_length(const matrix<S, N, M>& v)
    {
      return v.sqr_length();
    }
  
  
    template< typename  S, std::size_t N, std::size_t M,
        typename = std::enable_if_t< matrix<S, N, M>::is_vector()> >
    S length(const matrix<S, N, M>& v)
    {
      return v.length();
    }
  
    template< typename  S, std::size_t N, std::size_t M,
      typename = std::enable_if_t< matrix<S,N,M>::is_vector()> >
    S dot(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      return std::inner_product(lhs.cbegin(), lhs.cend(), rhs.cbegin(), S{0});
    }
  
    template<typename S, std::size_t N, std::size_t M>
    matrix<S, M, N> transpose(const matrix<S, N, M>& rhs)
    {
      using size_type = typename matrix<S, M, N>::size_type;
      matrix<S, M, N> m;
      for(size_type i = 0; i < N;++i)
        for(size_type j = 0; j < M; ++j)
          m(j, i) = rhs(i, j);
      return m;
    }
  
    template<typename S, std::size_t N, std::size_t M,
        typename = std::enable_if_t< matrix<S, N, M>::is_vector(3)> >
    matrix<S, N, M> cross(const matrix<S, N, M>& lhs, const matrix<S, N, M>& rhs)
    {
      matrix<S, N, M> m;
      m << lhs.y() * rhs.z() - lhs.z() * rhs.y(),
        lhs.z() * rhs.x() - lhs.x() * rhs.z(),
        lhs.x() * rhs.y() - lhs.y() * rhs.x();
      return m;
    }
  
    template<typename S, std::size_t N, std::size_t M, typename S2>
    auto comp_mult(const matrix<S, N, M>& lhs, const matrix<S2, N, M>& rhs)
    {
      matrix<decltype(std::declval<S>() * std::declval<S2>()), N, M> m;
      std::transform(lhs.begin(), lhs.end(), rhs.begin(), m.begin(), std::multiplies<>());
      return m;
    }
  
    template<typename S, std::size_t N, std::size_t M, typename S2>
    auto comp_div(const matrix<S, N, M>& lhs, const matrix<S2, N, M>& rhs)
    {
      matrix<decltype(std::declval<S>() * std::declval<S2>()), N, M> m;
      std::transform(lhs.begin(), lhs.end(), rhs.begin(), m.begin(), std::divides<>());
      return m;
    }
  
    template<typename S, std::size_t N, std::size_t M, typename S2>
    auto comp_max(const matrix<S, N, M>& lhs, const matrix<S2, N, M>& rhs)
    {
      using S3 = decltype(std::declval<S>() * std::declval<S2>());
      matrix<S3, N, M> m;
      std::transform(lhs.begin(), lhs.end(), rhs.begin(), m.begin(),
        [](S a, S2 b) { return a > b ? a : b; });
      return m;
    }
  
    template<typename S, std::size_t N, std::size_t M, typename S2>
    auto comp_min(const matrix<S, N, M>& lhs, const matrix<S2, N, M>& rhs)
    {
      using S3 = decltype(std::declval<S>() * std::declval<S2>());
      matrix<S3, N, M> m;
      std::transform(lhs.begin(), lhs.end(), rhs.begin(), m.begin(),
        [](S a, S2 b) { return a < b ? a : b; });
      return m;
    }
  
    //compute project of u onto v
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t< matrix<S, N, M>::is_vector(3)> >
    auto projected_length(const matrix<S, N, M>& u, const matrix<S, N, M>& v)
    {
      return dot(u,v) / dot(v,v);
    }
  
    //compute project of u onto v
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t< matrix<S, N, M>::is_vector(3)> >
    auto project(const matrix<S, N, M>& u, const matrix<S, N, M>& v)
    {
      return projected_length(u, v) * v;
    }
  
    //compute orthogonal component of u w.r.t v
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t< matrix<S, N, M>::is_vector(3)> >
    auto orthogonal(const matrix<S, N, M>& u, const matrix<S, N, M>& v)
    {
      return (1 - projected_length(u, v)) * v;
    }
  
  
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t<matrix<S, N, M>::is_vector(3)> >
    auto reflect(const matrix<S, N, M>& v, const matrix<S, N, M>& n)
    {
      return v - 2 * dot(v, n) / dot(n, n) * n;
    }
  
    ///returns the double cross product of vector a, b and c a x(b x c)
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t<matrix<S, N, M>::is_vector(3)> >
    matrix<S, N, M> dbl_cross(const matrix<S, N, M> &a, const matrix<S, N, M> &b, const matrix<S, N, M> &c)
    {
      return dot(a, c) * b - dot(a, b) * c;
    }

    ///returns the spat product (mixed vector product) of the vectors a, b and c
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t<matrix<S, N, M>::is_vector(3)> >
    S spat(const matrix<S, N, M> &a,const matrix<S, N, M> &b, const matrix<S, N, M> &c)
    {
      return dot(cross(a, b), c);
    }
  
    ///calculates the refracted direction of v on a surface with normal n and refraction indices c1,c2 and a bool flag  which indicates total reflection
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t<matrix<S, N, M>::is_vector(3)> >
    std::pair<matrix<S, N, M>, bool> refract(const matrix<S, N, M> &v, const matrix<S, N, M> &n,S c1, S c2)
    {
      S NdotV = - dot(n, v) / dot(n, n);
      S c = c2 / c1;

      S cosasqr = static_cast<S>(1) - (c * c) * (static_cast<S>(1) - NdotV * NdotV);
  
      if(cosasqr < 0)
        return {reflect(v,n), true};
      else
        return {c * v + (c * NdotV - sqrt(cosasqr) / dot(n, n) ) * n, false};
    }
  
    //rotate v ccw by 90 deg
    template< typename S, std::size_t N, std::size_t M,
      typename = std::enable_if_t< matrix<S, N, M>::is_vector(2)> >
    auto rotate90(const matrix<S, N, M>& v)
    {
      return matrix<S, N, M>(-v.y(), v.x());
    }
  
  
    template <typename S, std::size_t N, std::size_t M>
    std::ostream& operator<<(std::ostream& out, const matrix<S, N, M>& m)
    {
      for(std::size_t i = 0; i < N; ++i)
        for(std::size_t j = 0; j < M; ++j)
          out << m(i, j) << ((j == M - 1) ? (i < N-1 ? "\n" : "") : " ");
      return out;
    }
  
    template <std::size_t N, std::size_t M>
    std::ostream& operator<<(std::ostream& out, const matrix<std::uint8_t, N, M>& m)
    {
      for(std::size_t i = 0; i < N; ++i)
        for(std::size_t j = 0; j < M; ++j)
          out << static_cast<int>(m(i, j)) << ((j == M - 1) ? (i < N-1 ? "\n": "") : " ");
      return out;
    }
  
    template <typename S, std::size_t M, std::size_t N>
    std::istream& operator>>(std::istream& in, matrix<S, M, N>& m)
    {
      for(std::size_t i = 0; i < M; ++i)
        for(std::size_t j = 0; j < N; ++j)
          in >> m(i, j);
      return in;
    }
  
  
    ///determinant of 2x2 matrix
    template <typename S>
    S det(const square_matrix<S, 2>& m)
    {
      return m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);
    }
  
    ///determinant of 3x3 matrix
    template <typename S>
    S det(const square_matrix<S,3>& m)
    {
      return m(0, 0) * m(1, 1) * m(2, 2)
        +m(0, 1) * m(1, 2) * m(2, 0)
        +m(0, 2) * m(1, 0) * m(2, 1)
        -m(0, 0) * m(1, 2) * m(2, 1)
        -m(0, 1) * m(1, 0) * m(2, 2)
        -m(0, 2) * m(1, 1) * m(2, 0);
    }
  
    ///determinant of 4x4 matrix
    template <typename S>
    S det(const square_matrix<S,4>& m)
    {
      return
        m(0, 3) * m(1, 2) * m(2, 1) * m(3, 0) - m(0, 2) * m(1, 3) * m(2, 1) * m(3, 0) -
        m(0, 3) * m(1, 1) * m(2, 2) * m(3, 0) + m(0, 1) * m(1, 3) * m(2, 2) * m(3, 0) +
        m(0, 2) * m(1, 1) * m(2, 3) * m(3, 0) - m(0, 1) * m(1, 2) * m(2, 3) * m(3, 0) -
        m(0, 3) * m(1, 2) * m(2, 0) * m(3, 1) + m(0, 2) * m(1, 3) * m(2, 0) * m(3, 1) +
        m(0, 3) * m(1, 0) * m(2, 2) * m(3, 1) - m(0, 0) * m(1, 3) * m(2, 2) * m(3, 1) -
        m(0, 2) * m(1, 0) * m(2, 3) * m(3, 1) + m(0, 0) * m(1, 2) * m(2, 3) * m(3, 1) +
        m(0, 3) * m(1, 1) * m(2, 0) * m(3, 2) - m(0, 1) * m(1, 3) * m(2, 0) * m(3, 2) -
        m(0, 3) * m(1, 0) * m(2, 1) * m(3, 2) + m(0, 0) * m(1, 3) * m(2, 1) * m(3, 2) +
        m(0, 1) * m(1, 0) * m(2, 3) * m(3, 2) - m(0, 0) * m(1, 1) * m(2, 3) * m(3, 2) -
        m(0, 2) * m(1, 1) * m(2, 0) * m(3, 3) + m(0, 1) * m(1, 2) * m(2, 0) * m(3, 3) +
        m(0, 2) * m(1, 0) * m(2, 1) * m(3, 3) - m(0, 0) * m(1, 2) * m(2, 1) * m(3, 3) -
        m(0, 1) * m(1, 0) * m(2, 2) * m(3, 3) + m(0, 0) * m(1, 1) * m(2, 2) * m(3, 3);
    }
  
  
    ///compute inverse of 2x2 matrix
    template <typename S>
    square_matrix<S,2> invert(const square_matrix<S,2>& m)
    {
      S t4 = (S)1.0 / (-m(0, 0) * m(1, 1) + m(0, 1) * m(1, 0));
    
      square_matrix<S,2> minv;
      minv <<  -m(1, 1) * t4,  m(0, 1) * t4,
                m(1, 0) * t4, -m(0, 0) * t4;
    }
  
    ///compute inverse of 3x3 matrix
    template <typename T>
    square_matrix<T,3> invert(const square_matrix<T, 3>& m)
    {
      T t4 = m(2, 0) * m(0, 1);
      T t6 = m(2, 0) * m(0, 2);
      T t8 = m(1, 0) * m(0, 1);
      T t10 = m(1, 0) * m(0, 2);
      T t12 = m(0, 0) * m(1, 1);
      T t14 = m(0, 0) * m(1, 2);
      T t17 = (T)1.0 / (t4 * m(1, 2) - t6 * m(1, 1) - t8 * m(2, 2) + t10 * m(2, 1) + t12 * m(2, 2) - t14 * m(2, 1));
      square_matrix<T,3> minv;
      minv << (m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1)) * t17,
              -(m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1)) * t17,
              (m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1)) * t17,
      
              -(-m(2, 0) * m(1, 2) + m(1, 0) * m(2, 2)) * t17,
              (-t6 + m(0, 0) * m(2, 2)) * t17,
              -(-t10 + t14) * t17,
      
              (-m(2, 0) * m(1, 1) + m(1, 0) * m(2, 1)) * t17,
              -(-t4 + m(0, 0) * m(2, 1)) * t17,
              (-t8 + t12) * t17 ;
      return minv;
    }
  
    //compute inverse of 4x4 matrix
    template <typename T>
    square_matrix<T,4> invert(const square_matrix<T, 4>& m)
    {
      T t1 = m(3, 3) * m(1, 1);
      T t3 = m(3, 2) * m(1, 1);
      T t7 = m(3, 1) * m(1, 2);
      T t9 = m(3, 1) * m(1, 3);
      T t11 = m(3, 2) * m(2, 1);
      T t14 = m(0, 0) * m(1, 1);
      T t19 = m(0, 0) * m(3, 3);
      T t20 = m(1, 2) * m(2, 1);
      T t22 = m(3, 1) * m(0, 0);
      T t23 = m(1, 2) * m(2, 3);
      T t25 = m(1, 3) * m(2, 2);
      T t27 = m(3, 2) * m(0, 0);
      T t28 = m(2, 1) * m(1, 3);
      T t30 = m(1, 1) * m(3, 0);
      T t31 = m(0, 3) * m(2, 2);
      T t33 = m(2, 0) * m(0, 3);
      T t35 = m(0, 2) * m(2, 3);
      T t37 = m(2, 0) * m(0, 2);
      T t39 = m(3, 0) * m(0, 2);
      T t41 = m(3, 1) * m(1, 0);
      T t43 = t14 * m(3, 3) * m(2, 2) - t14 * m(3, 2) * m(2, 3) - t19 * t20 +
      t22 * t23 - t22 * t25 + t27 * t28 - t30 * t31 + t3 * t33 + t30 * t35
      - t1 * t37 - t39 * t28 - t41 * t35;
      T t45 = m(3, 0) * m(0, 1);
      T t47 = m(1, 0) * m(3, 3);
      T t50 = m(2, 0) * m(3, 3);
      T t51 = m(0, 1) * m(1, 2);
      T t53 = m(3, 2) * m(1, 0);
      T t56 = m(0, 2) * m(2, 1);
      T t58 = m(3, 0) * m(0, 3);
      T t63 = m(3, 2) * m(2, 0);
      T t64 = m(0, 1) * m(1, 3);
      T t66 = m(1, 0) * m(0, 3);
      T t68 = -t7 * t33 - t45 * t23 - t47 * m(0, 1) * m(2, 2) + t50 * t51 + t53 *
      m(0, 1) * m(2, 3) + t47 * t56 + t58 * t20 + t9 * t37 + t41 * t31 + t45 *
      t25 - t63 * t64 - t11 * t66;
      T t70 = (T)1.0 / (t43 + t68);
      T t72 = m(3, 3) * m(0, 1);
      T t74 = m(3, 2) * m(0, 1);
      T t78 = m(0, 3) * m(3, 1);
      T t108 = m(2, 0) * m(1, 2);
      T t111 = m(1, 3) * m(3, 0);
      T t131 = m(0, 0) * m(1, 2);
      T t135 = m(1, 0) * m(0, 2);
      T t148 = m(3, 1) * m(2, 0);
      T t150 = m(1, 0) * m(2, 1);
      T t156 = m(0, 0) * m(2, 1);
      T t158 = m(0, 0) * m(2, 3);
      T t161 = m(2, 0) * m(0, 1);
      square_matrix<T, 4> minv;
      minv << (t1 * m(2, 2) - t3 * m(2, 3) - m(3, 3) * m(1, 2) * m(2, 1) +
              t7 * m(2, 3) - t9 * m(2, 2) + t11 * m(1, 3)) * t70,
      
              -(t72 * m(2, 2) - t74 * m(2, 3) - t56 * m(3, 3) + t35 * m(3, 1) -
              t78 * m(2, 2) + m(0, 3) * m(3, 2) * m(2, 1)) * t70,
      
              (t72 * m(1, 2) - t74 * m(1, 3) - t1 * m(0, 2) + m(0, 2) * m(3, 1) *
              m(1, 3) + t3 * m(0, 3) - t78 * m(1, 2)) * t70,
      
              -(t51 * m(2, 3) - t64 * m(2, 2) - m(1, 1) * m(0, 2) * m(2, 3) + t56 *
              m(1, 3) + m(1, 1) * m(0, 3) * m(2, 2) - m(0, 3) * m(1,2) * m(2, 1)) * t70,
      
              -(t47 * m(2, 2) - t53 * m(2, 3) + m(1, 3) * m(3, 2) * m(2, 0) - t108 *
              m(3, 3) + t23 * m(3, 0) - t111 * m(2, 2)) * t70,
      
              (t19 * m(2, 2) - t27 * m(2, 3) - t58 * m(2, 2) + t63 * m(0, 3) + t39 *
              m(2,3) - t50 * m(0,2)) * t70,
     
              -(t19 * m(1, 2) - t27 * m(1, 3) - t47 * m(0, 2) - t58 * m(1, 2) + t111 *
              m(0, 2) + t66 * m(3,2)) * t70,
      
              (t131 * m(2, 3) - m(0, 0) * m(1, 3) * m(2, 2) - t135 * m(2, 3) - t108 *
              m(0, 3) + m(1, 3) * m(2, 0) * m(0, 2) + t66 * m(2, 2)) * t70,
      
              (-m(1, 1) * m(2, 0) * m(3, 3) + m(1, 1) * m(2, 3) * m(3, 0) - t28 *
              m(3, 0) + t148 * m(1, 3) + t150 * m(3, 3) - m(2, 3) * m(3, 1) * m(1, 0)) * t70,
      
              -(t156 * m(3, 3) - t158 * m(3, 1) + t33 * m(3, 1) - t161 * m(3, 3) - m(2, 1) *
              m(3, 0) * m(0, 3) + m(2, 3) * m(3, 0) * m(0, 1)) * t70,
      
             (t19 * m(1, 1) - t22 * m(1, 3) - t58 * m(1, 1) - t47 * m(0,1) + t41 *
             m(0, 3) + t45 * m(1, 3)) * t70,
    
             -(-m(2, 3) * m(1, 0) * m(0, 1) + t158 * m(1, 1) - t33 * m(1, 1) + t161 *
             m(1, 3) - t156 * m(1, 3) + t150 * m(0, 3)) * t70,
      
             -(-t3 * m(2, 0) + t30 * m(2, 2) + t11 * m(1, 0) - m(3, 0) * m(1, 2) *
             m(2, 1) - t41 * m(2, 2) + t7 * m(2, 0)) * t70,
      
             (-t22 * m(2, 2) + t27 * m(2, 1) - t39 * m(2, 1) + t148 * m(0, 2) + t45 *
             m(2, 2) - t63 * m(0, 1)) * t70,
        
             -(-t53 * m(0, 1) + t27 * m(1, 1) - t39 * m(1, 1) + t41 * m(0, 2) - t22 *
             m(1, 2) + t45 * m(1, 2)) * t70,
      
             t70 * (t161 * m(1, 2) - t37 * m(1, 1) - m(1, 0) * m(0, 1) * m(2, 2) + t135 *
             m(2, 1) + t14 * m(2, 2) - t131 * m(2, 1));
    
      return minv;
    }
  

    template <typename T, std::size_t M, std::size_t N>
    matrix<T, M, N> dyad(const vector<T, M>& v1, const vector<T, N>& v2)
    {
      matrix<T, M, N> m;
      for(std::size_t j = 0; j < N; ++j)
        for(std::size_t i = 0; i < M; ++i)
          m(i, j) = v1(i) * v2(j);
      return m;
    }
  
  
    template <typename T, std::size_t M, std::size_t N>
    T trace(const matrix<T, M, N>& m)
    {
      T tr = (T)0;
      std::size_t o = (std::min)(M, N);
      for(std::size_t i = 0; i < o; ++i)
          tr += m(i, i);
      return tr;
    }
  
    template <typename T, std::size_t M, std::size_t N>
    matrix<T, M, N> eye()
    {
      matrix<T, M, N> r{};
      std::size_t o = (std::min)(M, N);
      for(std::size_t i = 0; i < o; i++)
        r(i, i) = (T)1;
      return r;
    }
  
    template <typename T, std::size_t M>
    matrix<T, M, M> eye()
    {
      matrix<T, M, M> r{};
  
      for(std::size_t i = 0; i < M; i++)
        r(i, i) = (T)1;
      return r;
    }
  
    template <typename T,std::size_t M, std::size_t N,
      typename Mat = matrix<T, M, N>, typename = std::enable_if_t<Mat::is_vector(3)> >
    square_matrix<T, 3> cross_mat(const matrix<T, M, N>& v)
    {
      square_matrix<T, 3> m;
      m <<     0, -v(2),  v(1),
            v(2),     0, -v(0),
           -v(1),  v(0),    0;
      return m;
    }
  
    template <typename T, std::size_t M, std::size_t N>
    matrix<T, M, N> ones()
    {
      matrix<T, M, N> r;
      r = T{1};
      return r;
    }
  
    template <typename T, std::size_t N >
    square_matrix<T, N> diag(const vector<T, N>& v)
    {
      square_matrix<T, N> r;
      r = (T)0;
      for(std::size_t i = 0; i < N; i++)
        r(i, i) = v(i);
      return r;
    }
  
    template <typename T, typename S, typename = std::enable_if_t<std::is_convertible<S, double>::value> >
    auto lerp(T start, T end, S alpha)
    {
      return start + (end-start) * alpha;
    }
  
    template <typename T, std::size_t M, std::size_t N,
      typename Engine = std::mt19937, typename Distribution = std::normal_distribution<T>>
    matrix<T, M, N> random_matrix()
    {
      static auto generator = std::bind(Distribution(), owl::utils::create_seeded_engine<Engine>());
      matrix<T, M, N> m;
      std::generate(m.begin(), m.end(), generator);
      return m;
    }
    
    template <typename T, std::size_t M,
      typename Engine = std::mt19937, typename Distribution = std::normal_distribution<T>>
    square_matrix<T, M> random_square_matrix()
    {
      return random_matrix<T, M, M, Engine, Distribution>();
    }
  
    template <typename Scalar>
    std::array<std::size_t, 2> min_abs_components(const vector3<Scalar>& nml)
    {
      auto xabs = std::abs(nml[0]);
      auto yabs = std::abs(nml[1]);
      auto zabs = std::abs(nml[2]);
    
      if(xabs >= yabs)
      {
        if(xabs >= zabs)
        {
          if(nml[0] >= 0)
            return {{1,2}};
          else
            return {{2,1}};
        }
        else
        {
          if(nml[2] >= 0)
            return {{0,1}};
          else
            return {{1,0}};
        }
      }
      else
      {
        if(yabs >= zabs)
        {
          if(nml[1] >= 0)
            return {{2,0}};
          else
            return {{0,2}};
        }
        else
        {
          if(nml[2] >= 0)
            return {{0,1}};
          else
            return {{1,0}};
        }
      }
    }
  
    template <typename T>
    matrix<T,4,4> projection_matrix_from_intrinsics(const matrix<T, 3, 3>& K, int img_width, int img_height, T znear, T zfar)
    {
      T fx = K(0, 0);
      T fy = K(1, 1);
      T skew = K(0, 1);
      assert(skew == 1); //not rectangular pixel case is not supported
      T u0 = K(0, 2);
      T v0 = K(1, 2);
      
      T l = -znear * u0 / fx;
      T r = znear  * (img_width - u0) / fx;
      T b = -znear * v0 / fy;
      T t = znear  *(img_height - v0) / fy;
      return frustrum(l, r, b, t, znear, zfar);
    }
  }
}


