//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2018 Sören König. All rights reserved.
//

#pragma once

#include <memory>
#include <iostream>
#include <functional>
#include <fstream>
#include <vector>

namespace owl
{
  namespace io
  {
    class off_reader
    {
    public:
      off_reader();
  
      off_reader(const std::string& filename);
  
      void open(const std::string& filename);
  
      bool is_open() const;
  
      bool listen_2_face(std::function<void(const std::vector<std::size_t>&)> fn);
  
      bool listen_2_vertex(std::function<void(const float &, const float&, const float&)> fn);
    
      bool read();
    
      std::size_t num_vertices() const;
    
      std::size_t num_faces() const;
  
  private:
      bool read_header();
  
      bool read_magic(std::istream& in);
    
      bool read_counts(std::istream& in);
    
      bool read_vertices(std::istream& in);
    
      bool read_faces(std::istream& in);
  
      std::size_t n_vertices_ = 0;
      std::size_t n_faces_ = 0;
      std::size_t n_edges_ = 0;
    
      std::ifstream file_;
    
      std::function<void(const float&, const float&, const float&)> on_vertex_;
    
      std::function<void(const std::vector<std::size_t>&)> on_face_;
    
    };
  
    bool create_off_cube(const std::string& filename)
    {
      std::ofstream off(filename);
      if(off.is_open())
      {
        off << "OFF" << std::endl;
        off << "8 6 0" << std::endl;
        off << "-0.500000 -0.500000 0.500000" << std::endl;
        off << "0.500000 -0.500000 0.500000" << std::endl;
        off << "-0.500000 0.500000 0.500000" << std::endl;
        off << "0.500000 0.500000 0.500000" << std::endl;
        off << "-0.500000 0.500000 -0.500000" << std::endl;
        off << "0.500000 0.500000 -0.500000" << std::endl;
        off << "-0.500000 -0.500000 -0.500000" << std::endl;
        off << "0.500000 -0.500000 -0.500000" << std::endl;
        off << "4 0 1 3 2" << std::endl;
        off << "4 2 3 5 4" << std::endl;
        off << "4 4 5 7 6" << std::endl;
        off << "4 6 7 1 0" << std::endl;
        off << "4 1 7 5 3" << std::endl;
        off << "4 6 0 2 4" << std::endl;
        return true;
      }
      return false;
    }
  }
}


