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
#include <cmath>

#include "owl/utils/template_utils.hpp"
#include "owl/math/constants.hpp"

namespace owl
{
  namespace math
  {
    template <typename Scalar>
    class angle
    {
    public:
      using scalar = Scalar;
    
      template <typename S1, typename S2 = Scalar>
      using enable_if_scalar_t = std::enable_if_t<std::is_convertible<S1, S2>::value>;
  
      struct degree_tag {};
      struct radian_tag {};
      struct gon_tag {};
  
      constexpr angle() = default;
  
      template <typename S, typename = enable_if_scalar_t<S>>
      constexpr angle(const S& value, degree_tag)
        : value_(value * constants::pi<scalar> / 180)
      {
      }
  
      template <typename S, typename = enable_if_scalar_t<S>>
      constexpr angle(const S& value, radian_tag)
        : value_(value)
      {
      }
  
      template <typename S, typename = enable_if_scalar_t<S>>
      constexpr angle(const S& value, gon_tag)
        : value_(value * constants::pi<scalar> / 200)
      {
      }
    
      operator scalar() const
      {
        return value_;
      }
  
      scalar degrees() const
      {
        return scalar(value_ * 180 / constants::pi<scalar>);
      }
  
      scalar gons() const
      {
        return scalar(value_ * 200 / constants::pi<scalar>);
      }
  
      scalar radians() const
      {
        return value_;
      }
    
    private:
      scalar value_;
    };
  
    template<typename Scalar>
    angle<Scalar> degrees(const typename utils::identity<Scalar>::type& value)
    {
      return angle<Scalar>(value, typename angle<Scalar>::degree_tag());
    }
  
    template<typename Scalar>
    angle<Scalar> degrees(const angle<Scalar>& value)
    {
      return value;
    }
  
    template<typename Scalar>
    angle<Scalar> radians(const typename utils::identity<Scalar>::type& value)
    {
      return angle<Scalar>(value, typename angle<Scalar>::radian_tag());
    }
  
    template<typename Scalar>
    angle<Scalar> radians(const angle<Scalar>& value)
    {
      return value;
    }
  
    template<typename Scalar>
    angle<Scalar> gons(const typename utils::identity<Scalar>::type& value)
    {
      return angle<Scalar>(value, typename angle<Scalar>::gon_tag());
    }
  
    template<typename Scalar>
    angle<Scalar> gons(const angle<Scalar>& value)
    {
      return value;
    }
  
    template <typename Scalar>
    bool is_acute(const angle<Scalar>& a)
    {
      return a < degrees<Scalar>(90);
    }
  
    template <typename Scalar>
    bool is_obtuse(const angle<Scalar>& a)
    {
      return a > degrees<Scalar>(90) && a < degrees<Scalar>(180);
    }
  
    template <typename Scalar>
    bool is_reflex(const angle<Scalar>& a)
    {
      return a > degrees<Scalar>(180) && a < degrees<Scalar>(360);
    }
  
    template <typename Scalar>
    Scalar cotan(const angle<Scalar>& a)
    {
      return Scalar{1} / std::tan<Scalar>(a);
    }
  }
}
