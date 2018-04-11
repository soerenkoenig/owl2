//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <stack>

#include "owl/utils/handle.hpp"
#include "owl/utils/dynamic_properties.hpp"
#include "owl/utils/count_iterator.hpp"
#include "owl/utils/iterator_range.hpp"
#include "owl/utils/map_iterator.hpp"
#include "owl/utils/filter_iterator.hpp"
#include "owl/utils/adjacent_iterator.hpp"
#include "owl/utils/range_algorithm.hpp"
#include "owl/math/matrix.hpp"
#include "owl/math/angle.hpp"
#include "owl/math/interval.hpp"
#include "owl/math/constants.hpp"
#include "owl/color/color.hpp"
#include "owl/math/line_segment.hpp"
//#include "owl/utils/progress.hpp"

namespace owl
{
  namespace math
  {
  
    struct vertex_tag{};
    using vertex_handle = owl::utils::handle<vertex_tag>;
  
    struct halfedge_tag{};
    using halfedge_handle = owl::utils::handle<halfedge_tag>;
  
    struct edge_tag{};
    using edge_handle = owl::utils::handle<edge_tag>;
  
    struct face_tag{};
    using face_handle = owl::utils::handle<face_tag>;
 
  
    template <typename T>
    using mesh_property_handle = owl::utils::property_handle<T>;
  
    template <typename T>
    using vertex_property_handle = owl::utils::indexed_property_handle<T,vertex_tag>;
    template <typename T>
    using edge_property_handle = owl::utils::indexed_property_handle<T,edge_tag>;
    template <typename T>
    using halfedge_property_handle = owl::utils::indexed_property_handle<T,halfedge_tag>;
    template <typename T>
    using face_property_handle = owl::utils::indexed_property_handle<T,face_tag>;
  

    class status_flags
    {
    public:
      status_flags()
        : bits_(0)
      {}
    
      bool is_removed() const
      {
        return bits_.test(0);
      }
  
      void remove()
      {
        bits_.set(0, true);
      }
    
      void restore()
      {
        bits_.set(0, false);
      }
    
      bool is_selected() const
      {
        return bits_.test(1);
      }
  
      void select()
      {
        bits_.set(1, true);
      }
  
      void deselect()
      {
        bits_.set(1, false);
      }
  
      void invert_selection()
      {
        if(is_selected())
          deselect();
        else
          select();
      }
    
    private:
      std::bitset<8> bits_;
    };
  
    template <typename Scalar>
    class mesh
    {
    public:
      using scalar = Scalar;
      using angle = angle<scalar>;
    
      template <std::size_t Dim>
      using vector = vector<Scalar, Dim>;
      using vector3 = vector<3>;
      using vector2 = vector<2>;
      using box = box<Scalar>;
      using color_t = color::rgba8u;
    
      template <typename Range>
      using is_vertex_handle_range = std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, vertex_handle>;
    
      template <typename Range>
      using is_halfedge_handle_range = std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, halfedge_handle>;
    
      template <typename Range>
      using is_edge_handle_range = std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, edge_handle>;
    
      template <typename Range>
      using is_face_handle_range = std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, face_handle>;
    
      template <typename Range, std::size_t N = 3>
      using is_vector_range = std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, vector<N>>;
    
      mesh()
      {
        add_property(vertex_position_handle_, "vertex_position");
        add_property(face_normal_handle_, "face_normal");
        add_property(face_color_handle_, "face_color");
        add_property(halfedge_normal_handle_, "halfedge_normal");
        add_property(halfedge_texcoord_handle_, "halfedge_texcoord");
      }
    
      auto faces() const
      {
        return utils::make_counting_range(face_handle(0), face_handle(num_faces()));
      }

      auto vertices() const
      {
       return utils::make_counting_range(vertex_handle(0), vertex_handle(num_vertices()));
      }

      auto edges() const
      {
       return make_counting_range(edge_handle(0), edge_handle(num_edges()));
      }

      auto halfedges() const
      {
       return make_counting_range(halfedge_handle(0), halfedge_handle(num_halfedges()));
      }
    
      auto vertices(face_handle f, vertex_handle v_start) const
      {
        auto step = [this](halfedge_handle he)
        {
          return next(he);
        };

        auto deref = [this](halfedge_handle he)
        {
          return target(he);
        };
      
        halfedge_handle he = inner(f);
        while(target(he) != v_start)
          he = next(he);
      
        return make_handle_circulator_range(he, step, deref);
      }

      auto vertices(face_handle f) const
      {
        return vertices(f, target(inner(f)));
      }

      auto vertices(vertex_handle v) const
      {
        auto step = [this](halfedge_handle he)
        {
          return next_incoming(he);
        };

        auto deref = [this](halfedge_handle he)
        {
          return target(he);
        };
      
        return make_handle_range(incoming(v), step, deref);
      }
    
      auto incoming_halfedges(vertex_handle v) const
      {
        return make_handle_circulator_range(incoming(v),
          [this](halfedge_handle he)
          {
            return next_incoming(he);
          });
      }

      auto outgoing_halfedges(vertex_handle v) const
      {
        return make_handle_circulator_range(outgoing(v),
          [this](halfedge_handle he)
          {
            return next_outgoing(he);
          });
      }

      auto edges(face_handle f) const
      {
        auto step = [this](halfedge_handle he)
        {
          return next(he);
        };
     
        auto deref = [this](halfedge_handle he)
        {
          return edge(he);
        };
      
        return make_handle_circulator_range(inner(f), step, deref);
      }
    
      auto inner_halfedges(face_handle f) const
      {
        return inner_halfedges(f, inner(f));
      }
    
      auto inner_halfedges(face_handle f, halfedge_handle he_start) const
      {
        halfedge_handle he = inner(f);
        while(he != he_start)
          he = next(he);
      
        return make_handle_circulator_range(he,
          [this](halfedge_handle he)
          {
            return next(he);
          });
      }
    
      auto halfedges(halfedge_handle he) const
      {
        return make_handle_circulator_range(he,
          [this](halfedge_handle he)
          {
            return next(he);
          });
      }
    
      auto outer_halfedges(face_handle f) const
      {
        auto step = [this](halfedge_handle he)
        {
          return next(he);
        };
     
        auto deref = [this](halfedge_handle he)
        {
          return opposite(he);
        };

        return make_handle_circulator_range(inner(f), step, deref);
      }
    
      auto faces(face_handle f) const
      {
        auto step = [this](halfedge_handle he)
        {
          return next(he);
        };
     
        auto deref = [this](halfedge_handle he)
        {
          return face(opposite(he));
        };
      
        return utils::filter([](face_handle f){ return f.is_valid(); },
            make_handle_circulator_range(inner(f), step, deref));
      }

      const vector3& position(vertex_handle v) const
      {
        return vertex_properties_[vertex_position_handle_][v.index()];
      }

      vector3& position(vertex_handle v)
      {
        return vertex_properties_[vertex_position_handle_][v.index()];
      }

      const vector2& texcoord(halfedge_handle he) const
      {
        return halfedge_properties_[halfedge_texcoord_handle_][he.index()];
      }

      vector2& texcoord(halfedge_handle he)
      {
        return halfedge_properties_[halfedge_texcoord_handle_][he.index()];
      }

      const vector3& normal(face_handle f) const
      {
        return face_properties_[face_normal_handle_][f.index()];
      }
    
      vector3& normal(face_handle f)
      {
        return face_properties_[face_normal_handle_][f.index()];
      }
    
      const color_t& color(face_handle f) const
      {
        return face_properties_[face_color_handle_][f.index()];
      }
    
      color_t& color(face_handle f)
      {
        return face_properties_[face_color_handle_][f.index()];
      }
    
      const vector3& normal(halfedge_handle he) const
      {
        return halfedge_properties_[halfedge_normal_handle_][he.index()];
      }
    
      vector3& normal(halfedge_handle he)
      {
        return halfedge_properties_[halfedge_normal_handle_][he.index()];
      }

      template<typename Handle>
      status_flags& status(Handle h)
      {
        return operator[](h);
      }
    
      const vertex_handle& target(halfedge_handle he) const
      {
        return operator[](he).target;
      }
    
      vertex_handle& target(halfedge_handle he)
      {
        return operator[](he).target;
      }
    
      const vertex_handle& origin(halfedge_handle he) const
      {
        return operator[](opposite(he)).target;
      }
    
      vertex_handle& origin(halfedge_handle he)
      {
        return operator[](opposite(he)).target;
      }
    
      const vertex_handle& target(edge_handle e) const
      {
        return operator[](halfedge(e)).target;
      }
    
      vertex_handle& target(edge_handle e)
      {
        return operator[](halfedge(e)).target;
      }
    
      const vertex_handle& origin(edge_handle e) const
      {
        return operator[](opposite(halfedge(e))).target;
      }
    
      vertex_handle& origin(edge_handle e)
      {
        return operator[](opposite(halfedge(e))).target;
      }
    
      template<typename Handle>
      void select(Handle h)
      {
        return status(h).select();
      }
    
      template<typename Handle>
      void deselect(Handle h)
      {
        return status(h).deselect();
      }
    
      template<typename Handle>
      void invert_selection(Handle h)
      {
        return status(h).invert_selection();
      }
    
      template<typename Handle>
      bool is_selected(Handle h) const
      {
        return status(h).is_selected();
      }
    
      template <typename VertexRange, typename =
        std::enable_if_t<is_vertex_handle_range<VertexRange>::value>>
      auto positions(VertexRange&& vertices) const
      {
        return utils::map_range([this](vertex_handle v)->const auto&{ return position(v); },
          std::forward<VertexRange>(vertices));
      }

      template <typename VertexRange, typename =
        std::enable_if_t<is_vertex_handle_range<VertexRange>::value>>
      auto positions(VertexRange&& vertices)
      {
        return utils::map_range([this](vertex_handle v)->auto&{ return position(v); },
          std::forward<VertexRange>(vertices));
      }

      template <typename FaceHandleRange, typename = std::enable_if_t<is_face_handle_range<FaceHandleRange>::value>>
      auto normals(FaceHandleRange&& faces) const
      {
        return utils::map_range([this](face_handle f)->const auto&{ return normal(f);},
          std::forward<FaceHandleRange>(faces));
      }

      template <typename FaceHandleRange, typename = std::enable_if_t<is_face_handle_range<FaceHandleRange>::value>>
      auto normals(FaceHandleRange&& faces)
      {
        return utils::map_range([this](face_handle f)->auto&{ return normal(f);},
          std::forward<FaceHandleRange>(faces));
      }

      template <typename HalfEdgeHandleRange, typename = std::enable_if_t<is_halfedge_handle_range<HalfEdgeHandleRange>::value>,
        typename = void, typename = void>
      auto normals(HalfEdgeHandleRange&& halfedges) const
      {
        return utils::map_range([this](halfedge_handle he)->const auto&{ return normal(he);},
          std::forward<HalfEdgeHandleRange>(halfedges));
      }
    
      template <typename HalfEdgeHandleRange, typename = std::enable_if_t<is_halfedge_handle_range<HalfEdgeHandleRange>::value>,
        typename = void, typename = void>
      auto normals(HalfEdgeHandleRange&& halfedges)
      {
        return utils::map_range([this](halfedge_handle he)->auto&{ return normal(he);},
          std::forward<HalfEdgeHandleRange>(halfedges));
      }
    
      template <typename HalfEdgeHandleRange, typename = std::enable_if_t<is_halfedge_handle_range<HalfEdgeHandleRange>::value>>
      auto texcoords(HalfEdgeHandleRange&& halfedges) const
      {
        return utils::map_range([this](halfedge_handle he)->const auto&{ return texcoord(he);},
          std::forward<HalfEdgeHandleRange>(halfedges));
      }

      template <typename HalfEdgeHandleRange, typename = std::enable_if_t<is_halfedge_handle_range<HalfEdgeHandleRange>::value>,
        typename = void>
      auto texcoords(HalfEdgeHandleRange&& halfedges)
      {
        return utils::map_range([this](halfedge_handle he) -> auto&{ return texcoord(he);},
          std::forward<HalfEdgeHandleRange>(halfedges));
      }
    
      std::size_t num_vertices() const
      {
        return vertices_.size();
      }
    
      std::size_t num_edges() const
      {
        return edges_.size();
      }

      std::size_t num_halfedges() const
      {
        return 2 * num_edges();
      }
    
      std::size_t num_faces() const
      {
        return faces_.size();
      }
    
      std::size_t num_n_gons(std::size_t n) const
      {
        return utils::count_if(faces(),
          [this,n](face_handle f)
          {
            return is_n_gon(f, n);
          });
      }
    
      std::size_t num_triangles() const
      {
        return num_n_gons(3);
      }
    
      std::size_t num_quads() const
      {
        return num_n_gons(4);
      }
    
      bool is_non_manifold(vertex_handle v) const
      {
        std::size_t n = 0;
        for(auto he : incoming_halfedges(v))
          if(is_boundary(he))
          {
            if(++n > 1)
              return true;
          }
        return false;
      }

      bool is_boundary(vertex_handle v) const
      {
        auto he = incoming(v);
        return !(he.is_valid() && face(he).is_valid());
      }

      bool is_boundary(halfedge_handle he) const
      {
        return !face(he).is_valid();
      }

      bool is_boundary(edge_handle e) const
      {
        auto hes = halfedges(e);
        return is_boundary(hes[0]) || is_boundary(hes[1]);
      }
 
      bool is_boundary(face_handle f, bool check_vertices = false) const
      {
        if(check_vertices)
        {
          for(auto v : vertices(f))
            if(is_boundary(v))
              return true;
          return false;
        }
        for(auto he : outer_halfedges(f))
          if(is_boundary(he))
            return true;
        return false;
      }
    
      bool is_isolated(vertex_handle v) const
      {
        return !incoming(v).is_valid();
      }
  
      const status_flags& status(vertex_handle v) const
      {
        return vertices_[v.index()].status;
      }
    
      status_flags& status(vertex_handle v)
      {
        return vertices_[v.index()].status;
      }
    
      const status_flags& status(face_handle f) const
      {
        return faces_[f.index()].status;
      }
    
      status_flags& status(face_handle f)
      {
        return faces_[f.index()].status;
      }
    
      const status_flags& status(edge_handle e) const
      {
        return edges_[e.index()].status;
      }
    
      status_flags& status(edge_handle e)
      {
        return edges_[e.index()].status;
      }
    
      bool is_sharp(edge_handle e, const angle& max_angle = degrees<scalar>(44)) const
      {
        return is_sharp(halfedge(e), max_angle);
      }
  
      bool is_sharp(halfedge_handle he, const angle& max_angle = degrees<scalar>(44)) const
      {
        auto angle = std::abs(dihedral_angle(he));
        return angle >= max_angle;
      }
    
      vector3 direction(halfedge_handle he) const
      {
        return position(target(he)) - position(origin(he));
      }
    
      scalar length(halfedge_handle he) const
      {
        return direction(he).length();
      }
    
      scalar length(edge_handle e) const
      {
        return length(halfedge(e));
      }
    
      edge_handle split(edge_handle e, const vector3& position)
      {
         return edge(split(halfedge(e), position));
      }
    
      edge_handle split(edge_handle e, vertex_handle v)
      {
        return edge(split(halfedge(e), v));
      }
    
      halfedge_handle split(halfedge_handle he, const vector3& position)
      {
        auto v = add_vertex(position);
        return split(he, v);
      }
    
      //returns first halfedge of new edge pointing to new vertices
      halfedge_handle split(halfedge_handle he, vertex_handle v)
      {
        auto he_opp = opposite(he);
        auto he_opp_prev = prev(he_opp);
        auto vold = target(he);
      
        auto e = add_edge(vold, v);
        auto he_new = halfedge(e);
        auto he_new_opp = opposite(he_new);
      
         if(incoming(vold) == he)
          incoming(vold) = he_new_opp;
      
        next(he_opp_prev) = he_new;
        next(he_new_opp) = next(he);
        next(he) = he_new_opp;
        next(he_new) = he_opp;
      
        target(he) = v;
        face(he_new) = face(he_opp);
        face(he_new_opp) = face(he);
        if(!face(he_new).is_valid())
         incoming(v) = he_new;
        else
          incoming(v) = he;
        
        return he_new;
      }
    
      auto split_edges()
      {
        auto first = edge_handle{num_edges()};
        for(auto e : edges())
        {
          auto pos = centroid(e);
          split(e, pos);
        }
        return make_counting_range(first, edge_handle{num_edges()});
      }
    
      void reserve_vertices(std::size_t n)
      {
        vertices_.reserve(n);
        vertex_properties_.reserve(n);
      }
    
      void reserve_edges(std::size_t n)
      {
        edges_.reserve(n);
        edge_properties_.reserve(n);
        halfedge_properties_.reserve(2 * n);
      }
    
      void reserve_faces(std::size_t n)
      {
        faces_.reserve(n);
        face_properties_.reserve(n);
      }
    
      void subdivide_quad_split()
      {
        assert(is_quad_mesh());
        reserve_vertices(num_vertices() + num_faces());
        reserve_edges(2 * num_edges() + 4 * num_faces());
        reserve_faces(4 * num_faces());
      
        std::size_t num_vertices_old = vertices_.size();
        split_edges();
      
        auto is_old_vertex = [&](vertex_handle v)
          {
            return v.index() < num_vertices_old;
          };
      
        for(auto f : faces())
        {
          auto he_prev = inner(f);
          if(is_old_vertex(target(he_prev)))
            he_prev = next(he_prev);
        
          auto he_next = next(next(next(he_prev)));
          auto e = insert_edge(he_prev, he_next);
          auto v = target(split_edge(e, centroid(f)));
          he_prev = next(he_prev);
          he_next = next(next(he_next));
          while(he_next != he_prev)
          {
            insert(edge(he_prev,he_next));
            he_next = next(next(he_next));
          }
          insert_edge(he_prev,he_next);
        }
      }
    
      void subdivide_triangle_split()
      {
        assert(is_triangle_mesh());
      
        reserve_vertices(num_vertices() + num_edges());
        reserve_faces(4 * num_faces());
        reserve_edges(2 * num_edges() + 3 * num_faces());
  
        std::size_t num_vertices_old = vertices_.size();
        split_edges();
      
        auto is_old_vertex = [&](vertex_handle v)
          {
            return v.index() < num_vertices_old;
          };
      
        for(auto f : faces())
        {
          auto he_prev = inner(f);
          if(is_old_vertex(target(he_prev)))
            he_prev = next(he_prev);
          
          auto he_next = next(next(next(he_prev)));
          insert_edge(he_prev, he_next);
          he_prev = next(he_prev);
          he_next = next(next(he_next));
          insert_edge(he_prev, he_next);
          he_prev = next(he_prev);
          he_next = next(next(he_next));
          insert_edge(he_prev, he_next);
        }
      }
    
      face_handle create_face(halfedge_handle he)
      {
        faces_.emplace_back(he);
        return face_properties_.add_elem();
      }
    
    
      // inserts an edge between target(he_prev) and origin(he_next).
      // returns the halfedge containing the new face
      //assumes he and he_next belong to the same face
      halfedge_handle insert_edge(halfedge_handle he_prev, halfedge_handle he_next)
      {
        auto e = add_edge(target(he_prev), origin(he_next));
        auto he = halfedge(e);
        auto he_opp = opposite(he);
        auto fold =  face(he_prev);
        auto he_next_prev = prev(he_next);
        face(he_opp) = face(he_prev);
        inner(fold) = he_opp;
        next(he_opp) = next(he_prev);
        next(he_prev) = he;
        next(he) = he_next;
        next(he_next_prev) = he_opp;
      
        auto f_new = create_face(he);
        face(he) = f_new;
        auto he2 = next(he);
        while(he2 != he)
        {
          face(he2) = f_new;
          he2 = next(he2);
        }
      
        return he;
      }
    
      void split(face_handle f, const vector3& pos)
      {
        split(f, add_vertex(pos));
      }
    
      vector3 centroid(halfedge_handle he) const
      {
        return (position(target(he)) + position(origin(he)))/2;
      }
    
      vector3 centroid(edge_handle e) const
      {
        return centroid(halfedge(e));
      }
    
      vector3 centroid(face_handle f) const
      {
        auto points = positions(vertices(f));
        vector3 mp = vector3::zero();
        std::size_t n = 0;
        for(auto p : points)
        {
          mp += p;
          ++n;
        }
        return mp / n;
      }
    
      void split(face_handle f, vertex_handle v)
      {
        auto hes = halfedges(f);
        std::vector<halfedge_handle> hes_old(hes.begin(), hes.end());
        std::vector<edge_handle> edges_new;
        for(auto he : hes_old)
        {
          split(he, centroid(he));
          auto e = add_edge(target(he), v);
          edges_new.push_back(e);
          next(opposite(halfedge(e))) = next(he);
          next(he) = halfedge(e);
        }
      
        for(auto e : utils::make_adjacent_range(edges_new))
        {
          auto he2 = halfedge(e.current);
          if(f == faces_.size())
            create_face(he2);
          else
            inner(f) = he2;
          auto he_start = he2;
          next(he2) = opposite(halfedge(e.prev));
          do
          {
            face(he2) = f;
            he2 = next(he2);
          }
          while(he2 != he_start);
        
          f = faces_.size();
        
        }
        incoming(v) = halfedge(edges_new.front());
      }
    
      angle sector_angle(halfedge_handle he) const
      {
        auto v0 = direction(next(he));
        auto v1 = direction(opposite(he));
        auto denom = v0.length() * v1.length();
        
        if(denom == scalar(0))
          return 0;
    
        scalar cos_a = dot(v0 , v1) / denom;
        cos_a = std::clamp(cos_a, -1, 1);
        if(is_boundary(he))
        {
          vector3 f_n(compute_loop_normal(opposite(he)));
          scalar sign_a = dot(cross(v0, v1), f_n);
          return radians<Scalar>(sign_a >= 0 ? acos(cos_a) : -acos(cos_a));
        }
        else
          return radians<Scalar>(acos(cos_a));
      }

      angle dihedral_angle(halfedge_handle he) const
      {
        return dihedral_angle(edge(he));
      }
    
      angle dihedral_angle(edge_handle e) const
      {
        if(is_boundary(e))
          return radians<scalar>(0);

        vector3 n0, n1;
        halfedge_handle he = halfedge(e);
        n0 = compute_sector_normal(he);
        n1 = compute_sector_normal(opposite(he));
        auto he_dir = direction(he);
        scalar denom = n0.length() * n1.length();
        if(denom == 0)
          return radians<scalar>(0);
        scalar da_cos = dot(n0, n1) / denom;
        da_cos = std::clamp(da_cos, scalar{-1}, scalar{1});
        scalar da_sin_sign = dot(cross(n0, n1), he_dir);
        return radians<scalar>(da_sin_sign >= 0 ? acos(da_cos) : -acos(da_cos));
      }
    
      vector3 compute_sector_normal(halfedge_handle he, bool normalize = true) const
      {
        auto nml = cross(direction(next(he)), direction(opposite(he)));
        if(normalize)
          nml.normalize();
        return nml;
      }
    
      vector3 compute_loop_normal(halfedge_handle he, bool normalize = true) const
      {
        auto nml = vector3::zero();
      
        for(auto he : halfedges(he))
          nml += compute_sector_normal(he, false);
      
        if(normalize)
         nml.normalize();
      
        return nml;
      }
    
      vector3 compute_face_normal(face_handle f) const
      {
        return compute_loop_normal(inner(f));
      }
    
      vector3 compute_vertex_normal(vertex_handle v) const
      {
        auto nml = vector3::zero();
        if(v.index() == 217)
          return nml;
        for(auto he : incoming_halfedges(v))
          nml += compute_sector_normal(he, false);
      
        nml.normalize();
        return nml;
      }
    
      void update_face_normals()
      {
//        utils::progress progress(num_faces());
        for(auto f : faces())
        {
          normal(f) = compute_face_normal(f);
       //   progress.step();
        }
      }
    
      void update_halfedge_normals(const angle& max_angle = degrees<scalar>(44))
      {
      //  utils::progress progress(num_halfedges());
        for(auto he : halfedges())
        {
          normal(he) = is_sharp(he, max_angle) ? compute_loop_normal(he) : compute_vertex_normal(target(he));
       //   progress.step();
        }
      }
    
      void update_normals(const angle& max_angle = degrees<scalar>(44))
      {
   //     utils::progress progress(num_faces() + num_halfedges());
      //  progress.make_current(num_faces());
        update_face_normals();
    //    progress.resign_current();
     //   progress.make_current(num_halfedges());
        update_halfedge_normals(max_angle);
     //   progress.resign_current();
      }
    
      template <typename Handle>
      std::size_t valence(Handle h) const
      {
        return std::size(vertices(h));
      }

      bool is_n_gon_mesh(std::size_t n) const
      {
        return utils::all_of(faces(), [this,&n](face_handle f){ return is_n_gon(f, n); });
      }

      bool is_quad_mesh() const
      {
        return is_n_gon_mesh(4);
      }

      bool is_triangle_mesh() const
      {
        return is_n_gon_mesh(3);
      }
    
      bool is_n_gon(face_handle f, std::size_t n) const
      {
        return valence(f) == n;
      }
    
      bool is_triangle(face_handle f) const
      {
        auto  he = inner(f);
        return he == next(next(next(he)));
      }
    
      bool is_quad(face_handle f) const
      {
        auto  he = inner(f);
        return he == next(next(next(next(he))));
      }
    
      box bounds() const
      {
        return math::bounds(positions(vertices()));
      }
    
      void clear()
      {
        faces_.clear();
        face_properties_.clear();
        edges_.clear();
        edge_properties_.clear();
        halfedge_properties_.clear();
        vertices_.clear();
        vertex_properties_.clear();
      }
    
      bool empty() const
      {
        return faces_.empty() && vertices_.empty() && edges_.empty();
      }
    
      auto add_vertex()
      {
        vertices_.emplace_back();
        return vertex_properties_.add_elem();
      }
    
      auto add_vertex(const vector3& pos)
      {
        auto v = add_vertex();
        position(v) = pos;
        return v;
      }
    
      auto add_vertices(std::size_t n)
      {
        vertices_.resize(num_vertices() + n);
        return vertex_properties_.add_elems(n);
      }
    
      template <typename VectorRange, typename = std::enable_if_t<is_vector_range<VectorRange>::value>>
      auto add_vertices(VectorRange&& points)
      {
        auto n = std::size(points);
        vertices_.resize(num_vertices() + n);
        auto verts = vertex_properties_.add_elems(n);
        owl::utils::copy(points, positions(verts).begin());
        return verts;
      }
    
      template <typename... VertexHandles, typename = std::enable_if_t<(... && std::is_same<std::decay_t<VertexHandles>,vertex_handle>::value)>>
      face_handle add_face(VertexHandles&&... vertices)
      {
         return add_face(std::array<vertex_handle, sizeof...(vertices)> {{ std::forward<VertexHandles>(vertices)... }});
      }
  
    
      //ensures the first vertex of returned face is the vertices.front()
      //adding a face which results in a non-manifold vertex is not allowed
      template <typename VertexHandleRange, typename = std::enable_if_t<is_vertex_handle_range<VertexHandleRange>::value>>
      face_handle add_face(VertexHandleRange&& vertices)
      {
        if(std::size(vertices) < 3)
        {
          std::cout << "not enough vertices" << std::endl;
          return face_handle::invalid();
        }
        if(utils::any_of(vertices, [this](const vertex_handle& v){ return !is_boundary(v); }))
        {
          for(auto v: vertices)
            std::cout << v << " " <<std::boolalpha <<is_boundary(v) <<std::endl;
          std::cout << "new face will produce a complex vertex" << std::endl;
          return face_handle::invalid();
        }
      
        face_handle f = face_handle{faces_.size()};
      
      
        std::vector<halfedge_handle> hes;
        hes.reserve(std::size(vertices));
        std::size_t num_edges_old = num_edges();
        for(auto v : owl::utils::make_adjacent_range(vertices))
        {
          auto he = find_halfedge(v.prev, v.current);
          if (!he.is_valid())
            he = halfedge(add_edge(v.prev, v.current));
          else
          {
            if(!is_boundary(he))
            {
              edges_.resize(num_edges_old);
              edge_properties_.resize(num_edges_old);
              halfedge_properties_.resize(num_edges_old * 2);
              return face_handle::invalid();
            }
          }
          face(he) = f;
          hes.push_back(he);
        }
  
        create_face(hes.back());
      
        for(auto he : owl::utils::make_adjacent_range(hes))
        {
          auto v = target(he.current);
        
          if(is_isolated(v))
          {
            next(he.current) = he.next;
            auto temp = opposite(he.next);
            next(temp) = opposite(he.current);
            incoming(v) = temp;
            continue;
          }
        
          if(next(he.current) == he.next)
          {
            if(incoming(v) == he.current)
              adjust_incoming(v);
           continue;
          }
        
          if(next(he.current).is_valid())
          {
            if(next(opposite(he.next)).is_valid())
            {
              assert(next(he.current) != he.next);
              auto a = next(he.current);
              auto b = prev_circ(he.next);
              auto he_gap = opposite(he.next);
              while(!is_boundary(he_gap))
                he_gap = next_incoming(he_gap);
              next(b) = next(he_gap);
              next(he_gap) = a;
              next(he.current) = he.next;
              adjust_incoming(v);
            }
            else
            {
              auto temp = opposite(he.next);
              next(temp) = next(he.current);
              next(he.current) = he.next;
              incoming(v) = temp;
            }
          }
          else //next(he.current) invalid
          {
            if(next(opposite(he.next)).is_valid())
            {
              auto b = prev_circ(he.next);
              next(b) = opposite(he.current);
              next(he.current) = he.next;
              adjust_incoming(v);
            }
            else //both invalid
            {
              auto he_gap = incoming(v);
              assert(is_boundary(he_gap));
              next(opposite(he.next)) = next(he_gap);
              next(he_gap) = opposite(he.current);
              next(he.current) = he.next;
              adjust_incoming(v);
            }
          }
        }
  
        return f;
      }
    
      bool is_flipable(edge_handle e) const
      {
        if (is_boundary(e))
          return false;
      
        auto [he1, he2] = halfedges(e);
        if(!is_triangle(face(he1)) || !is_triangle(face(he2)))
          return false;
      
        auto v1 = target(next(he1));
        auto v2 = target(next(he2));
     
        auto one_ring_v1 = vertices(v1);
        return utils::find(one_ring_v1,v2) != std::end(one_ring_v1);
      }
    
      void flip_edge(edge_handle e)
      {
        auto [he1, he2] = halfedges(e);
        auto f1 = face(he1);
        auto f2 = face(he2);
        assert(f1.is_valid() && is_triangle(f1));
        assert(f2.is_valid() && is_triangle(f2));
        auto n1 = next(he1);
        auto p1 = next(n1);
        auto n2 = next(he2);
        auto p2 = next(n2);
        auto v1 = target(n1);
        auto v2 = target(n2);
        auto v3 = target(he1);
        auto v4 = target(he2);
      
        next(p1) = n2;
        next(p2) = n1;
        next(n2) = he2;
        next(n1) = he1;
        next(he1) = p2;
        next(he2) = p1;
      
        face(p2) = f1;
        face(p1) = f2;
        if(inner(f1) == p1)
          inner(f1) = he1;
        if(inner(f2) == p2)
          inner(f2) = he2;
        target(he1) = v1;
        target(he2) = v2;
        if(incoming(v3) == he1)
          incoming(v3) = p2;
        if(incoming(v4) == he2)
          incoming(v4) = p1;
      }
    
    
      /*//mark vertex v and all incident faces and edges as removed
      void remove(vertex_handle v, bool remove_isolated_vertices)
      {
        std::vector<face_handle> incident_faces;
      
        for(auto he : incoming(v))
          if(face(he).is_valid())
            incident_faces.push_back(face(he));
        for(auto f: incident_faces)
          remove(f, remove_isolated_vertices);
        status(v).remove();
      }
    
      //mark
      void remove(edge_handle e)
      {
        status(e).remove();
        auto [he1, he2] = halfedges(e);
      
      }
    
      void remove(face_handle f)
      {
        status(f).remove();
        for(auto he : inner_halfedges(f))
          face(he).invalidate();
      }
    */
    
    
      void remove_isolated_vertices()
      {
        for(auto v: vertices())
          if(is_isolated(v) && !is_removed(v))
            remove(v);
      }
    
      void erase_removed_vertices()
      {
        auto verts = vertices();
        auto last = verts.end();
        auto first = utils::find_if(verts, [this](vertex_handle v) { return status(v).removed(); });
        if(first != last)
          for(auto i = first; ++i != last; )
            if (!status(*i).removed())
              move(*i, *first++);
      }
    
      template <typename TexCoordRange, typename = std::enable_if_t<is_vector_range<TexCoordRange,2>::value>>
      void set_face_texcoords(face_handle f, TexCoordRange&& texcoords)
      {
        owl::utils::copy(texcoords,this->texcoords(inner_halfedges(f)).begin());
      }
    
      template <typename NormalRange, typename = std::enable_if_t<is_vector_range<NormalRange,3>::value>>
      void set_face_normals(face_handle f, NormalRange&& normals)
      {
        owl::utils::copy(normals, this->normals(halfedges(f)).begin());
      }
    
      //ensure halfedge of v is a boundary halfedge if v is on boundary
      void adjust_incoming(vertex_handle v)
      {
        if(is_boundary(incoming(v)))
          return;
        for(auto he : incoming_halfedges(v))
        {
          if(is_boundary(he))
          {
            incoming(v) = he;
            break;
          }
        }
      }
    
      template <typename T>
      void add_property(vertex_property_handle<T>& ph, const std::string& name = "")
      {
        return vertex_properties_.add_property(ph, name);
      }
    
      template <typename T>
      void add_property(edge_property_handle<T>& ph, const std::string& name = "")
      {
        return edge_properties_.add_property(ph, name);
      }
    
      template <typename T>
      void add_property(halfedge_property_handle<T>& ph, const std::string& name = "")
      {
        return halfedge_properties_.add_property(ph, name);
      }
    
      template <typename T>
      void add_property(face_property_handle<T>& ph, const std::string& name = "")
      {
        face_properties_.add_property(ph, name);
      }
    
      template <typename T>
      void add_property(mesh_property_handle<T>& ph, const std::string& name = "")
      {
        mesh_properties_.add_property(ph, name);
      }
    
      template <typename T>
      void remove_property(vertex_property_handle<T>& ph)
      {
        vertex_properties_.remove_property(ph);
      }
    
      template <typename T>
      void remove_property(edge_property_handle<T>& ph)
      {
        edge_properties_.remove_property(ph);
      }
    
      template <typename T>
      void remove_property(halfedge_property_handle<T>& ph)
      {
        halfedge_properties_.remove_property(ph);
      }
    
      template <typename T>
      void remove_property(face_property_handle<T>& ph)
      {
        face_properties_.remove_property(ph);
      }
    
      template <typename T>
      void remove_property(mesh_property_handle<T>& ph)
      {
        mesh_properties_.remove_property(ph);
      }
    
      template <typename T>
      bool has_vertex_property(const std::string& name = "") const
      {
        return vertex_properties_.has_property<T>(name);
      }
    
      template <typename T>
      bool has_edge_property(const std::string& name = "") const
      {
        return edge_properties_.has_property(name);
      }
    
      template <typename T>
      bool has_halfedge_property(const std::string& name = "") const
      {
        return halfedge_properties_.has_property(name);
      }
    
      template <typename T>
      bool has_face_property(const std::string& name = "") const
      {
        return face_properties_.has_property(name);
      }
    
      template <typename T>
      void has_mesh_property(const std::string& name = "") const
      {
         return mesh_properties_.get_property(name);
      }
    
      const face_handle& face(halfedge_handle he) const
      {
        return operator[](he).face;
      }
    
      halfedge_handle opposite(halfedge_handle he) const
      {
        return he.index() % 2  == 0 ? halfedge_handle{he.index() + 1} : halfedge_handle{he.index() - 1};
      }
    
      const halfedge_handle& inner(face_handle f) const
      {
        return operator[](f).halfedge;
      }
    
      face_handle& face(halfedge_handle he)
      {
        return operator[](he).face;
      }
    
      halfedge_handle& inner(face_handle f)
      {
        return operator[](f).halfedge;
      }
    
      halfedge_handle& incoming(vertex_handle v)
      {
        return operator[](v).incoming;
      }
    
      halfedge_handle& next(halfedge_handle he)
      {
        return operator[](he).next;
      }
    
      halfedge_handle outer(face_handle f) const
      {
        return opposite(inner(f));
      }
    
      const halfedge_handle& next(halfedge_handle he) const
      {
        return operator[](he).next;
      }
    
      halfedge_handle next_incoming(halfedge_handle he) const
      {
        return opposite(next(he));
      }
    
      halfedge_handle prev_incoming(halfedge_handle he) const
      {
        return prev(opposite(he));
      }
    
      halfedge_handle next_outgoing(halfedge_handle he) const
      {
        return next(opposite(he));
      }
    
      halfedge_handle prev_outgoing(halfedge_handle he) const
      {
        return opposite(prev(he));
      }
    
      const halfedge_handle& incoming(vertex_handle v) const
      {
        return operator[](v).incoming;
      }
    
      halfedge_handle outgoing(vertex_handle v) const
      {
        if(is_isolated(v))
          return halfedge_handle::invalid();
        return opposite(incoming(v));
      }
  
      halfedge_handle prev(halfedge_handle he) const
      {
        auto prev_he = next(he);
        auto next_prev_he = next(prev_he);
     
        while(next_prev_he != he)
        {
          prev_he = next_prev_he;
          next_prev_he = next(next_prev_he);
        }
        return prev_he;
      }
    
      halfedge_handle prev_circ(halfedge_handle he) const
      {
        auto prev_he = opposite(next(opposite(he)));
        auto next_prev_he = next(prev_he);
     
        while(next_prev_he != he)
        {
          prev_he = opposite(next_prev_he);
          next_prev_he = next(prev_he);
        }
        return prev_he;
      }
    
      edge_handle edge(halfedge_handle he) const
      {
        return edge_handle(he.index() >> 1);
      }
    
      halfedge_handle halfedge(edge_handle e) const
      {
        return halfedge_handle{e.index() << 1};
      }
    
      std::array<halfedge_handle, 2> halfedges(edge_handle e) const
      {
        halfedge_handle he = halfedge_handle{e.index() << 1};
        halfedge_handle he_opp = he + 1;
        return {{he, he_opp}};
      }
    
      halfedge_handle find_halfedge(vertex_handle from, vertex_handle to) const
      {
          for(auto he : outgoing_halfedges(from))
            if(target(he) == to)
              return he;
        return halfedge_handle::invalid();
      }

      edge_handle find_edge(vertex_handle from, vertex_handle to) const
      {
        halfedge_handle he = find_halfedge(from, to);
        if(he.is_valid())
          return edge(he);

       return edge_handle::invalid();
      }
    
      std::size_t check(bool supress_warnings = false)
      {
        std::size_t count_error = 0;
        std::size_t count_warning = 0;
        for(auto he: halfedges())
       {
          if(!target(he).is_valid() )
          {
             std::cout << "target(" << he << ") is invalid "<< he << std::endl;
            ++count_error;
          }
          if(!next(he).is_valid())
          {
            std::cout << "next(" << he << ") is invalid "<< he << std::endl;
            ++count_error;
          }
          else if(face(he) != face(next(he)))
          {
            std::cout << "face(" << he <<") = "<<face(he) << std::endl;
            std::cout << "next(" << he <<") = "<< next(he) << std::endl;
            std::cout << "face(" << next(he) <<") = "<< face(next(he)) << std::endl;
            ++count_error;
          }
       }
        for(auto v: vertices())
        {
          if(is_isolated(v))
          {
            if(!supress_warnings)
            {
              std::cout << "mesh contains isolated vertex "<< v << " at " << position(v)<< std::endl;
              ++count_warning;
            }
          }
          else
          {
            if(status(edge(incoming(v))).is_removed())
            {
              std::cout << "mesh contains removed incoming halfedge at vertex "<< v << std::endl;
               ++count_error;
            }
            if(target(incoming(v)) != v)
            {
              std::cout << "mesh contains inconsistent vertex <-> halfedge linkage" <<std::endl;
              std::cout << "incoming("<< v << ") = " << incoming(v) << std::endl;
              std::cout << "target("<<incoming(v)<< ") =  "<< target(incoming(v)) << std::endl;
              ++count_error;
            }
            if(!is_boundary(v))
            {
              for(auto he: incoming_halfedges(v))
              {
                if(is_boundary(he))
                {
                 std::cout << "vertex "<< v << " is not adjusted to boundary vertex" << he << std::endl;
                   ++count_error;
                }
              }
            }
          }
        }
     
        for(auto f: faces())
        {
          if(status(f).is_removed())
          {
            if(inner(f).is_valid())
            {
              std::cout << "removed face " << f << "is referencing an halfedge " << std::endl;
              ++count_error;
            }
            continue;
          }
       
          if(!inner(f).is_valid() )
          {
            std::cout << "halfedge of face " << f << "is invalid "<<std::endl;
            ++count_error;
          }
          else
          {
            if(status(edge(inner(f))).is_removed())
            {
              std::cout << "halfedge of face " << f << "is removed"<<std::endl;
               ++count_error;
            }
           for(auto he : inner_halfedges(f))
           {
             if(face(he) != f)
             {
               std::cout << "face " << f << "contains inconsistent halfedge "<< he <<std::endl;
                ++count_error;
             }
           }
         }
       }
       return count_error + count_warning;
     }

   private:
   
      struct vertex_t
      {
        vertex_t() = default;
        halfedge_handle incoming;
        status_flags status;
      };
   
      struct halfedge_t
      {
        halfedge_t() = default;
        halfedge_t(vertex_handle to, face_handle f = face_handle::invalid())
          : target{to}
          , face{f}
        {
        }
     
        vertex_handle target;
        halfedge_handle next;
        face_handle face;
        status_flags status;
      };
    
      struct edge_t
      {
        edge_t() = default;
        edge_t(vertex_handle from, vertex_handle to)
          : halfedges{{halfedge_t(to), halfedge_t(from)}}
        {
        }
     
        std::array<halfedge_t,2> halfedges;
        status_flags status;
      };
    
      struct face_t
      {
        face_t() = default;
     
        face_t(halfedge_handle he)
          : halfedge{he}
        {
        }
     
        halfedge_handle halfedge;
        status_flags status;
      };
    
    
      auto operator[](edge_handle e)
      {
        auto he = halfedge(e);
        return std::tie(operator[](he), operator[](he + 1));
      }
    
      auto operator[](edge_handle e) const
      {
        auto he = halfedge(e);
        return std::tie(operator[](he), operator[](he + 1));
      }

      auto& operator[](halfedge_handle he)
      {
        edge_handle e = edge(he);
        return edges_[e.index()].halfedges[he.index() - (e.index() << 1)];
      }
        
      const auto& operator[](halfedge_handle he) const
      {
        edge_handle e = edge(he);
        return edges_[e.index()].halfedges[he.index() - (e.index() << 1)];
      }

      auto& operator[](face_handle f)
      {
        return faces_[f.index()];
      }

      const auto& operator[](face_handle f) const
      {
        return faces_[f.index()];
      }
    
      auto& operator[](vertex_handle v)
      {
        return vertices_[v.index()];
      }

      const auto& operator[](vertex_handle v) const
      {
        return vertices_[v.index()];
      }
    
      edge_handle add_edge(vertex_handle from, vertex_handle to)
      {
        edges_.emplace_back(from, to);
        halfedge_properties_.add_elems(2);
        return edge_properties_.add_elem();
      }
    
      // all references of from must be replaced by to
      //data stored under to is replaced by from
      void move(vertex_handle to, vertex_handle from)
      {
        for(auto he : incoming_halfedges(from))
          target(he) = to;
        incoming(to) = std::move(incoming(from));
        vertices_[to.index()] = std::move(vertices_[from.index()]);
        vertex_properties_.move(to.index(), from.index());
        incoming(from).invalidate();
      }
    
      // all references of from must be replaced by to
      //data stored under to is replaced by from
      void move(face_handle to, face_handle from)
      {
        for(auto he : inner_halfedges(from))
          face(he) = to;
      
        halfedge(from) = halfedge(to);
      
        faces_[to.index()] = std::move(faces_[from.index()]);
        face_properties_.move(to.index(), from.index());
      }
    
    /*  void move(edge_handle form, edge_handle to)
      {
        auto[hea1, hea2] = halfedges(a);
        auto[heb1, heb2] = halfedges(b);
      
        vertex_handle target_hea1 = target(hea1);
        vertex_handle target_hea2 = target(hea2);
        vertex_handle target_heb1 = target(heb1);
        vertex_handle target_heb2 = target(heb2);
      
        face_handle face_hea1 = face(hea1);
        face_handle face_hea2 = face(hea2);
        face_handle face_heb1 = face(heb1);
        face_handle face_heb2 = face(heb2);
      
        halfedge_handle prev_hea1 = prev(hea1);
        halfedge_handle prev_hea2 = prev(hea2);
        halfedge_handle prev_heb1 = prev(heb1);
        halfedge_handle prev_heb2 = prev(heb2);
      
        if(incoming(target_hea1) == hea1)
          incoming(target_hea1) = heb1;
        if(incoming(target_hea2) == hea2)
          incoming(target_hea2) = heb2;
        if(incoming(target_heb1) == heb1)
          incoming(target_heb1) = hea1;
        if(incoming(target_heb2) == heb2)
          incoming(target_heb2) = hea2;
      
        if(halfedge(face_hea1) == hea1)
          halfedge(face_hea1) = heb1;
        if(halfedge(face_hea2) == hea2)
          halfedge(face_hea2) = heb2;
      
        if(halfedge(face_heb1) == heb1)
          halfedge(face_heb1) = hea1;
        if(halfedge(face_heb2) == heb2)
          halfedge(face_heb2) = hea2;
      
        next(prev_hea1) = heb1;
        next(prev_hea2) = heb2;
        next(prev_heb1) = hea1;
        next(prev_heb2) = hea2;
      
        halfedge_properties_.move(hea1.index(), heb1.index());
        halfedge_properties_.move(hea2.index(), heb2.index());
        edges_[to.index()] = std::move(edges_[from.index()]);
        edge_properties_.move(from.index(), to.index());
      }
    */
    
      
    
      std::vector<edge_t> edges_;
      std::vector<vertex_t> vertices_;
      std::vector<face_t> faces_;

      vertex_property_handle<vector3> vertex_position_handle_;
      face_property_handle<vector3> face_normal_handle_;
      face_property_handle<color_t> face_color_handle_;
      halfedge_property_handle<vector3> halfedge_normal_handle_;
      halfedge_property_handle<vector2> halfedge_texcoord_handle_;
    
      utils::indexed_property_container<vertex_tag> vertex_properties_;
      utils::indexed_property_container<edge_tag> edge_properties_;
      utils::indexed_property_container<halfedge_tag> halfedge_properties_;
      utils::indexed_property_container<face_tag> face_properties_;
      utils::property_container mesh_properties_;
    };
  
  
    template<typename Scalar>
    void colorize_faces(mesh<Scalar>& m, const typename mesh<Scalar>::color_t& col)
    {
      for(auto f: m.faces())
        m.color(f) = col;
  
    }
  
    template<typename Scalar>
    std::size_t num_shells(mesh<Scalar>& mesh)
    {
      std::size_t count = 0;
      std::vector<bool> visited(mesh.num_faces(), false);
      for(auto f : mesh.faces())
      {
        if(visited[f.index()])
          continue;
        ++count;
      
        std::stack<face_handle> stack;
        stack.push(f);
        while(!stack.empty())
        {
          auto fc = stack.top();
          visited[fc.index()] = true;
          stack.pop();
        
          for(auto adj_face : mesh.faces(fc))
          {
            if(!visited[adj_face.index()])
              stack.push(adj_face);
          }
        }
      }
      return count;
    }
  
    template<typename Scalar>
    bool is_closed(mesh<Scalar>& mesh)
    {
      return owl::utils::none_of(mesh.halfedges(),
        [&mesh](auto he){ return mesh.is_boundary(he); });
    }
  
    template<typename Scalar>
    bool is_open(mesh<Scalar>& mesh)
    {
      return !is_closed(mesh);
    }
  
    template<typename Scalar>
    void transform(mesh<Scalar>& mesh, const matrix<Scalar,4,4>& point_trafo,
      const matrix<Scalar,4,4>& normal_trafo, bool auto_normalize = true)
    {
      for(auto& pos : mesh.positions(mesh.vertices()))
        pos = unhomog_point(point_trafo * homog_point(pos));
   
      for(auto& nml : mesh.normals(mesh.halfedges()))
        nml = unhomog_normal(normal_trafo * homog_normal(nml));
   
      for(auto& nml : mesh.normals(mesh.faces()))
        nml = unhomog_normal(normal_trafo * homog_normal(nml));
   
      if(auto_normalize)
      {
        for(auto& nml : mesh.normals(mesh.halfedges()))
          nml.normalize();
   
        for(auto& nml : mesh.normals(mesh.faces()))
          nml.normalize();
      }
    }
  
    template<typename Scalar>
    void transform(mesh<Scalar>& mesh, const matrix<Scalar,4,4>& point_trafo,
      bool auto_normalize = true)
    {
      transform(mesh, point_trafo, transpose(invert(point_trafo)), auto_normalize);
    }
  
  
    template<typename Scalar>
    void XYZ_to_YZX(mesh<Scalar>& mesh)
    {
      matrix<Scalar,4,4> m;
      m << 0, 1, 0, 0,
           0, 0, 1, 0,
           1, 0, 0, 0,
           0, 0, 0, 1;
    
      transform(mesh, m,false);
    }
  
    template <typename Scalar>
    void print_vertex_positions(const mesh<Scalar>& m)
    {
      for(auto pos: m.positions(m.vertices()))
        std::cout << pos << std::endl;
    }
  
    template <typename Scalar>
    std::vector<line_segment<Scalar,3>> compute_face_normal_line_segments(const mesh<Scalar>& m, const Scalar& length = Scalar{1})
    {
      std::vector<line_segment<Scalar,3>> normal_line_segments;
      normal_line_segments.reserve(m.num_faces());
    
      for(auto f: m.faces())
      {
        auto centroid = m.centroid(f);
        normal_line_segments.emplace_back(centroid, centroid + length * m.normal(f));
      }
    
     return normal_line_segments;
    }
  
  
  }
}
