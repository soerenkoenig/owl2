
//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <vector>
#include <string>
#include <initializer_list>
#include <algorithm>
#include <iterator>

#include "owl/export.hpp"

namespace owl
{
  namespace utils
  {
    class OWL_API buffer
    {
     public:
    
      using value_type = std::uint8_t;
      using reference = std::uint8_t&;
	    using const_reference = const std::uint8_t&;
#ifdef WIN32
	    using const_iterator = stdext::checked_array_iterator<const std::uint8_t*>;
      using iterator = stdext::checked_array_iterator<std::uint8_t*>;
#else
	    using const_iterator = const std::uint8_t*;
      using iterator = std::uint8_t*;
#endif
      using size_type = std::size_t;
      using pointer = std::uint8_t*;
      using const_pointer = const std::uint8_t*;
    
      buffer();
    
      buffer(std::initializer_list<std::uint8_t> il);
     
      buffer(size_type size);
     
      buffer(void *data, size_t size, bool copy = false);
  
      buffer(const buffer &other);
  
      buffer(buffer &&other);
    
      template <typename Iterator>
      buffer(Iterator first, Iterator one_past_last)
      {
        size_type elem_size = sizeof(decltype(*first));
        size_ = std::distance(first, one_past_last) * elem_size;
        data_ = static_cast<std::uint8_t*>(malloc(size_));
        is_owner_ = true;
        auto it = begin();
        while(first != one_past_last)
        {
         memcpy(it, first, elem_size);
         it += elem_size;
         ++first;
        }
      }
        

      buffer& operator=(const buffer &rhs);

      buffer& operator=(buffer &&rhs);

      ~buffer();
      
      size_type size() const;
      
      pointer data();
   
      const_pointer data() const;

      void resize(size_type new_size);
        
      void append(const buffer& buf); 
    
      iterator begin();
    
      const_iterator begin() const;
    
      const_iterator cbegin() const;
    
      iterator end();
    
      const_iterator end() const;
    
      const_iterator cend() const;
    
      buffer sub_buffer(size_type offset, size_type length);
      
      bool is_owner() const;
  
      reference operator[](size_type i);
      
      const_reference operator[](size_type i) const;
    
      bool operator==(const buffer& other) const;
    
      bool operator!=(const buffer& other) const;
    
      bool operator<(const buffer& other) const;
    
      bool operator<=(const buffer& other) const;
    
      bool operator>(const buffer& other) const;
    
      bool operator>=(const buffer& other) const;   
    
   private:
    
      pointer  data_;
      size_type size_;
      bool   is_owner_;
    };
  }
}

