//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <fstream>

#include "owl/utils/buffer.hpp"
#include "owl/export.hpp"

namespace owl
{
  namespace utils
  {
  
    OWL_API bool file_exists(const std::string& path);
  
    OWL_API std::streamsize file_size(const std::string& path);
  
    OWL_API std::string parent_path(const std::string& path);
  
    OWL_API std::string file_extension(const std::string& path);
  
    OWL_API bool read_file(const std::string& path, buffer& buf);
    
    OWL_API bool write_file(const std::string& path, buffer& buf);
  }
}
