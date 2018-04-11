//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2018 Sören König. All rights reserved.
//

#pragma once

#include <array>
#include "owl/math/matrix.hpp"
#include "owl/math/interval.hpp"

namespace owl
{
  namespace math
  {
    template <typename Scalar, std::size_t N>
    class triangle
    {
     static_assert(N > 1, "invalid template parameter N");
    public:
      using scalar = Scalar;
      using vector = vector<Scalar, N>;
      using real = std::conditional_t<std::is_same<scalar, double>::value, double, float>;
    
      auto bounds() const
      {
        return math::bounds(positions);
      }
    
      template<bool Cond = (N == 2), typename = std::enable_if_t<Cond>>
      bool inside(const vector& pnt) const
      {
        auto lambdas = edge_funcs(pnt);
        for(const auto& l: lambdas)
          if(l < 0)
            return false;
        return true;
      }
    
      template<bool Cond = (N > 2), typename = std::enable_if_t<Cond>, typename = void>
      bool inside(const vector& pnt, std::size_t u, std::size_t v) const
      {
        auto lambdas = edge_funcs(pnt, u, v);
        for(const auto& l: lambdas)
          if(l < 0)
            return false;
        return true;
      }
  
    
   private:
   
      template<bool Cond = (N == 2), typename = std::enable_if_t<Cond>>
      inline scalar edge_func(const vector &a, const vector &b, const vector&c)
      {
          return (c[0] - a[0]) * (b[1] - a[1]) - (c[1] - a[1]) * (b[0] - a[0]);
      }
   
      template<bool Cond = (N > 2), typename = std::enable_if_t<Cond>, typename = void>
      inline scalar edge_func(const vector &a, const vector &b, const vector&c, std::size_t u, std::size_t v)
      {
          return (c[u] - a[u]) * (b[v] - a[v]) - (c[v] - a[v]) * (b[u] - a[u]);
      }
    
      template<bool Cond = (N == 2), typename = std::enable_if_t<Cond>>
      inline math::vector<real,3> edge_func(const vector& pnt) const
      {
        return math::vector<scalar,3>( edge_function(v1, v2, pnt),
          edge_func(v2, v0, pnt), edge_func(v0, v1, pnt));
      }
    
      template<bool Cond = (N > 2), typename = std::enable_if_t<Cond>, typename = void>
      inline math::vector<real,3> edge_func(const vector& pnt, std::size_t u, std::size_t v) const
      {
         return math::vector<scalar,3>( edge_function(v1, v2, pnt, u,  v),
           edge_func(v2, v0, pnt, u, v), edge_func(v0, v1, pnt, u, v));
      }
 
   public:
   
      union
      {
        std::array<vector, 3> positions;
        struct
        {
          vector v0,v1,v2;
        };
      };
    };
  }
}
