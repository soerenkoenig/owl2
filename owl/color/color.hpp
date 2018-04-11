//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <cstdint>
#include <algorithm>
#include <array>
#include <type_traits>
#include "owl/math/matrix.hpp"
#include "owl/optional.hpp"
#include "owl/color/channel_traits.hpp"

namespace owl
{
  namespace color
  {
    template <typename T, std::size_t N, bool HasAlpha, template <typename, bool> typename Derived>
    class color
    {
    public:
    
      using vector_type = std::conditional_t<HasAlpha, math::vector<T, N + 1>, math::vector<T, N>>;
      using value_type = T;
      using reference = T&;
      using pointer = T*;
      using const_reference = const T&;
      using const_pointer = const T*;
      using size_type = typename vector_type::size_type;
      using iterator = typename vector_type::iterator;
      using const_iterator = typename vector_type::const_iterator;
    
      static constexpr std::size_t num_channels() { return  HasAlpha ? N + 1 : N; }
    
      color() = default;
      color(const color&) = default;
      color(color&&) = default;
      color& operator=(const color&) = default;
      color& operator=(color&&) = default;
    
      explicit color(const math::vector<T,N>& other)
        : channels_{other}
      {
      }
    
      explicit color(vector_type&& other)
        : channels_{std::forward<vector_type>(other)}
      {
      }
    
      template <typename... Args, typename = std::enable_if_t<sizeof...(Args) == (HasAlpha ? N + 1 : N) >>
      color(Args&&... args)
        : channels_{static_cast<value_type>(std::forward<Args>(args))...}
      {
      }
    
      color& operator=(vector_type&& other)
      {
        channels_ = std::forward<vector_type>(other);
        return *this;
      }
    
      color& operator=(const vector_type& other)
      {
        channels_ = other;
        return *this;
      }
    
      explicit operator const vector_type&() const
      {
        return channels_;
      }
    
      explicit operator vector_type&()
      {
        return channels_;
      }
    
      size_type size() const
      {
        return channels_.size();
      }
    
      iterator begin()
      {
        return channels_.begin();
      }
    
      const_iterator begin() const
      {
        return channels_.begin();
      }
    
      const_iterator cbegin() const
      {
        return channels_.begin();
      }
    
      iterator end()
      {
        return channels_.end();
      }
    
      const_iterator end() const
      {
        return channels_.end();
      }
    
      const_iterator cend() const
      {
        return channels_.end();
      }
    
      reference operator[](size_type i)
      {
        return channels_[i];
      }
    
      const_reference operator[](size_type i) const
      {
        return channels_[i];
      }
    
      reference operator()(size_type i)
      {
        return channels_[i];
      }
    
      const_reference operator()(size_type i) const
      {
        return channels_[i];
      }
    
      bool operator==(const color& other) const
      {
        return channels_ == other.channels_;
      }
    
      bool operator!=(const color& other) const
      {
        return channels_ != other.channels_;
      }
    
      bool operator<(const color& other) const
      {
        return channels_ < other.channels_;
      }
    
      bool operator<=(const color& other) const
      {
        return channels_ <= other.channels_;
      }
    
      bool operator>(const color& other) const
      {
        return channels_ > other.channels_;
      }
    
      bool operator>=(const color& other) const
      {
        return channels_ >= other.channels_;
      }
    
      template <typename T2>
      auto operator+(const color<T2, N, HasAlpha, Derived>& other) const
      {
        auto res = channels_ + static_cast<const owl::math::vector<T2,N>&>(other);
        return Derived<decltype(std::declval<T>() * std::declval<T2>()), HasAlpha>(res);
      }
    
      template <typename T2>
      color& operator+=(const color<T2, N, HasAlpha, Derived>& other)
      {
        channels_ += static_cast<const owl::math::vector<T2,N>&>(other);
        return *this;
      }
    
      template <typename T2>
      auto operator-(const color<T2, N, HasAlpha, Derived>& other) const
      {
       return Derived<decltype(std::declval<T>() * std::declval<T2>()), HasAlpha>(channels_ - static_cast<const owl::math::vector<T2,N>&>(other));
      }
    
      template <typename T2>
      color& operator-=(const color<T2, N, HasAlpha, Derived>& other)
      {
        channels_ -= static_cast<const owl::math::vector<T2,N>&>(other);
        return *this;
      }
    
      template <typename T2>
      auto operator*(const color<T2,N,HasAlpha,Derived>& other) const
      {
        return Derived<decltype(std::declval<T>() * std::declval<T2>()), HasAlpha>(comp_mult(channels_, other.channels_));
      }
    
      template <typename T2>
      color& operator*=(const color<T2, N, HasAlpha, Derived>& other)
      {
        channels_ =  comp_mult(channels_, static_cast<const owl::math::vector<T2,N>&>(other));
        return *this;
      }
    
      auto operator*(color&& other) const
      {
        return Derived<T, HasAlpha>(comp_mult(channels_, other.channels_));
      }
    
      template <typename T2>
      auto operator/(const color& other) const
      {
        return Derived<decltype(std::declval<T>() * std::declval<T2>()), HasAlpha>(comp_div(channels_, other.channels_));
      }
    
      auto operator/(color&& other) const
      {
        return Derived<T, HasAlpha>(comp_div(channels_, other.channels_));
      }
    
      template <typename T2>
      color& operator/=(const color<T2, N, HasAlpha, Derived>& other)
      {
        channels_ =  comp_div(channels_, static_cast<const owl::math::vector<T2,N>&>(other));
        return *this;
      }
    
      template <typename Scalar, typename = typename vector_type::template enable_if_scalar_t<Scalar> >
      auto operator*(Scalar s) const
      {
        return Derived<decltype(std::declval<T>() * std::declval<Scalar>()), HasAlpha>(channels_ * s);
      }
    
      template<bool Dummy = true, typename = std::enable_if_t<Dummy && HasAlpha>>
      const_reference a() const
      {
        return (*this)[N];
      }
    
      template<bool Dummy = true, typename = std::enable_if_t<Dummy && HasAlpha>>
      reference a()
      {
        return (*this)[N];
      }
    
    private:
      vector_type channels_;
    };
  
    template <typename Scalar,typename T, std::size_t N, bool HasAlpha, template <typename, bool> typename Derived, typename = typename math::vector<T,N>::template enable_if_scalar_t<Scalar> >
    auto operator*(Scalar s, const color<T, N, HasAlpha, Derived>& col)
    {
      return col * s;
    }
  
    template <typename T, std::size_t N, bool HasAlpha, template <typename, bool> typename Derived>
    std::ostream& operator<<(std::ostream& out, const color<T, N, HasAlpha, Derived>& col)
    {
      return out << static_cast<const typename color<T, N, HasAlpha, Derived>::vector_type&>(col);
    }
  
  
    template<typename T, bool HasAlpha = false>
    class cmyk : public color<T, 4, HasAlpha, cmyk>
    {
    public:
      using base_type = color<T, 4, HasAlpha, owl::color::cmyk>;
      using color<T, 4, HasAlpha, owl::color::cmyk>::color;
      using typename base_type::value_type;
      using typename base_type::reference;
      using typename base_type::const_reference;
      using typename base_type::iterator;
      using typename base_type::const_iterator;
   
      const_reference c() const { return (*this)[0];}
      reference c() { return (*this)[0]; }
      const_reference m() const { return (*this)[1];}
      reference m() { return (*this)[1]; }
      const_reference y() const { return (*this)[2];}
      reference y() { return (*this)[2]; }
      const_reference k() const { return (*this)[3];}
      reference k() { return (*this)[3]; }
    };
  
    template<typename Channel, bool HasAlpha = false>
    class rgb : public color<Channel, 3, HasAlpha, rgb>
    {
    public:
  
      using base_type = color<Channel, 3, HasAlpha,  owl::color::rgb>;
      using color<Channel, 3, HasAlpha, owl::color::rgb>::color;
      using typename base_type::value_type;
      using typename base_type::reference;
      using typename base_type::const_reference;
      using typename base_type::iterator;
      using typename base_type::const_iterator;
    
   
      const_reference r() const { return (*this)[0];}
      reference r() { return (*this)[0]; }
      const_reference g() const { return (*this)[1];}
      reference g() { return (*this)[1]; }
      const_reference b() const { return (*this)[2];}
      reference b() { return (*this)[2]; }
    
      static rgb hex(std::uint32_t hexValue)
      {
        rgb c;
        if constexpr(HasAlpha)
          c.a() = channel_traits<Channel>::convert(std::uint8_t(( hexValue >> 24 ) & 255));
        c.r() = channel_traits<Channel>::convert(std::uint8_t(( hexValue >> 16 ) & 255));
        c.g() = channel_traits<Channel>::convert(std::uint8_t(( hexValue >> 8 ) & 255));
        c.b() = channel_traits<Channel>::convert(std::uint8_t(hexValue & 255));
        return c;
      }
    };
  
    template<typename Channel, bool HasAlpha = false>
    class bgr : public color<Channel, 3, HasAlpha, bgr>
    {
    public:
      using base_type = color<Channel, 3, HasAlpha, owl::color::bgr>;
      using color<Channel, 3, HasAlpha, owl::color::bgr>::color;
      using typename base_type::value_type;
      using typename base_type::reference;
      using typename base_type::const_reference;
      using typename base_type::iterator;
      using typename base_type::const_iterator;
   
      const_reference b() const { return (*this)[0];}
      reference b() { return (*this)[0]; }
      const_reference g() const { return (*this)[1];}
      reference g() { return (*this)[1]; }
      const_reference r() const { return (*this)[2];}
      reference r() { return (*this)[2]; }
    };
  
    template<typename Channel, bool HasAlpha = false>
    class hsv : public color<Channel, 3, HasAlpha, hsv>
    {
    public:
      using base_type = color<Channel, 3, HasAlpha, owl::color::hsv>;
      using color<Channel, 3, HasAlpha, owl::color::hsv>::color;
      using typename base_type::value_type;
      using typename base_type::reference;
      using typename base_type::const_reference;
      using typename base_type::iterator;
      using typename base_type::const_iterator;
   
      const_reference h() const { return (*this)[0];}
      reference h() { return (*this)[0]; }
      const_reference s() const { return (*this)[1];}
      reference s() { return (*this)[1]; }
      const_reference v() const { return (*this)[2];}
      reference v() { return (*this)[2]; }
    };
  
    template <typename T>
    using gray = T;
  
    template <typename T>
    using rgba = rgb<T, true>;
  
    template <typename T>
    using bgra = bgr<T, true>;
  
    using gray8u = gray<std::uint8_t>;
    using rgb8u = rgb<std::uint8_t>;
    using bgr8u = bgr<std::uint8_t>;
    using cmyk8u = cmyk<std::uint8_t>;
    using rgba8u = rgb<std::uint8_t, true>;
    using bgra8u = bgr<std::uint8_t, true>;
  
    using gray16u = gray<std::uint16_t>;
    using rgb16u = rgb<std::uint16_t>;
    using bgr16u = bgr<std::uint16_t>;
    using cmyk16u = cmyk<std::uint16_t>;
    using rgba16u = rgb<std::uint16_t, true>;
    using bgra16u = bgr<std::uint16_t, true>;

    using gray32f = gray<float>;
    using rgb32f = rgb<float>;
    using cmyk32f = cmyk<float>;
    using hsv32f = hsv<float>;
    using rgba32f = rgb<float, true>;
    using bgra32f = bgr<float, true>;

    using gray64f = gray<double>;
    using rgb64f = rgb<double>;
    using cmyk64f = cmyk<double>;
    using hsv64f = hsv<double>;
    using rgba64f = rgb<double, true>;
    using bgra64f = bgr<double, true>;
  
    
  }
}
