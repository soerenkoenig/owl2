#pragma once


namespace math
{
  template <typename Scalar>
  class camera
  {
    void set_camera_matrix(Scalar f = 4.1, Scalar sensorSizeX = 4.89 /*mm*/, Scalar sensorSizeY=3.67, Scalar resX, Scalar resY)
    {
      fx = f * resX / sensorSizeX;
      fy = f * resY / sensorSizeY;
      Scalar cx = resX/2.;
      Scalar cy = resY/2.;
      principal_point.set(cx, cy);
    }
  
    void look_at(const vector<Scalar,3>& eye, const vector<Scalar,3>& target, vector<Scalar,3> up)
    {
      vector<S,3>  f = normalize(target - eye);
      up.normalize();
      vector<S,3> s = cross(f, up);
      s.normalize();
      vector<S,3> u = cross(s, f);
    
      R <<  s(0),  s(1),  s(2),
            u(0),  u(1),  u(2),
           -f(0), -f(1), -f(2);
      t.set(-dot(s, eye), -dot(u, eye), -dot(f, eye));
    }
  
    square_matrix<Scalar, 3> camera_matrix() const
    {
      square_matrix<Scalar, 3> K;
      K << fx,  0, principal_point.x(),
            0,    fy, principal_point.y(),
            0,     0,                   1;
      return K;
    }
  
    square_matrix<Scalar,3> inv_camera_matrix() const
    {
      square_matrix<Scalar, 3> Kinv;
      Kinv.zeros();
      Kinv(0, 0) = (Scalar)1 / fx;
      Kinv(2, 2) = 1;

      Kinv(1, 1) = (Scalar)1 / fy;
      Kinv(1, 2) = -principal_point[1] / fy;
      Kinv(0, 2) = -principal_point[0] / fx;
    
    
      return Kinv;
    }
  
    //return camera eye in world coordinates
    vector<Scalar, 3> eye() const
    {
      return -transpose(R) * t;
    }
  
    vector<Scalar, 3> ray_direction(const Scalar& u, const Scalar& v) const
    {
      vector<Scalar, 3> xc(u,v,1);
      return normalize(Minv * xc);
    }
  
    vector<Scalar,3> row_plane_normal(const Scalar& y) const
    {
      vector<Scalar,3> l((Scalar)0, (Scalar)-1, y);
      return normalize(Mt * l);
    }

    vector<T,3> get_column_plane_normal(const Scalar& x) const
    {
      vector<Scalar,3> l((Scalar)-1, (Scalar)0, x);
      return normalize(Mt * l);
    }
  
    vector<Scalar,2> project_point(const vector<Scalar,3>& p) const
    {
      vector<Scalar,3> pl = R*p + t;
      vector<Scalar,2> q(pl[0]/pl[2], pl[1]/pl[2]);
      vector<Scalar, 2> u = principal_point;
      u[0] += fx*q[0];
      u[1] += fy*q[1];
      return u;
    }
  
  
  
    square_matrix<Scalar,3> R = square_matrix<Scalar,3>::identity();
    vector<Scalar,3> t = vector<Scalar,3>::zeros();
  
  
    Scalar fx, fy, skew;
    vector<Scalar,2> principal_point = {320, 240};
    std::size_t width = 640;
    std::size_t height = 480;
    Scalar znear = 1;
    Scalar zfar = 100;
  
    square_matrix<Scalar,3> K;
    square_matrix<Scalar,3> Mt;//M = (KR)^T
    square_matrix<Scalar,3> Minv;// Minv = (KR)^(-1)
  };
}
