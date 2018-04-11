#include "owl/math/quaternion.hpp"
#include "owl/math/trafos.hpp"
#include "owl/math/approx.hpp"
#include "catch/catch.hpp"

namespace test
{

  TEST_CASE( "quaternion", "[math]" )
  {
    using namespace owl::math;
    matrix33f R = rotate_x<float,3>(degrees<float>(30));
    quaternionf q = R;
    matrix33f R2 = q;
    CHECK(approx(R2) == R);
  
    quaternionf q2;
    q2.set_from_axis_angle(vector3f(1,0,0), degrees<float>(30));
    matrix33f R3 = q2;
    CHECK(approx(R3) == R);
  
    euler_angles<float> euler(degrees<float>(14), degrees<float>(25), degrees<float>(70));
    matrix33f RR = euler;

    matrix33f RR2 = rotate_x<float,3>(euler.pitch)
      * rotate_y<float,3>(euler.yaw) * rotate_z<float,3>(euler.roll);
    CHECK(approx(RR2) == RR);
  
    quaternion<float> q3 = euler;
    matrix33f RR3 = q3;
    CHECK(approx(RR3) == RR);
  
    euler_angles<float> euler2 = q3;
    CHECK(approx(euler) == euler2);
  
  }
  
}

