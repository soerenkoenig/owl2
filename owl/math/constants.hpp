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

namespace owl
{
  namespace math
  {
    namespace constants
    {
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      pi = Scalar(3.14159265358979323846);
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      two_pi = Scalar{2} * pi<Scalar>;
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      pi_2 = Scalar(1.57079632679489661923);

      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      pi_4 = Scalar(0.785398163397448309616);
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      sqrt_2 = Scalar{1.41421356237309504880};
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      sqrt1_2 = Scalar{0.707106781186547524401};
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      e = Scalar(2.71828182845904523536);
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      log2e = Scalar{1.44269504088896340736};
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      log10e = Scalar{0.434294481903251827651};
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      ln2 = Scalar{0.693147180559945309417};
  
      template<typename Scalar>
      constexpr std::enable_if_t<std::is_floating_point<Scalar>::value, Scalar>
      ln10 = Scalar{2.30258509299404568402};
    }
  }
}


