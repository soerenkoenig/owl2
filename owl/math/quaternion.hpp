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
#include "owl/math/euler_angles.hpp"
#include "owl/math/angle.hpp"

namespace owl
{
  namespace math
  {
    template <typename Scalar>
    class quaternion
    {
    public:
      using scalar = Scalar;
      using matrix33 = square_matrix<Scalar, 3>;
      using matrix44 = square_matrix<Scalar, 4>;
      using vector3 = vector<Scalar, 3>;
      using vector4 = vector<Scalar, 4>;
      using angle = angle<Scalar>;
      using euler_angles = euler_angles<Scalar>;
    
      quaternion()
        : data_{0, 0, 0, 1}
      {
      }
    
      quaternion(const scalar& x, const scalar& y, const scalar& z, const scalar& w)
        : data_{x, y, z, w}
      {
      }
    
      quaternion(const matrix33& m)
      {
        *this = m;
      }
    
      quaternion(const matrix44& m)
      {
        *this = m;
      }
    
      quaternion(const euler_angles& other)
      {
        *this = other;
      }
    
      quaternion(const vector3& axis, const angle &theta)
      {
        set_from_axis_angle(axis, theta);
      }
    
      quaternion& operator=(const euler_angles& other)
      {
        scalar cy = cos(other.yaw / 2);
        scalar sy = sin(other.yaw / 2);
        scalar cz = cos(other.roll / 2);
        scalar sz = sin(other.roll / 2);
        scalar cx = cos(other.pitch / 2);
        scalar sx = sin(other.pitch / 2);
    
        w() = cx * cy * cz - sx * sy * sz;
        x() = sx * cy * cz + cx * sy * sz;
        y() = cx * sy * cz - sx * cy * sz;
        z() = cx * cy * sz + sx * sy * cz;
    
        return *this;
      }
    
      template <typename Matrix, typename = std::enable_if_t<Matrix::is_square(3) || Matrix::is_square(4)> >
      quaternion& operator=(const Matrix& m)
      {
        std::size_t i = 2;
        if(m(0,0) > m(1,1))
        {
          if(m(0,0) > m(2,2))
            i = 0;
        }
        else
        {
          if(m(1,1) > m(2,2))
            i = 1;
        }
  
        if (m(0,0) + m(1,1) + m(2,2) > m(i,i))
        {
          w() = sqrt(m(0,0) + m(1,1) + m(2,2) + 1.0) / 2.0;
          x() = (m(2,1) - m(1,2)) / (4.0 * w());
          y() = (m(0,2) - m(2,0)) / (4.0 * w());
          z() = (m(1,0) - m(0,1)) / (4.0 * w());
        }
        else
        {
          std::size_t j = (i + 1) % 3;
          std::size_t k = (i + 2) % 3;

          data_[i] = sqrt(m(i,i) - m(j,j) - m(k,k) + 1.0) / 2.0;
          data_[j] = (m(i,j) + m(j,i)) / (4.0 * data_[i]);
          data_[k] = (m(i,k) + m(k,i)) / (4.0 * data_[i]);
          w() = (m(k,j) - m(j,k)) / (4.0 * data_[i]);
        }
        return *this;
      }
    
    
      operator matrix33() const
      {
        const scalar q00 = 2 * data_[0] * data_[0];
        const scalar q11 = 2 * data_[1] * data_[1];
        const scalar q22 = 2 * data_[2] * data_[2];
        const scalar q01 = 2 * data_[0] * data_[1];
        const scalar q02 = 2 * data_[0] * data_[2];
        const scalar q03 = 2 * data_[0] * data_[3];
        const scalar q12 = 2 * data_[1] * data_[2];
        const scalar q13 = 2 * data_[1] * data_[3];
        const scalar q23 = 2 * data_[2] * data_[3];
    
        matrix33 m;
      
        m << 1 - q11 - q22,     q01 - q23,     q02 + q13,
                 q01 + q23, 1 - q22 - q00,     q12 - q03,
                 q02 - q13,     q12 + q03, 1 - q11 - q00;
    
        return m;
      }
    
      operator matrix44() const
      {
      
        const scalar q00 = 2 * data_[0] * data_[0];
        const scalar q11 = 2 * data_[1] * data_[1];
        const scalar q22 = 2 * data_[2] * data_[2];
        const scalar q01 = 2 * data_[0] * data_[1];
        const scalar q02 = 2 * data_[0] * data_[2];
        const scalar q03 = 2 * data_[0] * data_[3];
        const scalar q12 = 2 * data_[1] * data_[2];
        const scalar q13 = 2 * data_[1] * data_[3];
        const scalar q23 = 2 * data_[2] * data_[3];
    
        matrix44 m;
      
        m << 1.0 - q11 - q22,       q01 - q23,       q02 + q13, 0,
                   q01 + q23, 1.0 - q22 - q00,       q12 - q03, 0,
                   q02 - q13,       q12 + q03, 1.0 - q11 - q00, 0,
                           0,               0,               0, 1;
        return m;
      }
    
      operator euler_angles() const
      {
        euler_angles euler;
        auto r11 = -2 * (y() * z() - w() * x());
        auto r12 =  w() * w() - x() * x() - y() * y() + z() * z();
        auto r21 =  2 * (x() * z() + w() * y());
        auto r31 = -2 * (x() * y() - w() * z());
        auto r32 =  w() * w() + x() * x() - y() * y() - z() * z();
        euler.roll = radians<scalar>(std::atan2( r31, r32 ));
        euler.yaw = radians<scalar>(std::asin ( r21 ));
        euler.pitch = radians<scalar>(std::atan2( r11, r12 ));
        return euler;
      }
    
      quaternion inverse() const
      {
        return quaternion(-x(), -y(), -z(), w());
      }
  
      void invert()
      {
        imag() *= -1;
      }
    
      void negate()
      {
        data_ *= -1;
      }
    
      quaternion operator-() const
      {
        quaternion q = *this;
        q.negate();
        return q;
      }
    
      const scalar& operator()(std::size_t index) const
      {
        return data_[index];
      }
    
      scalar& operator()(std::size_t index)
      {
        return data_[index];
      }
    
      const scalar& operator[](std::size_t index) const
      {
        return data_[index];
      }
    
      scalar& operator[](std::size_t index)
      {
        return data_[index];
      }
    
      const scalar& x() const
      {
        return data_[0];
      }
    
      scalar& x()
      {
        return data_[0];
      }
    
      const scalar& y() const
      {
        return data_[1];
      }
    
      scalar& y()
      {
        return data_[1];
      }
    
      const scalar& z() const
      {
        return data_[2];
      }
    
      scalar& z()
      {
        return data_[2];
      }
    
      const scalar& w() const
      {
        return data_[3];
      }
    
      scalar& w()
      {
        return data_[3];
      }
    
      const scalar& real() const
      {
        return data_[3];
      }
    
      scalar& real()
      {
        return data_[3];
      }
    
      const vector3& imag() const
      {
        return reinterpret_cast<const vector3&>(*this);
      }
    
      vector3& imag()
      {
        return reinterpret_cast<vector3&>(*this);
      }
    
      template< typename S>
      friend bool operator==(const quaternion<S>& lhs, const quaternion<S>& rhs)
      {
        return lhs.data_ == rhs.data_;
      }
    
      template< typename S>
      friend bool operator!=(const quaternion<S>& lhs, const quaternion<S>& rhs)
      {
        return lhs.data_ != rhs.data_;
      }
    
      template< typename S>
      friend bool operator<(const quaternion<S>& lhs, const quaternion<S>& rhs)
      {
        return lhs.data_ < rhs.data_;
      }
    
      template<typename S>
      friend bool operator<=(const quaternion<S>& lhs, const quaternion<S>& rhs)
      {
        return lhs.data_ <= rhs.data_;
      }
    
      template<typename S>
      friend bool operator>(const quaternion<S>& lhs, const quaternion<S>& rhs)
      {
        return lhs.data_ > rhs.data_;
      }
    
      template <typename S>
      friend bool operator>=(const quaternion<S>& lhs, const quaternion<S>& rhs)
      {
        return lhs.data_ >= rhs.data_;
      }
    
      template <typename S>
      friend quaternion<S> operator*(const S& a, const quaternion<S>& b)
      {
        return quaternion<S>(a * b[0], a * b[1], a * b[2], a * b[3]);
      }
    
      quaternion& operator*=(const quaternion &q)
      {
        *this = (*this) * q;
        return *this;
      }

      template <typename S1, typename S2>
      friend auto operator*(const quaternion<S1>& a, const quaternion<S2>& b)
      {
        using S3 = decltype(std::declval<S1>() * std::declval<S2>());
        return quaternion<S3>(a[3] * b[0] + b[3] * a[0] + a[1] * b[2] - a[2] * b[1],
                              a[3] * b[1] + b[3] * a[1] + a[2] * b[0] - a[0] * b[2],
                              a[3] * b[2] + b[3] * a[2] + a[0] * b[1] - a[1] * b[0],
                              a[3] * b[3] - b[0] * a[0] - a[1] * b[1] - a[2] * b[2]);
      }
    
      void normalize()
      {
        data_.normalize();
      }
    
      void set(const scalar& x, const scalar& y, const scalar& z, const scalar& w)
      {
        data_[0] = x;
        data_[1] = y;
        data_[2] = z;
        data_[3] = w;
      }
    
      void set_from_axis_angle(const vector3& axis, const angle& theta)
      {
        scalar factor = sin(theta / 2);
        imag() = factor * axis;
        real() = cos(theta / 2);
        normalize();
      }

    private:
      vector4 data_;
    };
  
    template <typename Scalar>
    std::ostream& operator<<(std::ostream& out,const quaternion<Scalar>& q)
    {
      return out << q.w() << " + " << q.x() << "i + "<< q.y() << "j + " << q.z() << "k";
    }
  
    template <typename Scalar>
    bool compare_equal(const quaternion<Scalar>& lhs, const quaternion<Scalar>& rhs, double margin, double epsilon, double scale)
    {
      return compare_equal(lhs.real(), rhs.real(), margin, epsilon, scale) &&
        compare_equal(lhs.imag(), rhs.imag(), margin, epsilon, scale);
    }
  
    using quaternionf = quaternion<float>;
    using quaterniond = quaternion<double>;
  }
}

