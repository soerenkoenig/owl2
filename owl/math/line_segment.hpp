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
    class line_segment
    {
    public:
      using scalar = Scalar;
      using vector = vector<Scalar, Dimension>;
    
      line_segment(const vector& start, const vector& end)
        : points{start, end}
      {
      }
    
      union
      {
        std::array<vector, 2> points;
        struct
        {
          vector start;
          vector end;
        };
      };
    };
  
    using line_segment3f = line_segment<float, 3>;
    using line_segment3d = line_segment<double, 3>;
    using line_segment2f = line_segment<float, 2>;
    using line_segment2d = line_segment<double, 2>;
  
  
  }
}

