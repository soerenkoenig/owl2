#include "owl/io/off.hpp"
#include <iostream>

namespace owl
{
  namespace io
  {
    off_reader::off_reader()
    {
    }

    off_reader::off_reader(const std::string& filename)
    {
      open(filename);
    }

    void off_reader::open(const std::string& filename)
    {
      file_.open(filename, std::ios_base::in);
      read_header();
    }

    bool off_reader::is_open() const
    {
      return file_.is_open();
    }

    bool off_reader::listen_2_face(std::function<void(const std::vector<std::size_t>&)> fn)
    {
      if(!is_open())
        return false;
  
      on_face_ = fn;
      return true;
    }

    bool off_reader::listen_2_vertex(std::function<void(const float &, const float&, const float&)> fn)
    {
      if(!is_open())
        return false;
  
      on_vertex_ = fn;
      return true;
    }

    bool off_reader::read_magic(std::istream& in)
    {
      std::string magic;
      in >> magic;
      if(magic != "OFF")
      {
        in.setstate(std::ios::failbit);
        return false;
      }
      return true;
    }
  
    bool off_reader::read_counts(std::istream& in)
    {
      in >> n_vertices_ >> n_faces_ >> n_edges_;
      if(!in)
        return false;
  
      return true;
    }
  
    std::size_t off_reader::num_vertices() const
    {
      return n_vertices_;
    }

    std::size_t off_reader::num_faces() const
    {
      return n_faces_;
    }
    
    bool off_reader::read_vertices(std::istream& in)
    {
      for(std::size_t i = 0; i < n_vertices_; ++i)
      {
        float x, y, z;
        in >> x >> y >> z;
        if(!in)
          return false;
      
        if(on_vertex_)
          on_vertex_(x,y,z);
      }
      return true;
    }
  
    bool off_reader::read_faces(std::istream& in)
    {
      for(std::size_t i = 0; i < n_faces_; ++i)
      {
        std::size_t n;
        in >> n;
        if(!in)
          return false;
    
        std::vector<std::size_t> vertices(n);
        for(auto& v: vertices)
          in >> v;
        if(!in)
          return false;
    
        if(on_face_)
            on_face_(vertices);
      }
      return true;
    }
  
    bool off_reader::read_header()
    {
       if(!is_open())
        return false;
  
      if(!read_magic(file_))
        return false;
  
      if(!read_counts(file_))
        return false;
      return true;
    }
  
    bool off_reader::read()
    {
    
      if(!read_vertices(file_))
        return false;
  
      if(! read_faces(file_))
        return false;
  
      return true;
    }
  }
}
