//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include "owl/color/color.hpp"
#include "owl/math/mesh.hpp"
#include "owl/io/ply.hpp"
#include "owl/io/off.hpp"
#include "owl/utils/file_utils.hpp"
//#include "owl/utils/progress.hpp"

namespace owl
{
  namespace math
  {
    template <typename Scalar>
    bool read_off(math::mesh<Scalar>& mesh, const std::string& p)
    {
      if(!utils::file_exists(p))
        return false;
    
      mesh.clear();
      io::off_reader reader;
      reader.open(p);
      
      if(!reader.is_open())
        return false;
    
//      const std::uint64_t vertex_step_size = 1;
//      const std::uint64_t face_step_size = 6;
    
//      std::uint64_t n = vertex_step_size * reader.num_vertices() +
//                      face_step_size * reader.num_faces();
//      utils::progress loading_progress(n);
      mesh.reserve_vertices(reader.num_vertices());
      mesh.reserve_faces(reader.num_faces());
    
    
      reader.listen_2_vertex([&mesh](const float& x, const float& y,const float& z)
        {
          mesh.add_vertex(vector<Scalar,3>(x, y, z));
        });
    
      reader.listen_2_face([&mesh](const std::vector<std::size_t>& indices)
        {
          std::vector<math::vertex_handle> vertex_indices;
          for(auto v: indices)
            vertex_indices.push_back(math::vertex_handle(v));
          mesh.add_face(vertex_indices);
        });
   
      return reader.read();
    }
  
    template <typename Scalar>
    bool read_ply(math::mesh<Scalar>& mesh, const std::string& p)
    {
      if(!utils::file_exists(p))
        return false;
    
    
      mesh.clear();
    
      io::ply_reader ply;
    
      ply.open(p);
      if(!ply.is_open())
        return false;
    
//      const std::uint64_t vertex_step_size = 1;
//      const std::uint64_t face_step_size = 6;
    
//      std::uint64_t n = vertex_step_size * ply.get_element_count("vertex") +
//                      face_step_size * ply.get_element_count("face");
//      utils::progress loading_progress(n);
  
      std::vector<math::vector<Scalar,3>> positions;
      std::vector<math::vector<Scalar,3>> normals;
  
      math::vector<Scalar,3> pos, nml;
      color::rgba8u col, back_col;
      std::int32_t material_idx;
    
    
      ply.listen_2_element_begin(
        [&](const std::string& element_name, std::size_t n)
        {
          if(element_name == "vertex")
            mesh.reserve_vertices(n);
          if(element_name == "face")
            mesh.reserve_faces(n);
          if(element_name == "edge")
            return;
          if(element_name == "material")
            return;
        });
  
      ply.listen_2_element_property<float>("vertex", "x",
        [&pos](const float& x){ pos.x() = x;} );
      ply.listen_2_element_property<float>("vertex", "y",
        [&pos](const float& y){ pos.y() = y; });
      ply.listen_2_element_property<float>("vertex", "z",
        [&pos](const float& z){ pos.z() = z;
        });
    
      ply.listen_2_element_property<float>("vertex", "nx",
        [&nml](const float& nx){ nml.x() = nx; });
      ply.listen_2_element_property<float>("vertex", "ny",
        [&nml](const float& ny){ nml.y() = ny; });
      ply.listen_2_element_property<float>("vertex", "nz",
        [&nml](const float& nz){ nml.z() = nz; });
    
      ply.listen_2_element_property<std::uint8_t>("vertex", "red",
        [&col](const std::uint8_t& red){ col.r() = red; });
      ply.listen_2_element_property<std::uint8_t>("vertex", "green",
        [&col](const std::uint8_t& green){ col.g() = green; });
      ply.listen_2_element_property<std::uint8_t>("vertex", "blue",
        [&col](const std::uint8_t& blue){ col.b() = blue; });
      ply.listen_2_element_property<std::uint8_t>("vertex", "alpha",
        [&col](const std::uint8_t& alpha){ col.a() = alpha; });
    
      ply.listen_2_element_property<std::uint8_t>("face", "back_red",
        [&back_col](const std::uint8_t& red){ back_col.r() = red; });
      ply.listen_2_element_property<std::uint8_t>("face", "back_green",
        [&back_col](const std::uint8_t& green){ back_col.g() = green; });
      ply.listen_2_element_property<std::uint8_t>("face", "back_blue",
        [&back_col](const std::uint8_t& blue){ back_col.b() = blue; });
      ply.listen_2_element_property<int>("face", "vertex_indices",
        [&](const std::vector<int>& vindices)
        {
          std::vector<math::vertex_handle> vertex_indices;
          for(auto v: vindices)
            vertex_indices.push_back(math::vertex_handle(v));
          mesh.add_face(vertex_indices);
//          loading_progress.step(face_step_size);
        });
    
      ply.listen_2_element_property<std::int32_t>("vertex", "material_index",
        [&material_idx](const std::int32_t& id){ material_idx = id; });
  
      ply.listen_2_element_item_end("vertex",
        [&](std::size_t)
        {
          mesh.add_vertex(pos);
  //        loading_progress.step(vertex_step_size);
        });
    
      ply.listen_2_element_property<std::int32_t>("edge", "vertex1",
        [](const std::int32_t& i){ std::cout << " vertex1 = " << i; });
      ply.listen_2_element_property<std::int32_t>("edge", "vertex2",
        [](const std::int32_t& i){ std::cout << " vertex2 = " << i; });
      ply.listen_2_element_property<std::uint8_t>("edge", "crease_tag",
        [](const std::uint8_t& i){ std::cout << " create_tag = " << (int)i; });
    
      ply.listen_2_element_property<std::uint8_t>("material", "red",
        [](const std::uint8_t& r){ std::cout << " color = " << (int)r <<" "; });
      ply.listen_2_element_property<std::uint8_t>("material", "green",
        [](const std::uint8_t& g){ std::cout << (int)g <<" "; });
      ply.listen_2_element_property<std::uint8_t>("material", "blue",
        [](const std::uint8_t& b){ std::cout << (int)b <<" "; });
      ply.listen_2_element_property<std::uint8_t>("material", "alpha",
        [](const std::uint8_t& a){ std::cout << (int)a; });
  
      ply.listen_2_element_property<float>("material", "reflect_coeff",
        [](const float& reflection){ std::cout << reflection << " "; });
      ply.listen_2_element_property<float>("material", "refract_coeff",
        [](const float& refraction){ std::cout << refraction << " "; });
      ply.listen_2_element_property<float>("material", "refract_index",
        [](const float& refract_index){ std::cout << refract_index << " "; });
      ply.listen_2_element_property<float>("material", "extinct_coeff",
        [](const float& extinct_coeff){ std::cout << extinct_coeff << " ";});

      ply.read();
      return true;
    }
  
    template <typename Scalar>
    bool read(math::mesh<Scalar>& mesh, const std::string& p)
    {
      std::cout << "reading mesh "<< p << "... ";
      std::cout.flush();
    
//      utils::progress overall_progress(10);
//      overall_progress.make_current(10);
      bool ret;
      auto extension = utils::file_extension(p);
      if(extension == ".ply" || extension == ".PLY")
        ret = read_ply(mesh, p);
      else if(extension == ".off" || extension == ".OFF")
        ret = read_off(mesh, p);
      else ret = false;
   //  overall_progress.resign_current();
      std::cout << "complete" << std::endl;
     // if(ret)
     //   mesh.check();
    
      return ret;
    }
  }
}
