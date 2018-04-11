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
#include "owl/math/angle.hpp"
#include "owl/math/interval.hpp"

namespace owl
{
  namespace math
  {
   template<typename S,typename T>
    square_matrix<S,4> perspective(const T& fovy, const T&aspect, const T& znear, const T& zfar)
    {
      S fovyr = (S)(fovy*3.14159 / 180.0);
      S f = (S)(cos(fovyr/2.0f) / sin(fovyr / 2.0f));
      square_matrix<S,4> m;
      m << f / aspect,  0,                         0,                            0,
                    0,  f,                         0,                            0,
                    0,  0, (zfar+znear)/(znear-zfar),  (2*zfar*znear)/(znear-zfar),
                    0,  0,                        -1,                            0;
      return m;
    }
  
    template<typename S, typename T>
    square_matrix<S,4> frustrum(const T& left, const T&right, const T& bottom, const T&top, const T& znear, const T& zfar)
    {
      S A = (right + left) / (right - left);
      S B = (top + bottom) / (top - bottom);
      S C = -(zfar + znear) / (zfar - znear);
      S D = -2 * zfar * znear / (zfar - znear);
      square_matrix<S,4> m;
      m << 2 * znear / (right - left),                          0,  A, 0,
                                    0, 2 * znear / (top - bottom),  B, 0,
                                    0,                          0,  C, D,
                                    0,                          0, -1, 0;
      return m;
    }
  
    template <typename S, typename T>
    square_matrix<S,4> ortho(T left, T right, T bottom, T top, T zNear = -1, T zFar = 1)
    {
      T rl = right -  left;
      T tb = top - bottom;
      T zfn = zFar - zNear;
  
      square_matrix<S,4> m;
      m << T(2) / rl,         0, 0,               -(right + left) / rl,
                   0, T(2) / tb, 0,               -(top + bottom) / tb,
                   0,            0, -T(2) / zfn, -(zFar + zNear) / zfn,
                   0,            0,           0,                     1;
      return m;
    }
  
  
  
    ///creates a picking matrix like gluPickMatrix with pixel (0,0) in the lower left corner if flipy=false
    // x,y is the center of the region
    template<typename S, typename T>
    const square_matrix<S,4> pick(const T& x,const T& y,const T& width, const T& height,
                               const std::array<T,4>& viewport,
                               const square_matrix<T,4> &modelviewproj, bool flipy = true)
    {
      S sx, sy;
      S tx, ty;
      sx = viewport[2] / width;
      sy = viewport[3] / height;
      tx = (S)(viewport[2] + 2.0 * (viewport[0] - x)) / width;
      if(flipy)
        ty = (S)(viewport[3] + 2.0 * (viewport[1] - (viewport[3]-y))) / height;
      else
        ty = (S)(viewport[3] + 2.0 * (viewport[1] - y)) / height;
      square_matrix<S,4> m;
       m << sx,  0, 0, tx,
             0, sy, 0, ty,
             0,  0, 1, 0,
             0,  0, 0, 1;
      return m * modelviewproj;
    }
  
    template <typename S, typename T>
    square_matrix<S,4> translate(const T& tx, const T& ty, const T& tz)
    {
      square_matrix<S,4> m;
      m << 1,   0,  0, tx,
           0,   1,  0, ty,
           0,   0,  1, tz,
           0,   0,  0,  1;
      return m;
    }
  
    template <typename S, typename T>
    square_matrix<S,4> translate(const vector<T, 3>& t)
    {
      square_matrix<S,4> m;
       m << 1, 0, 0, t.x(),
            0, 1, 0, t.y(),
            0, 0, 1, t.z(),
            0, 0, 0,     1;
      return m;
    }
  
    template <typename T>
    square_matrix<T,4> translate(const vector<T, 3>& source, const vector<T,3>& dest)
    {
      return translate<T>(dest - source);
    }
  
    template <typename S, std::size_t N, typename T>
    square_matrix<S,N> scale(const T& sx, const T& sy, const T& sz)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
       square_matrix<S,N> m;
       if constexpr(N == 3)
       {
         m << sx,  0,  0,
               0, sy,  0,
               0,  0, sz;
       }
       if constexpr(N == 4)
       {
         m << sx,  0,  0, 0,
              0, sy,  0, 0,
              0,  0, sz, 0,
              0,  0,  0, 1;
       }

      return m;
    
    }
  
    template <typename T, std::size_t N>
    square_matrix<T,N> scale(const vector<T, 3>& s)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
      square_matrix<T,N> m;
      if constexpr (N == 3)
      {
        m << s.x(),    0,     0,
                 0,s.y(),     0,
                 0,    0, s.z();
      }
      if constexpr (N == 4)
      {
        m << s.x(),    0,     0, 0,
                 0,s.y(),     0, 0,
                 0,    0, s.z(), 0,
                 0,    0,     0, 1;
      }
      return m;
    }
  
    template <typename S, std::size_t N, typename T>
    square_matrix<S,N> uniform_scale(const T& s)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
      square_matrix<S,N> m;
      if constexpr (N == 3)
      {
        m << s, 0, 0,
             0, s, 0,
             0, 0, s;
      }
      if constexpr (N == 4)
      {
        m << s, 0, 0, 0,
             0, s, 0, 0,
             0, 0, s, 0,
             0, 0, 0, 1;
      }
      return m;
    }
  
    template<typename T, std::size_t N>
    square_matrix<T,N> rotate(const vector<T, 3> axis, const angle<T>& theta)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
      if(theta == 0)
        return eye<T,4>();
    
      square_matrix<T,3> R = eye<T,3>();
      vector<T,3> dirn =  axis;
      dirn.normalize();
      square_matrix<T,3> omega = cross_mat(dirn);
  
      R += sin(theta)*omega + (1.0 - cos(theta)) * (omega * omega);
      if constexpr (N == 3)
        return R;
    
      if constexpr(N == 4)
      {
        square_matrix<T,4> m;
        m << R(0,0), R(0,1), R(0,2), 0,
            R(1,0), R(1,1), R(1,2), 0,
            R(2,0), R(2,1), R(2,2), 0,
                  0,      0,      0, 1;
        return m;
      }
    }
  
    template<typename T>
    square_matrix<T,4> rotate(const vector<T, 3>& axis_start, const vector<T, 3>& axis_end, const angle<T>& theta)
    {
      vector<T,3> axis = axis_end - axis_start;
      return translate<T>(axis_start) * rotate<T,4>(axis,theta) * translate<T>(-axis_start);
    }
  
  
    template <typename T, std::size_t N>
    square_matrix<T,N> rotate_x(const angle<T>& theta)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
      T Cos = cos(theta);
      T Sin = sin(theta);
      square_matrix<T,N> m;
      if constexpr (N == 3)
      {
        m << 1,   0, 0,
             0, Cos, -Sin,
             0, Sin, Cos;
      }
      if constexpr (N == 4)
      {
        m << 1,   0,    0, 0,
             0, Cos, -Sin, 0,
             0, Sin,  Cos, 0,
             0,   0,    0, 1;
      }
      return m;
    }
  
  
  
    template <typename T, std::size_t N>
    square_matrix<T,N> rotate_y(const angle<T>& theta)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
      T Cos = cos(theta);
      T Sin = sin(theta);
      square_matrix<T,N> m;
      if constexpr( N == 3)
      {
        m <<  Cos, 0, Sin,
                0, 1, 0,
             -Sin, 0,  Cos;
      }
      if constexpr(N == 4)
      {
        m <<  Cos, 0,  Sin, 0,
                0, 1,    0, 0,
             -Sin, 0,  Cos, 0,
                0, 0,    0, 1;
      }
      return m;
    }
  
  
  
    template <typename T, std::size_t N>
    square_matrix<T,N> rotate_z(const angle<T>& theta)
    {
      static_assert(N == 3 || N == 4, "only 3 or 4 is allowed for template parameter N");
      T Cos = cos(theta);
      T Sin = sin(theta);
      square_matrix<T,N> m;
      if constexpr(N == 3)
      {
        m << Cos, -Sin, 0,
             Sin,  Cos, 0,
               0,    0, 1;
      }
      if constexpr(N == 4)
      {
        m << Cos, -Sin, 0, 0,
             Sin,  Cos, 0, 0,
               0,    0, 1, 0,
               0,    0, 0, 1;
      }
      return m;
    }
  
  
    template<typename S>
    square_matrix<S,4> lookat(const vector<S,3>& eye, const vector<S,3>& target, vector<S,3> up)
    {
      vector<S,3>  f = normalize(target - eye);
      up.normalize();
      vector<S,3> s = cross(f, up);
      s.normalize();
      vector<S,3> u = cross(s, f);
      square_matrix<S,4> m;
      m <<  s(0),  s(1),  s(2), -dot(s, eye),
            u(0),  u(1),  u(2), -dot(u, eye),
           -f(0), -f(1), -f(2), -dot(f, eye),
               0,     0,     0,            1;
      return m;
    }

  
    template <typename Scalar>
    matrix<Scalar,4,4> auto_center_and_scale(const box<Scalar>& source, const box<Scalar>& dest)
    {
      auto svec = comp_div(dest.extents(), source.extents());
      auto s = *std::min_element(svec.begin(), svec.end());
      auto cd = dest.center();
      cd.y() = dest.lower_bound.y();
      auto cs = source.center();
      cs.y() = source.lower_bound.y();
      auto tr = cd/s -  cs;
      return uniform_scale<Scalar,4>(s)* translate<Scalar>(tr) ;
    }
  
    template <typename Scalar, std::size_t N>
    vector<Scalar, N+1> homog_point(const vector<Scalar, N>& pos)
    {
      vector<Scalar, N+1> v;
      v << pos, 1;
      return v;
    }
  
    template <typename Scalar, std::size_t N>
    vector<Scalar, N+1> homog_normal(const vector<Scalar, N>& nml)
    {
      vector<Scalar, N+1> v;
      v << nml, 0;
      return v;
    }
  
    template <typename Scalar, std::size_t N>
    inline vector<Scalar, N-1> unhomog_point(const vector<Scalar, N>& pos)
    {
      vector<Scalar, N-1> v;
      auto pit = pos.begin();
      auto vit = v.begin();
      auto vend = v.end();
      auto& w = pos.back();
      while(vit != vend)
        *vit++ = *pit++ / w;
      return v;
    }
  
    template <typename Scalar, std::size_t N>
    inline vector<Scalar, N-1> unhomog_normal(const vector<Scalar, N>& nml)
    {
      vector<Scalar, N-1> v;
      auto nit = nml.begin();
      auto vit = v.begin();
      auto vend = v.end();
      while(vit != vend)
        *vit++ = *nit++;
      return v;
    }

  }
}
