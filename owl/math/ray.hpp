//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once
#include "owl/math/matrix.hpp"

namespace owl
{
  namespace math
  {
    template<typename Scalar, std::size_t Dimension>
    class ray
    {
    public:
      static_assert(Dimension > 1, "invalid parameter Dimension");
      
      using scalar = Scalar;
      using vector = vector<Scalar, Dimension>;
      
      ray() = default;
      
      ray(const vector &origin, const vector& direction)
        : origin(origin)
      {
        set_direction(direction);
      }
      
      void set_direction(const vector& direction)
      {
        direction_ = direction;
        inv_direction_ = vector(scalar(1) / direction.x(), scalar(1) / direction.y(), scalar(1) / direction.z());
      }
      
      vector operator()(const scalar& t) const
      {
        return origin + direction_ * t;
      }
      
      const vector& direction() const
      {
        return direction_;
      }
      
      const vector& inv_direction() const
      {
        return inv_direction_;
      }
      
      vector origin;
      
    private:
      vector inv_direction_;
      vector direction_;
    };
    
    using ray3f = ray<float, 3>;
    using ray3d = ray<double, 3>;
    using ray2f = ray<float, 2>;
    using ray2d = ray<double, 2>;
  }
}
