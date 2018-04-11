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
//#include "owl/utils/progress.hpp"

namespace owl
{
  namespace math
  {
    template <typename Scalar>
    class face_tesselator
    {
  
      mesh<Scalar>& mesh_;
      face_handle face_;
      std::size_t u,v;
    
    public:
      face_tesselator(mesh<Scalar>& m, face_handle face)
        : mesh_{m}
        , face_{face}
      {
        auto nml = mesh_.compute_face_normal(face_);
        std::tie(u, v) = min_abs_components(nml);
      }
    
      inline bool is_convex() const
      {
        if(is_triangle())
          return true;
      
        math::interval<Scalar> zrange;
        for(auto he : halfedges(face_))
        {
          auto d1 = position(target(next(he))) - position(target(he));
          auto d2 = position(target(opposite(he))) - position(target(he));
          Scalar z = d1[u] * d2[v] - d1[v] * d2[u];
          zrange.insert(z);
        }
        return (zrange.lower_bound >= 0 && zrange.upper_bound >= 0);
      }
    
      inline vertex_handle origin(halfedge_handle he) const
      {
        return mesh_.origin(he);
      }
    
      inline vertex_handle target(halfedge_handle he) const
      {
        return mesh_.target(he);
      }
    
      bool is_start(vertex_handle v_prev, vertex_handle v_curr, vertex_handle v_next) const
      {
        return edge_sign(v_prev, v_curr, v_next) > 0 && less_equal(v_curr, v_prev) && less_equal(v_curr, v_next);
       //interior angle < 180
        //neighbors both to the right of v
      }
    
      bool is_split(vertex_handle v_prev, vertex_handle v_curr, vertex_handle v_next) const
      {
      return edge_sign(v_prev, v_curr, v_next) > 0 && less_equal(v_curr, v_prev) && less_equal(v_curr, v_next);
         //interior angle > 180
       //neighbors both to the right of v
      }
    /*
      bool is_end(vertex_handle v_prev, vertex_handle v_curr, vertex_handle v_next) const
      {
        //interior angle < 180
        //neighbors both to the left of v
      }*/
    
     /* bool is_merge(vertex_handle v_prev, vertex_handle v_curr, vertex_handle v_next) const
      {
        //interior angle > 180
       //neighbors both to the left of v
      }*/
  
    /*
      bool is_monoton() const
      {
        for(auto adj_vertices : owl::utils::make_adjacent_range(mesh_.vertices(face_)))
        {
          if(is_split(adj_vertices.prev, adj_vertices.current, adj_vertices.next) ||
             is_merge(adj_vertices.prev, adj_vertices.current, adj_vertices.next))
            return false;
        }
        return true;
      }*/
    
      inline bool is_triangle() const
      {
        return mesh_.is_triangle(face_);
      }
    
      inline const vector3<Scalar>& position(vertex_handle v) const
      {
        return mesh_.position(v);
      }
    
      inline vector2<Scalar> position_2d(vertex_handle v) const
      {
        const auto& pos = position(v);
        return {pos[u], pos[v]};
      }
    
      inline bool less_equal(vertex_handle v_a, vertex_handle v_b) const
      {
        const auto& a = position(v_a);
        const auto& b = position(v_b);
        return (a[u] < b[u]) || (a[u] == b[u] && a[v] <= b[v]);
      }
    
       // +  convex - concave
      inline Scalar edge_sign(vertex_handle a, vertex_handle b, vertex_handle c) const
      {
        assert(less_equal(a, b) && less_equal(b, c));
        const auto& posa = position(a);
        const auto& posb = position(b);
        const auto& posc = position(c);
      
        auto gapL = posb[u] - posa[u];
        auto gapR = posc[u] - posb[u];

        if(gapL + gapR > 0)
          return (posb[v] - posc[v]) * gapL + (posb[v] - posa[v]) * gapR;
      
        return 0;
      }
    
      inline bool edge_goes_left(halfedge_handle he) const
      {
        return less_equal(target(he), origin(he));
      };
    
      inline bool edge_goes_right(halfedge_handle he) const
      {
        return less_equal(origin(he), target(he));
      };
    
      inline halfedge_handle next(halfedge_handle he) const
      {
        return mesh_.next(he);
      }
    
      inline halfedge_handle prev(halfedge_handle he) const
      {
        return mesh_.prev(he);
      }
    
      inline halfedge_handle opposite(halfedge_handle he) const
      {
        return mesh_.opposite(he);
      }
    
      inline halfedge_handle insert_edge(halfedge_handle he_prev, halfedge_handle he_next)
      {
        return mesh_.insert_edge(he_prev, he_next);
      }
    
      inline halfedge_handle halfedge(face_handle f) const
      {
        return mesh_.inner(f);
      }
    
    
      void triangulate_simple_polygon()
      {
      
      }
    
      void triangulate_monoton()
      {
        if(is_triangle())
          return;
  
        halfedge_handle up = halfedge(face_);
        while(less_equal(target(up), origin(up)))
          up = prev(up);
      
        while(less_equal(origin(up), target(up)))
          up = next(up);
      
        halfedge_handle lo = prev(up);
      
        while(next(up) != lo)
        {
          if(less_equal(target(up), origin(lo)))
          {
            while(next(lo) != up && (edge_goes_left( next(lo) )
              || edge_sign( mesh_.origin(lo), target(lo), target(next(lo)) ) <= 0))
              lo = opposite(insert_edge(next(lo), lo));

            lo = prev(lo);
          }
          else
          {
            while(next(lo) != up && (edge_goes_right(prev(up))
             || edge_sign(target(up), origin(up), origin(prev(up))) >= 0))
              up = opposite(insert_edge(up, prev(up)));
        
            up = next(up);
          }
        }

        assert(next(lo) != up);
        while(next(next(lo)) != up)
          lo = opposite(insert_edge(next(lo), lo));
      }
  
      void triangulate_convex()
      {
        assert(is_convex());
      
        if(is_triangle())
          return;
  
        auto he_prev = halfedge(face_);
        auto he_next = next(next(next(he_prev)));
        while(he_next != he_prev)
        {
           insert_edge(he_prev, he_next);
           he_next = next(he_next);
        }
      }
    };
  
    template <typename Scalar>
    class tesselator
    {
  
      mesh<Scalar>& mesh_;
      bool update_normals_;
    public:
      tesselator(mesh<Scalar>& m, bool update_normals = true)
        : mesh_{m}
        , update_normals_{update_normals}
        {}
    
    
      void triangulate_monoton(face_handle f)
      {
        face_tesselator<Scalar> tess(mesh_, f);
        tess.triangulate_monoton();
      }
    
      void triangulate_convex(face_handle f)
      {
        face_tesselator<Scalar> tess(mesh_, f);
        tess.triangulate_convex();
      }
  
      void triangulate_monoton()
      {
//        utils::progress progress(2*mesh_.num_faces() + mesh_.num_halfedges());
  //      progress.make_current(mesh_.num_faces());
        {
    //      utils::progress progress_tri(mesh_.num_faces());
          for(auto f : mesh_.faces())
          {
            triangulate_monoton(f);
      //      progress.step();
          }
        }
       // progress.resign_current();
       // progress.make_current(mesh_.num_faces() + mesh_.num_halfedges());
      
        if(update_normals_)
          mesh_.update_normals();
      
       // progress.resign_current();
      }
    
      void triangulate_convex()
      {
        for(auto f : mesh_.faces())
          triangulate_convex(f);
         if(update_normals_)
          mesh_.update_normals();
      }
    };
  
  template <typename Scalar>
  void triangulate_monoton(mesh<Scalar>& m)
  {
    auto tess = tesselator<Scalar>(m);
    tess.triangulate_monoton();
  }
  
  template <typename Scalar>
  void triangulate_convex(mesh<Scalar>& m)
  {
    auto tess = tesselator<Scalar>(m);
    tess.triangulate_convex();
  }
  }
}
