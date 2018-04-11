#include "file_utils.hpp"

namespace owl
{
  namespace utils
  {
    bool file_exists(const std::string& path)
    {
      std::ifstream file(path);
      return static_cast<bool>(file);
    }
  
    std::string file_extension(const std::string& path)
    {
      std::size_t i = path.rfind('.', path.length());
      if (i != std::string::npos)
        return(path.substr(i, path.length() - i));
      return("");
    }
  
    std::streamsize file_size(const std::string& path)
    {
      std::ifstream file(path, std::ios::binary | std::ios::ate);
      std::streamsize size = file.tellg();
      return size;
    }
  
    std::string parent_path(const std::string& path)
    {
      size_t found = path.find_last_of("/\\");
      if(found == std::string::npos)
        return "";
  
      return path.substr(0,found);
    }
  
    bool read_file(const std::string& path, buffer& buf)
    {
      std::streampos size;
      std::ifstream file(path, std::ios::binary);

      file.seekg(0, std::ios::end);
      size = file.tellg();
      file.seekg(0, std::ios::beg);
    
      buf.resize(static_cast<std::size_t>(size));
      return (bool)file.read((char*) buf.data(), size);
    }
  
  
    bool write_file(const std::string& path, buffer& buf)
    {
      std::ofstream file(path, std::ios::out | std::ios::binary);
      return (bool)file.write((const char*) buf.data(), buf.size());
    }
  }
}
