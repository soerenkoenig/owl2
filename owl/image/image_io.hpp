//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2018 Sören König. All rights reserved.
//

#pragma once

#include "owl/export.hpp"
#include "owl/image/image.hpp"

namespace owl
{
  namespace image
  {
    OWL_API bool write_ppm(const rgb8u_image& img, const std::string& path);
    OWL_API bool write_jpg(const rgb8u_image& img, const std::string& path, int quality = 96);
    OWL_API bool write_bmp(const rgb8u_image& img, const std::string& path);
    OWL_API bool write_png(const rgb8u_image& img, const std::string& path);
  
    OWL_API bool read_image(rgb8u_image& img, const std::string& path);
  }
}
