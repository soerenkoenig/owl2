//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright (c) 2018 Sören König. All rights reserved.
//

#include "owl/math/matrix.hpp"
#include "owl/math/trafos.hpp"
#include "owl/math/euler_angles.hpp"
#include "owl/math/approx.hpp"
#include "catch/catch.hpp"

namespace test
{

  TEST_CASE( "matrix transpose", "[math]" )
  {
    owl::math::matrix33f m;
    m << 1,2,3,
      4,5,6,
      7,8,9;
    CHECK(m.is_square());

    owl::math::matrix33f mt;
    mt << 1,4,7,
      2,5,8,
      3,6,9;

    CHECK(m.transposed() == mt);
    m.transpose();
    CHECK(m == mt);

    owl::math::matrix23f m2;
    m2 << 1,2,3,
      4,5,6;
    CHECK_FALSE(m2.is_square());

    owl::math::matrix32f m2t;
    m2t << 1,4,
      2,5,
      3,6;

    CHECK(m2.transposed() == m2t);

  }

  TEST_CASE( "matrix multiplications", "[math]" )
  {
    owl::math::matrix33f m;
    m << 1,2,3,
      4,5,6,
      7,8,9;

    owl::math::vector3f v;
    v << 1,4,7;

    auto vt = transpose(v);

    owl::math::vector3f v2;
    v2 << 30, 66, 102;

    owl::math::row_vector<float,3> v3;
    v3 << 66, 78, 90;

    CHECK(m*v == v2);
    CHECK(vt*m == v3);
    CHECK(vt*v == 66);
    CHECK(vt*v == sqr_length(v));

    owl::math::matrix33f m2;
    m2 << 1,  4,  7,
      4, 16, 28,
      7, 28, 49;

    CHECK( v*vt == m2);

  }


  TEST_CASE( "matrix", "[math]" )
  {
    using namespace owl::math;
    auto tr = translate<float>(vector3f(0,0,0),vector3f(1,2,3));
    CHECK(approx(tr * vector4f(0,0,0,1)) == vector4f(1,2,3,1));
    CHECK(approx(rotate_x<float,3>(degrees<float>(0))) == matrix33f::identity());
    CHECK(approx(det(rotate_y<float,3>(degrees<float>(4.9)))) == 1);
    CHECK(approx(det(rotate_z<float,3>(degrees<float>(4.9)))) == 1);

    auto trt = rotate(vector3f(1,0,0),vector3f(1,1,0),degrees<float>(90));

    CHECK(approx(trt*vector4f(0,0,0,1)) == vector4f(1,0,1,1));

    CHECK(approx(rotate_x<float,3>(degrees<float>(3.6)) * rotate_x<float,3>(degrees<float>(-3.6))) == matrix33f::identity());

    auto I = matrix44f::identity();
    auto V = vector4f{1,2,3,4};
    CHECK(approx(I*V) == vector4f(1,2,3,4));

    auto m = random_matrix<double,4,4>();
    CHECK(approx(invert(m)*m).margin(0.00000001) == matrix44f::identity());

    matrix33f R;
    R << 0.973076000000000  , 0.230486000000000,  -0.000000000061821,
      -0.008435560000000,   0.035613700000000 ,  0.999330000000000 ,
      0.230331000000000 , -0.972424000000000,   0.036599100000000;


    auto eul = euler_angles<float>(R);
    auto mat = matrix33f(eul);
    CHECK(approx(mat).margin(0.000001) == R);

    euler_angles<float> eul2{radians<float>(-1.53419), radians<float>(-1.68913e-09), radians<float>(-0.232577)};
    auto R2 = rotate_x<float,3>(eul2.pitch) * rotate_y<float,3>(radians<float>(eul2.yaw)) * rotate_z<float,3>(radians<float>(eul2.roll));
    auto mat2 = matrix33f(eul2);
    CHECK(approx(mat2).margin(0.000001) == R2);
    auto RX = rotate_x<float,3>(degrees<float>(30));

  }
}


