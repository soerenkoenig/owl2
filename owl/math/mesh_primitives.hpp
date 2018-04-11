//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include "owl/math/mesh.hpp"

namespace owl
{
  namespace math
  {
   /*
   
   Y is above
    
    Y
    2 -- 3
   /.   /|
  6 -- 7 |
  | 0..|.1  X
  |.   |/
  4 -- 5
 Z
 */
    template <typename Scalar>
    mesh<Scalar> create_box()
    {
      std::array<math::vector<Scalar,3>,8> positions = {{math::vector<Scalar,3>{0,0,0},
      math::vector<Scalar,3>{1,0,0},
      math::vector<Scalar,3>{0,1,0},
      math::vector<Scalar,3>{1,1,0},
      math::vector<Scalar,3>{0,0,1},
      math::vector<Scalar,3>{1,0,1},
      math::vector<Scalar,3>{0,1,1},
      math::vector<Scalar,3>{1,1,1}}};
      mesh<float> m;
      auto vertices = m.add_vertices(positions);
    
      std::array<math::vector<Scalar,2>,4> texcoord_right =
      {{
        math::vector<Scalar,2>{0.5,0.25},
        math::vector<Scalar,2>{0.5,0.5},
        math::vector<Scalar,2>{0.75,0.5},
        math::vector<Scalar,2>{0.75,0.25}
      }};
      m.set_face_texcoords(m.add_face(vertices[1],vertices[3],vertices[7],vertices[5]), texcoord_right);

      std::array<math::vector<Scalar,2>,4> texcoord_left =
      {{
        math::vector<Scalar,2>{0,0.5},
        math::vector<Scalar,2>{0.25,0.5},
        math::vector<Scalar,2>{0.25,0.25},
        math::vector<Scalar,2>{0,0.25}
      }};
      m.set_face_texcoords(m.add_face(vertices[0],vertices[4],vertices[6],vertices[2]),texcoord_left);
      std::array<math::vector<Scalar,2>,4> texcoord_top =
      {{
        math::vector<Scalar,2>{0.25,0.0},
        math::vector<Scalar,2>{0.25,0.25},
        math::vector<Scalar,2>{0.5,0.25},
        math::vector<Scalar,2>{0.5,0.0}
      }};
      m.set_face_texcoords(m.add_face(vertices[2], vertices[6], vertices[7], vertices[3]), texcoord_top);
    
      std::array<math::vector<Scalar,2>,4> texcoord_bottom =
      {{
        math::vector<Scalar,2>{0.25,0.75},
        math::vector<Scalar,2>{0.5,0.75},
        math::vector<Scalar,2>{0.5,0.5},
        math::vector<Scalar,2>{0.25,0.5}
      }};
      m.set_face_texcoords(m.add_face(vertices[0], vertices[1], vertices[5], vertices[4]), texcoord_bottom);
    
      std::array<math::vector<Scalar,2>,4> texcoord_front =
      {{
        math::vector<Scalar,2>{0.25,0.5},
        math::vector<Scalar,2>{0.5,0.5},
        math::vector<Scalar,2>{0.5,0.25},
        math::vector<Scalar,2>{0.25,0.25}
      }};
      m.set_face_texcoords(m.add_face(vertices[4], vertices[5], vertices[7], vertices[6]), texcoord_front);
    
      std::array<math::vector<Scalar,2>,4> texcoord_back =
      {{
        math::vector<Scalar,2>{1,0.5},
        math::vector<Scalar,2>{1.0,0.25},
        math::vector<Scalar,2>{0.75,0.25},
        math::vector<Scalar,2>{0.75,0.5}
      }};
      m.set_face_texcoords(m.add_face(vertices[0],vertices[2],vertices[3],vertices[1]), texcoord_back);
      m.update_normals(radians<Scalar>(0));
      return m;
    }
  
    template <typename Scalar>
    mesh<Scalar> create_tetradedron()
    {
      std::array<math::vector<Scalar,3>,4> positions = {{math::vector<Scalar,3>{0,0,0},
      math::vector<Scalar,3>{1,0,0},
      math::vector<Scalar,3>{0,1,0},
      math::vector<Scalar,3>{0,0,1}}};
   
      mesh<float> m;
      auto vertices = m.add_vertices(positions);
    
      m.add_face(vertices[0], vertices[2], vertices[1]);
      m.add_face(vertices[0], vertices[3], vertices[2]);
      m.add_face(vertices[0], vertices[1], vertices[3]);
      m.add_face(vertices[1], vertices[2], vertices[3]);
      m.update_normals();
      return m;
    }
  
    template <typename Scalar>
    mesh<Scalar> create_icosaeder(const Scalar& radius = Scalar(1))
    {
      mesh<Scalar> m;
  
      Scalar a  = (Scalar)(radius * 4.0/sqrt(10.0 + 2.0 * sqrt(5.0)));
      Scalar h = (Scalar)cos(2.0 * asin(a/(2.0 * radius))) * radius;
      Scalar r2 = (Scalar)sqrt(radius * radius - h * h);
    
      std::array<vector<Scalar,3>, 12> points;
      int k = 0;
      points[k++] =  vector<Scalar,3>(0,radius,0);
      for(int i = 0; i < 5; i++)
        points[k++] = vector<Scalar,3>(cos(i * degrees<Scalar>(72)) * r2, h, -sin(i * degrees<Scalar>(72)) * r2);
      for(int i = 0; i < 5; i++)
       points[k++] = vector<Scalar,3>(cos(degrees<Scalar>(36) + i * degrees<Scalar>(72)) * r2, -h, -sin(degrees<Scalar>(36) + i * degrees<Scalar>(72)) * r2);
      points[k] = vector<Scalar,3>(0, -radius, 0);

      auto vhandles = m.add_vertices(points);
    
      for(std::size_t i = 0; i < 5; i++)
      {
        m.add_face(vhandles[0], vhandles[i + 1], vhandles[(i + 1) % 5 + 1]);
        m.add_face(vhandles[11], vhandles[(i + 1) % 5 + 6], vhandles[i + 6]);
        m.add_face(vhandles[i + 1], vhandles[i + 6], vhandles[(i + 1) % 5 + 1]);
        m.add_face(vhandles[(i + 1) % 5 + 1], vhandles[i + 6], vhandles[(i + 1) % 5 + 6]);
      }
       m.update_normals(radians<Scalar>(0));
      return m;
    }
  
    template <typename Scalar>
    mesh<Scalar> create_triangle()
    {
      mesh<Scalar> m;
    
      std::array<vector3<Scalar>,3> positions =
        { vector3<Scalar>{0, 0,  0},
          vector3<Scalar>{1, 0,  0},
          vector3<Scalar>{(Scalar)0.5,(Scalar) 1,  0} };
    
      auto vhandles = m.add_vertices(positions);
      m.add_face(vhandles[0], vhandles[1], vhandles[2]);
      return m;
    }
  
    template <typename Scalar, typename Scalar2>
    mesh<Scalar> create_geodesic_sphere(Scalar2 radius = 1, std::size_t levels = 2)
    {
      mesh<Scalar> m = create_icosaeder<Scalar>(radius);
    
      for(std::size_t i = 0; i < levels; ++i)
      {
        auto n_old  = m.vertices().size();
        m.subdivide_triangle_split();
        auto verts = m.vertices();
      
        for(auto& pos: m.positions(make_iterator_range(verts).advance_begin(n_old)))
          pos = radius * normalize(pos);
      }
      m.update_normals();
      return m;
    }
  
    //create a sphere mesh
    template <typename Scalar, typename Scalar2>
    mesh<Scalar> create_sphere(Scalar2 radius, std::size_t slices = 48, std::size_t stacks = 48)
    {
      assert(slices >= 3 && stacks >= 3);
    
      mesh<Scalar> m;
    
      std::size_t n = slices * (stacks - 1) + 2;
      std::vector<vector<Scalar,3>> positions;
      positions.reserve(n);

      positions.emplace_back(0, radius, 0);
    
      int k = 1;
      for(std::size_t i = 1; i < stacks; i++)
      {
        Scalar angle1 = constants::pi_2<Scalar> - (Scalar)(i * constants::pi<Scalar>)/(Scalar)stacks;
        Scalar r = cos(angle1) * radius;
        Scalar height = sin(angle1) * radius;

        for(std::size_t j = 0; j < slices; j++)
        {
          Scalar angle2 = (Scalar)(j * constants::two_pi<Scalar>) / (Scalar)(slices);
          positions.emplace_back(cos(angle2) * r, height, sin(angle2) * r);
          ++k;
        }
      }
    
      positions.emplace_back(0, -radius, 0);
      auto vhandles = m.add_vertices(positions);
    
      for(std::size_t i = 0; i < slices; i++)
      {
        m.add_face(vhandles[0], vhandles[1 + (1 + i) % slices], vhandles[1 + i % slices]);
      
        for(std::size_t j = 0; j < stacks-2; ++j)
        {
          std::size_t a,b,c,d;
          a = 1 + j * slices + i % slices;
          b = 1 + j * slices + (1 + i) % slices;
          c = 1 + (j + 1) * slices + (1 + i) % slices;
          d = 1 + (j + 1) * slices + i % slices;
          m.add_face(vhandles[a], vhandles[b], vhandles[c], vhandles[d]);
        }
        m.add_face(vhandles[1 + slices * (stacks - 1)],
            vhandles[1 + (stacks - 2) * slices + i % slices],
            vhandles[1 + (stacks - 2) * slices + (1 + i) % slices]);
      }
    
      for(auto v: m.vertices())
      {
        for(auto& nml : m.normals(m.incoming_halfedges(v)))
          nml = normalize(m.position(v));
      }

      return m;
    }
  
    template <typename Scalar, typename Scalar2, typename Scalar3>
    mesh<Scalar> create_cylinder(Scalar2 radius, Scalar3 height, std::size_t stacks, std::size_t slices)
    {
      assert(slices >= 3 && stacks >= 1);

      mesh<Scalar> m;
    
      std::size_t n = 2 + slices * (stacks + 1);
      std::vector<vector<Scalar,3> > positions;
      positions.reserve(n);
    
      positions.emplace_back(0, height, 0);

   
      for(std::size_t i = 0; i < stacks + 1; i++)
      {
        Scalar h = (stacks - i) * height / stacks;

        for(std::size_t j = 0; j < slices; j++)
        {
          Scalar angle2 = (Scalar)(j*constants::two_pi<Scalar>)/(Scalar)(slices);
          positions.emplace_back(cos(angle2)*radius, h, sin(angle2)*radius);
        }
      }

      positions.emplace_back(0,0,0);
      auto vhandles = m.add_vertices(positions);

      for(std::size_t i = 0; i < slices; ++i)
      {
        m.add_face(vhandles[0], vhandles[1 + (1 + i) % slices], vhandles[1 + i % slices]);
    
        for(std::size_t j = 0; j < stacks; ++j)
        {
          std::size_t a,b,c,d;
          a = 1 + j * slices + i%slices;
          b = 1 + j * slices + (1 + i) % slices;
          c = 1 + (j + 1) * slices + (1+i) % slices;
          d = 1 + (j + 1) * slices + i % slices;
          m.add_face(vhandles[a], vhandles[b], vhandles[c], vhandles[d]);
        }
        m.add_face(vhandles[vhandles.size() - 1],
        vhandles[1 + stacks * slices + i % slices],
        vhandles[1 + stacks * slices + (1 + i) % slices]);
      }
      return m;
    }
  
    template <typename Scalar>
    mesh<Scalar> create_torus(Scalar r, Scalar R, std::size_t nsides, std::size_t rings)
    {
      assert(nsides >= 3 && rings >= 3);
      mesh<Scalar> m;

      std::size_t n = rings * nsides;
      std::vector<vector<Scalar,3>> positions;
      positions.reserve(n);
      std::size_t k = 0;
      for(std::size_t i = 0; i < rings; ++i)
      {
        Scalar angle1 = (Scalar)(i * constants::two_pi<Scalar> / rings);
        vector<Scalar,3> center(cos(angle1) * R, 0, sin(angle1) * R);
        vector<Scalar,3> t1(cos(angle1), 0, sin(angle1));
        vector<Scalar,3> t2(0, 1, 0);

        for(std::size_t j = 0; j < nsides; ++j)
        {
          Scalar angle2 = (Scalar)(j*constants::two_pi<Scalar>/nsides);
          positions.push_back(center + (Scalar)(sin(angle2) * r) * t1+(Scalar)(cos(angle2) * r) * t2);
        //  m.texcoord(vhandles[k]).set(angle1/(2*3.14159f),angle2/(2*3.14159f)) ;
          k++;
        }
      }
      auto vhandles = m.add_vertices(positions);

      for(std::size_t i = 0; i < rings; ++i)
      {
        for(std::size_t j = 0; j < nsides; ++j)
        {
          std::size_t a,b,c,d;
          a = (i + 1) % rings * nsides + j;
          b = (i + 1) % rings * nsides + (j + 1) % nsides;
          c = i * nsides + (j+1) % nsides;
          d = i * nsides + j;
          m.add_face(vhandles[a], vhandles[b], vhandles[c], vhandles[d]);
        }
      }
      return m;
    }
  
    template <typename Scalar, typename Scalar2>
    mesh<Scalar> create_disk(Scalar2 radius, std::size_t slices)
    {
      mesh<Scalar> m;
      std::vector<vector<Scalar,3>> positions;
      positions.reserve(slices + 1);
      positions.emplace_back(0, 0, 0);
    
      for(std::size_t i = 0; i < slices; ++i)
      {
        Scalar angle= -i * constants::two_pi<Scalar>/slices;
        positions.emplace_back(cos(angle) * radius, 0, sin(angle) * radius);
      }
      auto vhandles = m.add_vertices(positions);
      for(std::size_t i = 0; i < slices; ++i)
      {
        m.add_face(vhandles[0], vhandles[1 + i % slices], vhandles[1 + (1 + i) % slices]);
      }
    
      return m;
    }
  
    template <typename Scalar, typename Scalar2>
    mesh<Scalar> create_octaeder(Scalar2 radius)
    {
      mesh<Scalar> m;
    
      std::vector<vector<Scalar,3>> positions;
      positions.reserve(6);
    
      positions.emplace_back(0, radius, 0);
    
      for(std::size_t i = 0; i < 4; ++i)
        positions.emplace_back((Scalar)cos(i * constants::pi_2<Scalar>) * radius, 0, -(Scalar)sin(i * constants::pi_2<Scalar>) * radius);

      positions.emplace_back(0, -radius, 0);
      auto vhandles = m.add_vertices(positions);

      for(std::size_t i = 0; i < 4; i++)
      {
        m.add_face(vhandles[0], vhandles[i + 1], vhandles[(i + 1) % 4 + 1]);
        m.add_face(vhandles[5], vhandles[(i + 1) % 4 + 1], vhandles[i + 1]);
      }
      return m;
    }
  
  
  
    //pyramid
    // width x, height y, length z
    //width/height/length segment count
  
    //torus
    //ring radius / pipe radius
    //ring /pipe segment count
  
    //cone
    //radius /height
    //radial/height segment count
  
    //plane
     // width, height
     //radial /height segment count
  
   //cone
   // top, bottom  radius , height
  
    //todo capsule
    // radius, height (ydir)
    //radial-,  cap-, height segment count
  
  
  /*
  
   template <typename mesh>
mesh create_triangle()
{
  typedef typename primitive_traits<mesh>::scalar scalar;
  mesh m;
  std::vector<vertex_handle> vhandles = m.add_vertices(3);
  m.position(vhandles[0]).set(0, 0,  0);
  m.position(vhandles[1]).set((scalar)1, 0,  0);
  m.position(vhandles[2]).set((scalar)0.5,(scalar) 1,  0);
  m.add_face(vhandles);
  m.complete();
  compute_missing_normals(m);
  return m;
}

template <typename mesh>
mesh create_quad()
{
  typedef typename primitive_traits<mesh>::scalar scalar;

  mesh m;
  std::vector<vertex_handle> vhandles = m.add_vertices(4);
  m.position(vhandles[0]).set((scalar)-0.5, (scalar)-0.5,  0);
  m.position(vhandles[1]).set((scalar) 0.5, (scalar)-0.5,  0);
  m.position(vhandles[2]).set((scalar) 0.5, (scalar) 0.5,  0);
  m.position(vhandles[3]).set((scalar)-0.5, (scalar) 0.5,  0);
  
  m.add_face(vhandles);
  m.complete();
  compute_missing_normals(m);
  return m;
}



//create a unit arrow
template <typename mesh>
mesh create_unit_arrow(const typename primitive_traits<mesh>::scalar& stem_radius,
             const typename primitive_traits<mesh>::scalar& head_radius,
             const typename primitive_traits<mesh>::scalar& stem_height,
             int slices, int stem_stacks)
{
  typedef typename primitive_traits<mesh>::scalar scalar;
  typedef typename primitive_traits<mesh>::vec3 vec3;
  assert(slices >= 3 && stem_stacks >= 1&&stem_height <= 1 && stem_height >= 0);

  scalar head_height=(scalar)1-stem_height;
  mesh m;
  
  int n  = 2+slices*(stem_stacks+2);
  std::vector<vertex_handle> vhandles = m.add_vertices(n);
  
  scalar height = stem_height+ head_height;
  m.position(vhandles[0]).set(0,height,0);
  
  int k=1;
  for(int j = 0; j < slices; j++)
  {
    scalar angle2 = (scalar)(j*2.0*3.14159)/(scalar)(slices);
    m.position(vhandles[k]).set(cos(angle2)*head_radius,stem_height,sin(angle2)*head_radius);
    k++;
  }

  for(int i = 0; i < stem_stacks+1; i++)
  {
    scalar  h = (stem_stacks-i)*stem_height/(stem_stacks);

    for(int j = 0; j < slices; j++)
    {
      scalar angle2 = (scalar)(j*2.0*3.14159)/(scalar)(slices);
      m.position(vhandles[k]).set(cos(angle2)*stem_radius,h,sin(angle2)*stem_radius);
      k++;
    }
  }
  m.position(vhandles[k]).set(0,0,0);
  
  for(int i = 0; i < slices; i++)
  {
    m.add_triangle(vhandles[0],vhandles[1+(1+i)%slices],vhandles[1+i%slices]);
   
    for(int j = 0; j < stem_stacks+1;j++)
    {
      int a,b,c,d;
      a = 1+j*slices+(i)%slices;
      b = 1+j*slices+(1+i)%slices;
      c = 1+(j+1)*slices+(1+i)%slices;
      d = 1+(j+1)*slices+(i)%slices;
      m.add_quad(vhandles[a],vhandles[b],vhandles[c],vhandles[d]);
    }
    m.add_triangle(vhandles[vhandles.size()-1],
      vhandles[1+(stem_stacks+1)*slices+(i)%slices],
      vhandles[1+(stem_stacks+1)*slices+(1+i)%slices]);
  }
  m.complete();
  compute_missing_normals(m);
  return m;
}

template <typename mesh>
mesh create_saddle(int stacks, int slices, const typename primitive_traits<mesh>::scalar& xmin,
           const typename primitive_traits<mesh>::scalar& xmax,
           const typename primitive_traits<mesh>::scalar& ymin,
           const typename primitive_traits<mesh>::scalar& ymax)
{
  typedef typename primitive_traits<mesh>::scalar scalar;
  typedef typename primitive_traits<mesh>::vec3 vec3;
  assert(slices >= 2 && stacks >= 2);
  mesh m;
  int n  = slices*stacks;
  std::vector<vertex_handle> vhandles = m.add_vertices(n);
  for(int i = 0; i < n; ++i)
  {
    int x = i%slices;
    int y = i/slices;
    scalar xf = (xmax-xmin)*x/(scalar)(slices-1)+xmin;
    scalar yf = (ymax-ymin)*y/(scalar)(stacks-1)+ymin;
    m.position(vhandles[i]).set(xf,xf*xf - yf*yf,yf);
    if(x < slices-1 && y < stacks-1)
      m.add_quad(vhandles[i],vhandles[i+slices],vhandles[i+slices+1],vhandles[i+1]);
  }
  m.complete();
  compute_missing_normals(m);
  return m;

}
  */
  
  }
}
