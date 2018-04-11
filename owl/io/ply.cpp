
#include "owl/io/ply.hpp"
#include <iostream>

namespace owl
{
  namespace io
  {
    ply_reader::ply_property::ply_property(const std::string& name)
      : name(name)
    {}
  
    ply_reader::element::element()
    {}
  
    ply_reader::element::element(const std::string& name, std::size_t count)
      : name(name)
      , count(count)
    {}
  
    auto ply_reader::element::get_property(const std::string& name) -> std::shared_ptr<ply_property>
    {
      auto it = std::find_if(properties.begin(), properties.end(),
        [&name](const std::shared_ptr<ply_property>& p) -> bool
        {
          return p->name == name;
        });
      if(it == properties.end())
        return nullptr;
      else
        return *it;
    }
  
    std::istream& ply_reader::element::read_ascii(std::istream& in)
    {
      for(std::size_t i = 0; i < count; ++i)
      {
        if(on_begin)
          on_begin(i);
        for(auto& p: properties)
          p->read_ascii(in);
        if(on_end)
          on_end(i);
      }
  
      return in;
    }
  
    std::istream& ply_reader::element::read_binary(std::istream& in, bool swap_endianess)
    {
      for(std::size_t i = 0; i < count; ++i)
      {
        if(on_begin)
          on_begin(i);
        for(auto& p: properties)
          p->read_binary(in, swap_endianess);
        if(on_end)
          on_end(i);
      }
      return in;
    }
  
    ply_reader::ply_reader()
      : header_complete(false)
      , format_complete(false)
    {
    }
  
    ply_reader::ply_reader(const std::string& filename)
      : header_complete(false)
      , format_complete(false)
    {
      open(filename);
    }
  
    void ply_reader::open(const std::string& filename)
    {
      file.open(filename, std::ios_base::in | std::ios_base::binary);
      if(!file.is_open())
        return;
  
      if(!read_header(file))
      {
        file.close();
        return;
      }
    }
  
    bool ply_reader::is_open() const
    {
      return file.is_open();
    }
  
    bool ply_reader::listen_2_element_begin(std::function<void(const std::string&, std::size_t)> fn)
    {
      on_element_begin = fn;
      return true;
    }
  
    bool ply_reader::listen_2_element_end(std::function<void(const std::string&)> fn)
    {
      on_element_end = fn;
      return true;
    }
  
  
    bool ply_reader::listen_2_element_item_begin(const std::string& element_name,
      std::function<void(const std::size_t i)> fn)
    {
      auto e = get_element(element_name);
      if(!e)
        return false;
  
      e->on_begin = fn;
      return true;
    }
    bool ply_reader::listen_2_element_item_end(const std::string& element_name,
      std::function<void(const std::size_t i)> fn)
    {
      auto e = get_element(element_name);
      if(!e)
        return false;
      e->on_end = fn;
      return true;
    }
  
    bool ply_reader::read()
    {
      if(!is_open())
        return false;
      read_data(file);
      bool succ = (bool)file;
      close();
      return succ;
    }
  
    void ply_reader::close()
    {
      header_complete = false;
      format_complete = false;
      file.close();
    }
  
    //get element by name returning an optional element
    ply_reader::element* ply_reader::get_element(const std::string& name)
    {
      auto it = std::find_if(elements.begin(),elements.end(),
        [&name](const element& e)-> bool { return e.name == name; });
      if(it != elements.end())
        return &(*it);
      return nullptr;
    }
  
    //get element by name returning an optional element
    const ply_reader::element* ply_reader::get_element(const std::string& name) const
    {
      auto it = std::find_if(elements.begin(),elements.end(),
        [&name](const element& e)-> bool { return e.name == name; });
      if(it != elements.end())
        return &(*it);
      return nullptr;
    }
  
    std::size_t ply_reader::get_element_count(const std::string& name) const
    {
      const element* e = get_element(name);
      if(e == nullptr)
        return 0;
      else
        return e->count;
    }
  
    std::istream& ply_reader::read_header(std::istream& in)
    {
      format_complete = header_complete = false;
  
      if(!read_magic(in))
        return in;
      std::string l;
      while(!in.eof())
      {
        std::string keyword;
        in >> keyword;
        if(!in)
            break;
    
        if(keyword == "format")
        {
          if(!read_format(in))
            break;
          format_complete =true;
        }
        else if(keyword == "element")
        {
          if(!read_element(in))
            break;
        }
        else if(keyword == "property")
        {
          if(!read_property(in))
              break;
        }
        else if(keyword == "comment")
        {
          if(! read_comment(in))
            break;
        }
        else if(keyword == "end_header")
        {
          if(format_complete)
              header_complete =true;
          std::string rest;
          std::getline(in,rest);
          break;
        }
      }
      return in;
    }
  
    inline bool host_is_big_endian()
    {
      union { long l; char c[sizeof (long)]; } u;
      u.l = 1;
      return (u.c[sizeof (long) - 1] == 1) ? true : false;
    }
  
    std::istream& ply_reader::read_data(std::istream& in)
    {
      if(in)
      {
        if(on_comment)
        {
          for(auto& msg: comments)
            on_comment(msg);
        }
    
        if(file_encoding == encoding_type::ascii)
        {
          for(auto& e: elements)
          {
            if(on_element_begin)
              on_element_begin(e.name, e.count);
            if(!e.read_ascii(in))
              return in;
            if(on_element_end)
              on_element_end(e.name);
          }
        }
        else
        {
          bool is_big_endian_machine = host_is_big_endian();
        
          bool swap_endianess =
            (is_big_endian_machine && file_encoding == encoding_type::binary_little_endian)
            || (!is_big_endian_machine && file_encoding == encoding_type::binary_big_endian);
        
          for(auto& e: elements)
          {
            if(on_element_begin)
              on_element_begin(e.name, e.count);
            if(!e.read_binary(in, swap_endianess))
              return in;
            if(on_element_end)
              on_element_end(e.name);
          }
        }
      }
      return in;
    }
  
    std::istream& ply_reader::read_magic(std::istream& in)
    {
      std::string keyword;
      in >> keyword;
      if(keyword != "ply")
          in.setstate(std::ios::failbit);
      return in;
    }
  
  
    std::istream& ply_reader::read_comment(std::istream& in)
    {
      std::string msg;
      std::getline(in, msg);
      comments.push_back(msg);
      return in;
    }
  
    std::istream& ply_reader::read_format(std::istream& in)
    {
      std::string encoding_string;
      in >> encoding_string;
      if(!in)
        return in;
  
      if(encoding_string == "ascii")
        file_encoding = encoding_type::ascii;
      else if(encoding_string == "binary_little_endian")
        file_encoding = encoding_type::binary_little_endian;
      else if(encoding_string == "binary_big_endian")
        file_encoding = encoding_type::binary_big_endian;
      else
        in.setstate(std::ios::failbit);
      if(!in)
        return in;
  
      in >> version;
      if(!in)
          return in;
  
      if(version != "1.0")
        in.setstate(std::ios::failbit);
      std::string rest;
      std::getline(in, rest);
      return in;
    }
  
    std::istream& ply_reader::read_element(std::istream& in)
    {
      std::string name;
      in >> name;
      std::size_t count;
      in >> count;
  
      if(in)
        elements.push_back(element(name,count));
  
      return in;
    }
  
    std::istream& ply_reader::read_property(std::istream& in)
    {
      std::string type_or_list;
      in >> type_or_list;
      if(type_or_list == "list")
      {
        std::string size_type;
        in >> size_type;
        std::string value_type;
        in >> value_type;
        std::string name;
        in >> name;
        if(in)
          add_list_property(size_type,value_type,name);
      }
      else
      {
        std::string name;
        in >> name;
        if(in)
          add_scalar_property(type_or_list,name);
      }
      return in;
    }
  
    bool ply_reader::add_scalar_property(const std::string& value_type,const std::string& name)
    {
      if(elements.empty())
        return false;
  
      if(value_type == "float" || value_type == "float32")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<float>>(name));
        return true;
      }
      if(value_type == "double" || value_type == "float64")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<double>>(name));
        return true;
      }
      if(value_type == "uint8" || value_type == "uchar")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<std::uint8_t>>(name));
        return true;
      }
      if(value_type == "int8" || value_type == "char")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<std::int8_t>>(name));
        return true;
      }
      if(value_type == "uint16" || value_type == "ushort")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<std::uint16_t>>(name));
        return true;
      }
      if(value_type == "int16" || value_type == "short")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<std::int16_t>>(name));
        return true;
      }
      if(value_type == "uint32" || value_type == "uint")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<std::uint32_t>>(name));
        return true;
      }
      if(value_type == "int32" || value_type == "int")
      {
        elements.back().properties.push_back(std::make_shared<scalar_property<std::int32_t>>(name));
        return true;
      }
      return false;
    }
  
    bool ply_reader::add_list_property(const std::string& size_type,
      const std::string& value_type,const std::string& name)
    {
      if(size_type == "uint8" || size_type == "uchar")
      {
        add_list_property<std::uint8_t>(value_type, name);
        return true;
      }
      if(size_type == "int8" || size_type == "char")
      {
        add_list_property<std::int8_t>(value_type, name);
        return true;
      }
      if(size_type == "uint16" || size_type == "ushort")
      {
        add_list_property<std::uint16_t>(value_type, name);
        return true;
      }
      if(size_type == "int16" || size_type == "short")
      {
        add_list_property<std::int16_t>(value_type, name);
        return true;
      }
      if(size_type == "uint32" || size_type == "uint")
      {
        add_list_property<std::uint32_t>(value_type, name);
        return true;
      }
      if(size_type == "int32" || size_type == "int")
      {
        add_list_property<std::int32_t>(value_type, name);
        return true;
      }
      return false;
    }
  
    namespace detail
    {
      template <>
      std::istream& read_ascii<std::uint8_t>(std::istream& in, std::uint8_t& v)
      {
        std::uint16_t temp;
        in >> temp;
        v = (std::uint8_t)temp;
        return in;
      }
    }
  
    bool create_ply_cube_ascii(const std::string& filename)
    {
      std::ofstream ply(filename);
      if(ply.is_open())
      {
        ply << "ply" << std::endl;
        ply << "format ascii 1.0" << std::endl;
        ply << "comment created by owl" << std::endl;
        ply << "element vertex 8" << std::endl;
        ply << "property float32 x" << std::endl;
        ply << "property float32 y" << std::endl;
        ply << "property float32 z" << std::endl;
        ply << "element face 6" << std::endl;
        ply << "property list uint8 int32 vertex_indices" << std::endl;
        ply << "end_header" << std::endl;
        ply << "-1 -1 -1" << std::endl;
        ply << "1 -1 -1" << std::endl;
        ply << "1 1 -1" << std::endl;
        ply << "-1 1 -1" << std::endl;
        ply << "-1 -1 1" << std::endl;
        ply << "1 -1 1" << std::endl;
        ply << "1 1 1" << std::endl;
        ply << "-1 1 1" << std::endl;
        ply << "4 0 1 2 3" << std::endl;
        ply << "4 5 4 7 6" << std::endl;
        ply << "4 6 2 1 5" << std::endl;
        ply << "4 3 7 4 0" << std::endl;
        ply << "4 7 3 2 6" << std::endl;
        ply << "4 5 1 0 4" << std::endl;
        return true;
      }
      return false;
    }
  }
}

  
  /*
  
    namespace test {
      
      
      
        bool create_ply_cube_ascii(const std::string& filename)
        {
            std::ofstream ply(filename);
            if(ply.is_open())
            {
                ply << "ply"<<std::endl;
                ply << "format ascii 1.0"<<std::endl;
                ply << "comment created by platoply"<<std::endl;
                ply << "element vertex 8"<<std::endl;
                ply << "property float32 x"<<std::endl;
                ply << "property float32 y"<<std::endl;
                ply << "property float32 z"<<std::endl;
                ply << "element face 6"<<std::endl;
                ply << "property list uint8 int32 vertex_indices"<<std::endl;
                ply << "end_header"<<std::endl;
                ply << "-1 -1 -1"<<std::endl;
                ply << "1 -1 -1"<<std::endl;
                ply << "1 1 -1"<<std::endl;
                ply << "-1 1 -1"<<std::endl;
                ply << "-1 -1 1"<<std::endl;
                ply << "1 -1 1"<<std::endl;
                ply << "1 1 1"<<std::endl;
                ply << "-1 1 1"<<std::endl;
                ply << "4 0 1 2 3"<<std::endl;
                ply << "4 5 4 7 6"<<std::endl;
                ply << "4 6 2 1 5"<<std::endl;
                ply << "4 3 7 4 0"<<std::endl;
                ply << "4 7 3 2 6"<<std::endl;
                ply << "4 5 1 0 4"<<std::endl;
                return true;
            }
        
            return false;
        }
      
      
      
      
        void test_ply_reader()
        {
            if(!io::test::create_ply_cube_ascii("mycube.ply"))
                std::cout << "error creating cube file"<<std::endl;
        
        
            io::ply_reader my_ply;
            my_ply.open("mycube.ply");
            if(my_ply.is_open())
            {
            
                my_ply.listen_2_element_begin([](const std::string& element_name)
                                              {
                                                  std::cout <<"reading "<< element_name <<" data...\n";
                                              });
            
                my_ply.listen_2_element_end([](const std::string& element_name)
                                            { std::cout <<element_name<<" data complete"<<std::endl;});
            
            
                my_ply.listen_2_element_item_begin("vertex",[](std::size_t i){ std::cout <<"vertex "<< i <<":";});
                my_ply.listen_2_element_item_end("vertex",[](std::size_t i){ std::cout <<std::endl;});
                my_ply.listen_2_element_item_begin("face",[](std::size_t i){ std::cout <<"face "<< i <<": ";});
                my_ply.listen_2_element_item_end("face",[](std::size_t i){ std::cout <<std::endl;});
                my_ply.listen_2_element_item_begin("edge",[](std::size_t i){ std::cout <<"edge "<< i <<": ";});
                my_ply.listen_2_element_item_end("edge",[](std::size_t i){ std::cout <<std::endl;});
                my_ply.listen_2_element_item_begin("material",[](std::size_t i){ std::cout <<"material "<< i <<": ";});
                my_ply.listen_2_element_item_end("material",[](std::size_t i){ std::cout <<std::endl;});
            
                my_ply.listen_2_element_property<float>("vertex","x",[](const float& x){ std::cout <<" position = "<< x <<" ";});
                my_ply.listen_2_element_property<float>("vertex","y",[](const float& y){ std::cout <<y <<" ";});
                my_ply.listen_2_element_property<float>("vertex","z",[](const float& z){ std::cout << z;});
                my_ply.listen_2_element_property<float>("vertex","nx",[](const float& nx){ std::cout <<" normal = " <<nx <<" ";});
                my_ply.listen_2_element_property<float>("vertex","ny",[](const float& ny){ std::cout << ny <<" ";});
                my_ply.listen_2_element_property<float>("vertex","nz",[](const float& nz){ std::cout << nz ;});
                my_ply.listen_2_element_property<std::uint8_t>("vertex","red",[](const std::uint8_t& r){ std::cout <<" color = " <<(int)r <<" ";});
                my_ply.listen_2_element_property<std::uint8_t>("vertex","green",[](const std::uint8_t& g){ std::cout << (int)g <<" ";});
                my_ply.listen_2_element_property<std::uint8_t>("vertex","blue",[](const std::uint8_t& b){ std::cout << (int)b <<" ";});
                my_ply.listen_2_element_property<std::uint8_t>("vertex","alpha",[](const std::uint8_t& a){ std::cout << (int)a ;});
                my_ply.listen_2_element_property<std::int32_t>("vertex","material_index",[](const std::int32_t& i){ std::cout << " material index = " <<i ;});
            
                my_ply.listen_2_element_property<std::int32_t>("edge","vertex1",[](const std::int32_t& i)
                                                               { std::cout << " vertex1 = " <<i ;});
                my_ply.listen_2_element_property<std::int32_t>("edge","vertex2",[](const std::int32_t& i)
                                                               { std::cout << " vertex2 = " <<i ;});
                my_ply.listen_2_element_property<std::uint8_t>("edge","crease_tag",[](const std::uint8_t& i)
                                                               { std::cout << " create_tag = " <<(int)i ;});
            
                my_ply.listen_2_element_property<std::uint8_t>("material","red",[](const std::uint8_t& r){ std::cout <<" color = " <<(int)r <<" ";});
                my_ply.listen_2_element_property<std::uint8_t>("material","green",[](const std::uint8_t& g){ std::cout << (int)g <<" ";});
                my_ply.listen_2_element_property<std::uint8_t>("material","blue",[](const std::uint8_t& b){ std::cout << (int)b <<" ";});
                my_ply.listen_2_element_property<std::uint8_t>("material","alpha",[](const std::uint8_t& a){ std::cout << (int)a ;});
            
                my_ply.listen_2_element_property<float>("material","reflect_coeff",[](const float& reflection){ std::cout <<reflection <<" ";});
                my_ply.listen_2_element_property<float>("material","refract_coeff",[](const float& refraction){ std::cout <<refraction<<" ";});
                my_ply.listen_2_element_property<float>("material","refract_index",[](const float& refract_index){ std::cout <<refract_index<<" ";});
                my_ply.listen_2_element_property<float>("material","extinct_coeff",[](const float& extinct_coeff){ std::cout <<extinct_coeff<<" ";});
            
            
            
                my_ply.listen_2_element_property<std::int32_t>("face","vertex_indices",[](const std::vector<std::int32_t>& vindices)
                                                               {
                                                                   std::cout << "vertex_indices = ";
                                                                   for(auto v: vindices)
                                                                       std::cout << v <<" ";
                                                               });
            
            
                my_ply.read();
            }
        
        }
    }

}*/

