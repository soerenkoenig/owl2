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
#include <fstream>
#include <vector>

namespace owl
{
  namespace io
  {
    class ply_reader
    {
    public:
    
      ply_reader();
  
      ply_reader(const std::string& filename);
  
      void open(const std::string& filename);
  
      bool is_open() const;
  
      bool listen_2_element_begin(std::function<void(const std::string&, std::size_t)> fn);
  
      bool listen_2_element_end(std::function<void(const std::string&)> fn);
  
      bool listen_2_element_item_begin(const std::string& element_name,
        std::function<void(const std::size_t i)> fn);
      bool listen_2_element_item_end(const std::string& element_name,
        std::function<void(const std::size_t i)> fn);
  
      bool listen_2_comments(std::function<void(const std::string&)> fn);
  
      template <typename T>
      bool listen_2_element_property(const std::string& element_name,
        const std::string& property_name, std::function<void(const T& v)> fn);
      template <typename T>
      bool listen_2_element_property(const std::string& element_name,
        const std::string& property_name, std::function<void(const std::vector<T>& v)> fn);
  
      bool read();
  
      void close();
    
      std::size_t get_element_count(const std::string& name) const;
    
    private:
      struct ply_property
      {
        std::string name;
    
        virtual std::istream& read_ascii(std::istream& in) = 0;
    
        virtual std::istream& read_binary(std::istream& in, bool swap_endianess) = 0;
    
        ply_property(const std::string& name);
      };
  
      template <typename T>
      struct scalar_property: public ply_property
      {
        std::function<void(const T&)> on_read;
    
        scalar_property(const std::string& name);
    
        std::istream& read_ascii(std::istream& in);
    
        std::istream&  read_binary(std::istream& in, bool swap_endianess);
      };
    
    
      template <typename C, typename T>
      struct list_property: ply_property
      {
        std::function<void(const std::vector<T>&)> on_read;
    
        list_property(const std::string& name);
    
        std::istream& read_ascii(std::istream& in);
    
        std::istream& read_binary(std::istream& in, bool swap_endianess);
      };
    
      struct element
      {
        std::string name;
        std::size_t count;
    
        std::vector<std::shared_ptr<ply_property>> properties;
    
        std::function<void(const std::size_t& i)> on_begin, on_end;
    
        element();
    
        element(const std::string& name, std::size_t count);
    
        template <typename T>
        bool listen(const std::string& property_name, std::function<void(const T& v)> fn);
    
        template <typename T>
        bool listen(const std::string& property_name, std::function<void(const std::vector<T>& v)> fn);
    
        std::shared_ptr<ply_property> get_property(const std::string& name);
    
        std::istream& read_ascii(std::istream& in);
    
        std::istream& read_binary(std::istream& in, bool swap_endianess);
      };
  
      //get element by name returning an optional element
      element* get_element(const std::string& name);
    
      const element* get_element(const std::string& name) const;
  
      std::istream& read_header(std::istream& in);
  
      std::istream& read_data(std::istream& in);
  
      std::istream& read_magic(std::istream& in);
  
      std::istream& read_comment(std::istream& in);
  
      std::istream& read_format(std::istream& in);
  
      std::istream& read_element(std::istream& in);
  
      std::istream& read_property(std::istream& in);
  
      bool add_scalar_property(const std::string& value_type, const std::string& name);
  
      template <typename C>
      bool add_list_property( const std::string& value_type, const std::string& name);
  
      bool add_list_property(const std::string& size_type,
        const std::string& value_type, const std::string& name);
  
      bool header_complete;
  
      bool format_complete;
  
      std::string version;
  
      enum class encoding_type
      {
        ascii,
        binary_little_endian,
        binary_big_endian
      } file_encoding;
  
      std::vector<element> elements;
      std::vector<std::string> comments;
  
      std::function<void(const std::string&)> on_comment;
  
      std::function<void(const std::string&, std::size_t)> on_element_begin;
  
      std::function<void(const std::string&)> on_element_end;
  
      std::ifstream file;
    };
  
    namespace detail
    {
      template <typename T>
      std::istream& read_ascii(std::istream& in, T& v)
      {
        in >> v;
        return in;
      }
    
      template <>
      inline std::istream& read_ascii<std::uint8_t>(std::istream& in, std::uint8_t& v);
    } 
  
    template <typename T>
    ply_reader::scalar_property<T>::scalar_property(const std::string& name)
      : ply_property(name)
    {}
  
    template <typename T>
    std::istream& ply_reader::scalar_property<T>::read_ascii(std::istream& in)
    {
      T value;
      if(io::detail::read_ascii(in,value))
      {
        if(on_read)
          on_read(value);
      }
      return in;
    }
  
    template <typename T>
    std::istream&  ply_reader::scalar_property<T>::read_binary(std::istream& in,
      bool swap_endianess)
    {
      T value;
      in.read((char*)&value, sizeof(T));
      if(in && swap_endianess)
      {
        if(swap_endianess)
        {
          unsigned char *memp = reinterpret_cast<unsigned char*>(&value);
          std::size_t n = sizeof(T);
          std::reverse(memp, memp + n);
        }
      }
      if(in && on_read)
        on_read(value);
  
      return in;
    }
  
    template <typename C, typename T>
    ply_reader::list_property<C,T>::list_property(const std::string& name)
      : ply_property(name)
    {}
  
    template <typename C, typename T>
    std::istream& ply_reader::list_property<C,T>::read_ascii(std::istream& in)
    {
      C n;
      std::vector<T> values;
  
      if(!io::detail::read_ascii(in, n))
        return in;
      values.resize(n);
  
      for(auto&v: values )
        if(!io::detail::read_ascii(in, v))
          break;
      if(in && on_read)
        on_read(values);
  
      return in;
    }
  
    template <typename C, typename T>
    std::istream& ply_reader::list_property<C,T>::read_binary(std::istream& in,bool swap_endianess)
    {
      C n;
      std::vector<T> values;
      //read count
      in.read((char*)&n,sizeof(C));
    
      if(in && swap_endianess)
      {
        unsigned char *memp = reinterpret_cast<unsigned char*>(&n);
        std::reverse(memp, memp + sizeof(C));
      }
      values.resize((typename std::vector<T>::size_type)n);
  
      in.read((char*)values.data(),sizeof(T)*n);
      swap_endianess = true;
      if(in && swap_endianess)
      {
        for(T& v: values)
        {
          unsigned char *memp = reinterpret_cast<unsigned char*>(&v);
          std::reverse(memp, memp + sizeof(T));
        }
      }
      if(in && on_read)
        on_read(values);
      
      return in;
    }
  
    template <typename T>
    bool ply_reader::element::listen(const std::string& property_name,
      std::function<void(const T& v)> fn)
    {
      std::shared_ptr<ply_property> p = get_property(property_name);
      if(!p)
        return false;
      std::shared_ptr<scalar_property<T>>sp = std::dynamic_pointer_cast<scalar_property<T>>(p);
      if(!sp)
        return false;
      sp->on_read = std::move(fn);
      return true;
    }
  
    template <typename T>
    bool ply_reader::element::listen(const std::string& property_name,
      std::function<void(const std::vector<T>& v)> fn)
    {
      std::shared_ptr<ply_property> p = get_property(property_name);
      if(!p)
        return false;
  
      std::shared_ptr<list_property<std::uint8_t,T>> sp1 = std::dynamic_pointer_cast<list_property<std::uint8_t,T>>(p);
      if(sp1)
      {
        sp1->on_read = std::move(fn);
        return true;
      }
      std::shared_ptr<list_property<std::int8_t,T>> sp2 = std::dynamic_pointer_cast<list_property<std::int8_t,T>>(p);
      if(sp2)
      {
        sp2->on_read = std::move(fn);
        return true;
      }
      std::shared_ptr<list_property<std::uint16_t,T>> sp3 = std::dynamic_pointer_cast<list_property<std::uint16_t,T>>(p);
      if(sp3)
      {
        sp3->on_read = std::move(fn);
        return true;
      }
      std::shared_ptr<list_property<std::int16_t,T>> sp4 = std::dynamic_pointer_cast<list_property<std::int16_t,T>>(p);
      if(sp4)
      {
        sp4->on_read = std::move(fn);
        return true;
      }
      std::shared_ptr<list_property<std::uint32_t,T>> sp5 = std::dynamic_pointer_cast<list_property<std::uint32_t,T>>(p);
      if(sp5)
      {
        sp5->on_read = std::move(fn);
        return true;
      }
      std::shared_ptr<list_property<std::int32_t,T>> sp6 = std::dynamic_pointer_cast<list_property<std::int32_t,T>>(p);
      if(sp6)
      {
        sp6->on_read = std::move(fn);
        return true;
      }
      return false;
    }
  
    template <typename T>
    bool ply_reader::listen_2_element_property(const std::string& element_name,
      const std::string& property_name, std::function<void(const T& v)> fn)
    {
      if(!header_complete)
        return false;
      auto e = get_element(element_name);
      if(!e)
        return false;
  
      return e->listen(property_name, std::move(fn));
    }
  
    template <typename T>
    bool ply_reader::listen_2_element_property(const std::string& element_name,
      const std::string& property_name, std::function<void(const std::vector<T>& v)> fn)
    {
      if(!header_complete)
        return false;
      auto e = get_element(element_name);
      if(!e)
        return false;
  
      return e->listen(property_name,std::move(fn));
    }
  
    template <typename C>
    bool ply_reader::add_list_property( const std::string& value_type, const std::string& name)
    {
      if(elements.empty())
        return false;
  
      if(value_type == "float" || value_type == "float32")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,float>>(name));
        return true;
      }
      if(value_type == "double" || value_type == "float64")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,double>>(name));;
        return true;
      }
      if(value_type == "uint8" || value_type == "uchar")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,std::uint8_t>>(name));
        return true;
      }
      if(value_type == "int8" || value_type == "char")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,std::int8_t>>(name));
        return true;
      }
      if(value_type == "uint16" || value_type == "ushort")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,std::uint16_t>>(name));
        return true;
      }
      if(value_type == "int16" || value_type == "short")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,std::int16_t>>(name));
        return true;
      }
      if(value_type == "uint32" || value_type == "uint")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,std::uint32_t>>(name));
        return true;
      }
      if(value_type == "int32" || value_type == "int")
      {
        elements.back().properties.push_back(std::make_shared<list_property<C,std::int32_t>>(name));;
        return true;
      }
      return false;
    }
  
    
  
    bool create_ply_cube_ascii(const std::string& filename);
  }
}


