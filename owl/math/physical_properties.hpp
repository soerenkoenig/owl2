
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
#include "owl/math/quaternion.hpp"
#include "owl/math/utils.hpp"

namespace owl
{
  namespace math
  {
    template<typename Scalar>
    class physical_properties
    {
    public:
      physical_properties(Scalar density = 1)
        : surface_area_{0}
        , density_{density}
        , moments{vector<Scalar,10>::zero()}
      {
      
      }
    
    
      template <typename VertexRange>
      void add_face(VertexRange&& vertices)
      {
        if(std::size(vertices) == 3)
        {
          add_triangle(*std::begin(vertices),*std::next(std::begin(vertices),1),
                       *std::next(std::begin(vertices),2));
        }
        else if(std::size(vertices) == 4)
        {
          add_quad(*std::begin(vertices),*std::next(std::begin(vertices),1),
                       *std::next(std::begin(vertices),2), *std::next(std::begin(vertices),3));
        }
        else
        {
          //not yet supported
        }
      }
    
      void add_quad(const vector<Scalar,3>& p1, const vector<Scalar,3>& p2, const vector<Scalar, 3>& p3, const vector<Scalar,3>& p4)
      {
        add_triangle(p1,p2,p3);
        add_triangle(p1,p3,p4);
      }
    
      void add_triangle(const vector<Scalar,3>& p1, const vector<Scalar,3>& p2, const vector<Scalar, 3>& p3)
      {
        auto fn = cross(p2 - p1, p3 - p1);
        surface_area_ += fn.length()/2;
        auto c = (p1 + p2 + p3) / 3;
        auto cfn = comp_mult(c, fn);
      
        m000 += (cfn.x() + cfn.y() + cfn. z()) / 6;

        auto x_2 = ((p1.x() + p2.x()) * (p2.x() + p3.x()) + sqr(p1.x()) + sqr(p3.x()))/12;
        auto y_2 = ((p1.y() + p2.y()) * (p2.y() + p3.y()) + sqr(p1.y()) + sqr(p3.y()))/12;
        auto z_2 = ((p1.z() + p2.z()) * (p2.z() + p3.z()) + sqr(p1.z()) + sqr(p3.z()))/12;

        auto xy = ((p1.x() + p2.x() + p3.x()) * (p1.y() + p2.y() + p3.y()) + p1.x()*p1.y() + p2.x()*p2.y() + p3.x()*p3.y())/24;
    
        auto xz = ((p1.x() + p2.x() + p3.x()) * (p1.z() + p2.z() + p3.z()) + p1.x()*p1.z() + p2.x()*p2.z() + p3.x()*p3.z())/24;
    
        auto yz = ((p1.y() + p2.y() + p3.y()) * (p1.z() + p2.z() + p3.z()) + p1.y()*p1.z() + p2.y()*p2.z() + p3.y()*p3.z())/24;
      
        auto v1 = comp_mult(fn, vector<Scalar,3>(x_2, 2*xy, 2*xz));
        auto v2 = comp_mult(fn, vector<Scalar,3>(2*xy, y_2, 2*yz));
        auto v3 = comp_mult(fn, vector<Scalar,3>(2*xz, 2*yz, z_2));
      
        m100 += (v1.x() + v1.y() + v1.z()) / 6;
        m010 += (v2.x() + v2.y() + v2.z()) / 6;
        m001 += (v3.x() + v3.y() + v3.z()) / 6;
      
        auto x_3 = ((p1.x() + p2.x() + p3.x()) * (sqr(p1.x()) + sqr(p2.x()) + sqr(p3.x())) + p1.x() * p2.x() * p3.x())/20;
        auto y_3 = ((p1.y() + p2.y() + p3.y()) * (sqr(p1.y()) + sqr(p2.y()) + sqr(p3.y())) + p1.y()*p2.y()*p3.y())/20;
        auto z_3 = ((p1.z() + p2.z() + p3.z()) * (sqr(p1.z()) + sqr(p2.z()) + sqr(p3.z())) + p1.z()*p2.z()*p3.z())/20;

        auto x_2y = ((3 * p1.y() + p2.y() + p3.y()) * sqr(p1.x()) + (p1.y() + 3 * p2.y() + p3.y()) * sqr(p2.x()) + (p1.y() + p2.y() + 3 * p3.y()) * sqr(p3.x()) + (2 * p1.y() + 2 * p2.y() + p3.y()) * p1.x() * p2.x() + (2 * p1.y() + p2.y() + 2 * p3.y()) * p1.x() * p3.x() + (p1.y() + 2 * p2.y() + 2 * p3.y()) * p2.x() * p3.x()) / 60;
        auto x_2z = ((3 * p1.z() + p2.z() + p3.z()) * sqr(p1.x()) + (p1.z() + 3 * p2.z() + p3.z()) * sqr(p2.x()) + (p1.z() + p2.z() + 3 * p3.z()) * sqr(p3.x()) + (2 * p1.z() + 2 * p2.z() + p3.z()) * p1.x() * p2.x() + (2 * p1.z() + p2.z() + 2 * p3.z()) * p1.x() * p3.x() + (p1.z() + 2 * p2.z() + 2 * p3.z()) * p2.x() * p3.x())/60;

        auto y_2x = ((3 * p1.x() + p2.x() + p3.x()) * sqr(p1.y()) + (p1.x() + 3 * p2.x() + p3.x()) * sqr(p2.y()) + (p1.x() + p2.x( ) +3 * p3.x()) * sqr(p3.y()) + (2 * p1.x() + 2 * p2.x() + p3.x()) * p1.y() * p2.y() + (2 * p1.x() + p2.x() + 2 * p3.x()) * p1.y() * p3.y() + (p1.x() + 2 * p2.x() + 2 * p3.x()) * p2.y() * p3.y()) / 60;
        auto y_2z = (( 3 * p1.z() + p2.z() + p3.z()) * sqr(p1.y()) + (p1.z() + 3 * p2.z() + p3.z()) * sqr(p2.y()) + (p1.z() + p2.z() + 3 * p3.z()) * sqr(p3.y()) + (2 * p1.z() + 2 * p2.z() + p3.z()) * p1.y() * p2.y() + (2 * p1.z() + p2.z() + 2 * p3.z()) * p1.y()  *p3.y() + (p1.z() + 2 * p2.z() + 2 * p3.z()) * p2.y() * p3.y()) / 60;

        auto z_2y = ((3 * p1.y() + p2.y() + p3.y()) * sqr(p1.z()) + (p1.y() + 3 * p2.y() + p3.y()) * sqr(p2.z()) + (p1.y() + p2.y() + 3 * p3.y()) * sqr(p3.z()) + (2 * p1.y() + 2 * p2.y() + p3.y()) * p1.z() * p2.z() + (2 * p1.y() + p2.y() + 2 * p3.y()) * p1.z() * p3.z() + (p1.y() + 2 * p2.y() + 2 * p3.y()) * p2.z() * p3.z()) / 60;
        auto z_2x = ((3 * p1.x() + p2.x() + p3.x()) * sqr(p1.z()) + (p1.x() + 3 * p2.x() + p3.x()) * sqr(p2.z()) + (p1.x() + p2.x() + 3 * p3.x()) * sqr(p3.z()) + (2 * p1.x() + 2 * p2.x() + p3.x()) * p1.z() * p2.z() + (2 * p1.x() + p2.x() + 2 * p3.x()) * p1.z() * p3.z() + (p1.x() + 2 * p2.x() + 2 * p3.x()) * p2.z() * p3.z()) / 60;

        auto xyz = ((p1.x() + p2.x() + p3.x()) * (p1.y() + p2.y() + p3.y()) * (p1.z() + p2.z() + p3.z()) - (p2.y() * p3.z() + p3.y() * p2.z() - 4 * p1.y() * p1.z()) * p1.x()/2 -(p1.y() * p3.z()+ p3.y() * p1.z() - 4 * p2.y() * p2.z()) * p2.x() / 2 - (p1.y() * p2.z() + p2.y() * p1.z() - 4 * p3.y() * p3.z()) * p3.x() / 2) / 60;


        auto w1 = comp_mult(fn, vector<Scalar,3>(x_2y, y_2x, 2*xyz));
        auto w2 = comp_mult(fn, vector<Scalar,3>(x_2z, 2*xyz, z_2x));
        auto w3 = comp_mult(fn, vector<Scalar,3>(2*xyz, y_2z, z_2y));

        m110 += (w1.x() + w1.y() + w1.z()) / 6;
        m101 += (w2.x() + w2.y() + w2.z()) / 6;
        m011 += (w3.x() + w3.y() + w3.z()) / 6;

        auto w4 = comp_mult(fn, vector<Scalar,3>(x_3, 3*x_2y, 3*x_2z));
        auto w5 = comp_mult(fn, vector<Scalar,3>(3*y_2x, y_3, 3*y_2z));
        auto w6 = comp_mult(fn, vector<Scalar,3>(3*z_2x, 3*z_2y, z_3));

        m200 += (w4.x() + w4.y() + w4.z()) / 9;
        m020 += (w5.x() + w5.y() + w5.z()) / 9;
        m002 += (w6.x() + w6.y() + w6.z()) / 9;
      }
    
      Scalar volume() const
      {
        return m000;
      }
    
      Scalar surface_area() const
      {
        return surface_area_;
      }
    
      Scalar mass() const
      {
        return density_ * volume();
      }
    
      vector<Scalar,3> centroid()
      {
        if(m000 == 0)
         return vector<Scalar,3>::zero();
      
        return vector<Scalar,3>(m100, m010, m001) / m000;
      }
    
      square_matrix<Scalar,3> inertia_tensor() const
      {
        if(m000 == 0)
          return square_matrix<Scalar,3>::zero();
          
        auto Ixx = m020 + m002 - (sqr(m010) + sqr(m001)) / m000;
        auto Iyy = m200 + m002 - (sqr(m100) + sqr(m001)) / m000;
        auto Izz = m200 + m020 - (sqr(m100) + sqr(m010)) / m000;
        auto Ixy = m110 - m100 * m010 / m000;
        auto Ixz = m101 - m100 * m001 / m000;
        auto Iyz = m011 - m010 * m001 / m000;

        square_matrix<Scalar,3> I;
        I <<  Ixx, -Ixy, -Ixz,
             -Ixy, Iyy, -Iyz,
             -Ixz, -Iyz, Izz;
        return I;
      
      }
    
      square_matrix<Scalar,3> inertia_tensor_principal_frame() const
      {
        auto I = inertia_tensor();
        square_matrix<Scalar,3> Q = principal_orientation(I);
        return Q * I * transpose(Q);
      }
    
      quaternion<Scalar> principal_orientation(const square_matrix<Scalar,3> &A) const
      {
        // A must be a symmetric matrix.
        // returns quaternion q such that its corresponding matrix Q
        // can be used to Diagonalize A
        // Diagonal matrix D = Q * A * Transpose(Q);  and  A = QT*D*Q
        // The rows of q are the eigenvectors D's diagonal is the eigenvalues
        // As per 'row' convention if float3x3 Q = q.getmatrix(); then v*Q = q*v*conj(q)
        int maxsteps = 24;  // certainly wont need that many.
      
        quaternion<Scalar> q(0,0,0,1);
        for(int i=0; i < maxsteps; i++)
        {
          square_matrix<Scalar,3> Q  = q; // v*Q == q*v*conj(q)
          square_matrix<Scalar,3> D  = Q * A * transpose(Q);  // A = Q^T*D*Q
          vector<Scalar,3> offdiag(D(1,2),D(0,2),D(0,1)); // elements not on the diagonal
          vector<Scalar,3> om(std::fabs(offdiag.x()),std::fabs(offdiag.y()),std::fabs(offdiag.z()));
        
          int k = (om.x() > om.y() && om.x() > om.z()) ? 0: (om.y() > om.z()) ? 1 : 2; // index of largest element of offdiag
          int k1 = (k+1)%3;
          int k2 = (k+2)%3;
          if(offdiag[k] == 0.0f) break;  // diagonal already
          Scalar thet = (D(k2,k2) - D(k1,k1))/(2.0f*offdiag[k]);
          Scalar sgn = (thet > 0.0f)?1.0f:-1.0f;
          thet *= sgn; // make it positive
          Scalar t = sgn /(thet +((thet < 1.E6f)?sqrtf(sqr(thet)+1.0f):thet)) ; // sign(T)/(|T|+sqrt(T^2+1))
          Scalar c = 1.0f/sqrtf(sqr(t)+1.0f); //  c= 1/(t^2+1) , t=s/c
          if(c == 1.0f) break;  // no room for improvement - reached machine precision.
          quaternion<Scalar> jr(0,0,0,0); // jacobi rotation for this iteration.
          jr[k] = sgn * std::sqrt((1.0f-c)/2.0f);  // using 1/2 angle identity sin(a/2) = sqrt((1-cos(a))/2)
          jr[k] *= -1.0; // since our quat-to-matrix convention was for v*M instead of M*v
          jr.w()  = sqrtf(1.0 - sqr(jr[k]));
          if(jr.w() == 1.0f) break; // reached limits of floating point precision
          q =  q*jr;
          q.normalize();
        }
        return q;
      }
    
      Scalar surface_area_;
      Scalar density_;
    
      union
      {
        vector<Scalar, 10> moments;

        struct
        {
          Scalar m000;
          Scalar m100;
          Scalar m010;
          Scalar m001;
          Scalar m110;
          Scalar m101;
          Scalar m011;
          Scalar m200;
          Scalar m020;
          Scalar m002;
        };
      };
    };
  }
  
}


