//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <type_traits>

#include "owl/utils/container_utils.hpp"

namespace owl
{
  namespace math
  {
    namespace detail
    {
      inline bool compare_margin(double lhs, double rhs, double margin)
      {
        return (lhs + margin >= rhs) && (rhs + margin >= lhs);
      }
    }
  
    inline bool compare_equal(double lhs, double rhs, double margin, double epsilon, double scale)
    {
      return detail::compare_margin(lhs, rhs, margin) ||
        detail::compare_margin(lhs, rhs, epsilon * (scale + std::fabs(lhs)));
    }
  
    template <typename Container, typename = std::enable_if_t<owl::utils::is_container<Container>::value> >
    bool compare_equal(const Container& lhs, const Container& rhs, double margin, double epsilon, double scale)
    {
      return std::equal(lhs.begin(),lhs.end(),rhs.begin(),
        [&](const double& l, const double& r)
        {
          return compare_equal(l, r, margin, epsilon, scale);
        });
    }
 
    template <typename T>
    class approximately
    {
      static_assert(std::is_lvalue_reference<T>::value || std::is_rvalue_reference<T>::value,
                    "must be an r or l value reference");
    public:
      
        explicit approximately(T value)
          : epsilon_(std::numeric_limits<float>::epsilon() * 100)
          , margin_(0.0)
          , scale_(0.0)
          , value_(std::forward<T>(value))
        {}
    
        
        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator==(const T2& lhs, const approximately& rhs)
        {
          auto lhs_v = static_cast<std::decay_t<T>>(lhs);
          auto rhs_v = rhs.value_;
          return compare_equal(rhs_v, lhs_v, rhs.margin_, rhs.epsilon_ ,rhs.scale_);
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator==(const approximately& lhs, const T2& rhs)
        {
          return operator==(rhs, lhs);
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator!=(const T2& lhs, const approximately& rhs)
        {
          return !operator==(lhs, rhs);
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator!=(approximately const& lhs, const T2& rhs)
        {
          return !operator==(rhs, lhs);
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator<=(const T2& lhs, const approximately& rhs)
        {
          return static_cast<double>(lhs) < rhs.value_ || lhs == rhs;
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator<=(const approximately& lhs, const T2& rhs)
        {
          return lhs.value_ < static_cast<std::decay_t<T>>(rhs) || lhs == rhs;
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator>=(const T2& lhs, const approximately& rhs)
        {
          return static_cast<std::decay_t<T>>(lhs) > rhs.value_ || lhs == rhs;
        }

        template <typename T2, typename = typename std::enable_if_t<std::is_constructible<std::decay_t<T>, T2>::value>>
        friend bool operator>=(const approximately& lhs, const T2& rhs)
        {
          return lhs.value_ > static_cast<std::decay_t<T>>(rhs) || lhs == rhs;
        }

        template <typename Scalar,
          typename = typename std::enable_if_t<std::is_constructible<double, Scalar>::value>>
        approximately& epsilon(const Scalar& s)
        {
          double eps_as_double = static_cast<double>(s);
          if(eps_as_double < 0 || eps_as_double > 1.0)
          {
            throw std::domain_error("Invalid parameter, epsilon has to be between 0 and 1" );
          }
          epsilon_ = eps_as_double;
          return *this;
        }

        template <typename Scalar,
          typename = typename std::enable_if_t<std::is_constructible<double, Scalar>::value>>
        approximately& margin(const Scalar& s)
        {
          double margin_as_double = static_cast<double>(s);
          if(margin_as_double < 0)
          {
            throw std::domain_error("Invalid parameter, margin has to be non-negative." );
          }
          margin_ = margin_as_double;
          return *this;
        }

        template <typename Scalar,
          typename = typename std::enable_if_t<std::is_constructible<double, Scalar>::value>>
        approximately& scale(const Scalar& newScale)
        {
          scale_ = static_cast<double>(newScale);
          return *this;
        }
    
      const std::decay_t<T>& value() const { return value_; }
    
     private:
        double epsilon_;
        double margin_;
        double scale_;
        T value_;
    };
  
    
    template <typename T>
    approximately<T&&> approx(T&& val)
    {
      return approximately<T&&>(std::forward<T>(val));
    }
  
    template <typename T>
    approximately<T&> approx(T& val)
    {
      return approximately<T&>(val);
    }
  
  
    template <typename T>
    std::ostream& operator<<(std::ostream& out, const approximately<T>& a)
    {
      return out << a.value();
    }
  }
}



