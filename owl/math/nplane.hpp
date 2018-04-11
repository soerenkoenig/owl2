//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once
#include "owl/optional.hpp"
#include "owl/math/matrix.hpp"
#include "owl/math/ray.hpp"

namespace owl
{
  namespace math
  {
  
    template <typename Scalar, std::size_t Dimension>
    class nplane
    {
    public:
      using scalar = Scalar;
      using vector = vector<Scalar, Dimension>;
      using homog_vector = math::vector<Scalar, Dimension + 1>;
   
      template <typename S1>
      using enable_if_scalar_t = typename vector::template enable_if_scalar_t<S1>;
    
      nplane() = default;
    
      nplane(const homog_vector& arr)
        : data_(arr)
      {
        normalize();
      }
    
      template<typename S, typename... Args, typename = enable_if_scalar_t<S> >
      nplane(S&& nx, Args&&... args)
        : data_{std::forward<S>(nx), std::forward<Args>(args)...}
      {
        normalize();
      }
    
      const vector& normal() const
      {
        return reinterpret_cast<const vector&>(data_);
      }
    
      scalar distance() const
      {
        return -data_[3];
      }
    
      scalar operator()(const vector& vec)
      {
        return dot(normal(), vec) + data_[3];
      }
    
      scalar operator()(const homog_vector& hvec)
      {
        return dot(data_, hvec);
      }
    
    private:
      void normalize()
      {
        auto len = normal().length();
        if(len != 0)
          data_ /= len;
      }
    
      homog_vector data_;
    };
  
    template<typename Scalar>
    using line = nplane<Scalar, 2>;
  
    template<typename Scalar>
    using plane = nplane<Scalar, 3>;
  
  
    using planef = plane<float>;
    using planed = plane<double>;
    using linef = line<float>;
    using lined = line<double>;
  
     /*plane(const vector_type& point, const vector_type& normal)
      {
        auto it = std::copy(normal.begin(), normal.end(), data_.begin());
        *it = -dot(normal, point);
      
        normalize();
      }*/
  
    template<typename T, std::size_t N>
    nplane<T, N> nplane_from_point_and_normal(const vector<T, N>& point, const vector<T, N>& normal)
    {
      vector<T, N + 1> coeffs;
      coeffs << normal, -dot(normal, point);
      return nplane<T, N>(coeffs);
    }
    
    template<typename T, std::size_t N>
    T distance(const nplane<T, N>& pl, const vector<T, N>& pnt)
    {
      return dot(pl.normal(), pnt) - pl.distance();
    }
  
    template<typename T, std::size_t N>
    std::optional<T> intersect(const ray<T, N> r, const nplane<T, N>& pl)
    {
      T denom = dot( pl.normal(), r.direction());
      if( denom == 0)
        return std::nullopt;
    
      T t = distance(pl, r.origin) / denom;
      if (t < 0)
        return std::nullopt;
      return t;
    }
  }
}
