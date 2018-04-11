#include "owl/image/image_io.hpp"
#include <fstream>
#include "owl/utils/file_utils.hpp"
#include "owl/utils/buffer.hpp"

#define STBI_NO_STDIO

#define STB_IMAGE_IMPLEMENTATION
//#define STB_IMAGE_STATIC
#define STB_IMAGE_INLINE
#include "thirdparty/stb/stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
//#define STB_IMAGE_RESIZE_STATIC
#define STB_IMAGE_RESIZE_INLINE
#include "thirdparty/stb/stb_image_resize.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
//#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_INLINE
#include "thirdparty/stb/stb_image_write.h"

namespace owl
{
  namespace image
  {
    bool write_ppm(const rgb8u_image& img, const std::string& path)
    {
      std::ofstream ofs;
      ofs.open(path);
      if(!ofs.is_open())
        return false;
    
      ofs << "P6\n" << img.width() << " " << img.height() << "\n255\n";
      ofs.write(reinterpret_cast<const char*>(img.data()), img.width() * img.height() * 3);
      return true;
    }
  
    void writer(void *context, void *data, int size)
    {
      utils::buffer& buf = *reinterpret_cast<utils::buffer*>(context);
      buf.append(utils::buffer(data, size, false));
    }
  
    bool write_jpg(const rgb8u_image& img, const std::string &path, int quality)
    {
      utils::buffer buf;
      int ret = stbi_write_jpg_to_func(&writer, &buf, static_cast<int>(img.width()),
                             static_cast<int>(img.height()), 3, img.data(), quality);
      if(ret == 0)
        return false;
      return utils::write_file(path, buf);
    }
  
    bool write_bmp(const rgb8u_image& img, const std::string &path)
    {
      utils::buffer buf;
      int ret = stbi_write_bmp_to_func(&writer,&buf, static_cast<int>(img.width()),
                           static_cast<int>(img.height()),3, img.data());
      if(ret == 0)
          return false;
       return utils::write_file(path, buf);
    }
  
    bool write_png(const rgb8u_image& img, const std::string &path)
    {
      utils::buffer buf;
      int ret = stbi_write_png_to_func(&writer, &buf, static_cast<int>(img.width()),
        static_cast<int>(img.height()), 3,img.data(), 0);
      if(ret == 0)
        return false;
      return utils::write_file(path, buf);
    }
  
    bool read_image(rgb8u_image& img, const std::string &path)
    {
      utils::buffer buf;
      if(!owl::utils::file_exists(path))
        return false;
   
      if(!owl::utils::read_file(path, buf))
      return false;
   
      int w,h,d;
      stbi_uc *input_pixels = stbi_load_from_memory(buf.data(),(int)buf.size(), &w, &h, &d, STBI_rgb);
   
      if(input_pixels == nullptr)
      {
        std::cout << std::string(stbi_failure_reason()) <<std::endl;
        // throw image_error("failed to open image:" +
        //               std::string(stbi_failure_reason()));
        return false;
      }
    
      img.resize(w,h);

      std::copy_n(input_pixels, w * h * 3, reinterpret_cast<stbi_uc*>(img.data()));

      stbi_image_free(input_pixels);
      return true;
    }

  }
}

