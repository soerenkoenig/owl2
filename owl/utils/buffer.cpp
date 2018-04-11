
#include "owl/utils/buffer.hpp"


//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//


namespace owl
{
  namespace utils
  {
    
    buffer::buffer()
      : data_(nullptr) , size_(0), is_owner_(false)
    {
    }
  
    buffer::buffer(std::initializer_list<std::uint8_t> il)
    {
    
        size_ = il.size();
        data_ = static_cast<std::uint8_t*>(malloc(size_));
        is_owner_ = true;
        std::copy(il.begin(),il.end(),begin());
    }
      
    buffer::buffer(void *data, size_t n, bool copy)
    {
      size_ = n;
      if(copy)
      {
        data_ = static_cast<std::uint8_t*>(malloc(size_));
        memcpy(data_, data, n);
        is_owner_ = true;
      }
      else
      {
        data_ = static_cast<std::uint8_t*>(data);
        is_owner_ = false;
      }
    }
             
    buffer::buffer(size_t n)
      : data_(static_cast<std::uint8_t*>(malloc(n))), size_(n), is_owner_(true)
    {
    }
             
    buffer::buffer(const buffer &other)
      : data_(static_cast<std::uint8_t*>(malloc(other.size_))), size_(other.size_), is_owner_(true)
    {
      memcpy(data_, other.data_, other.size_);
    }
    
    buffer::buffer(buffer &&other)
      : data_(other.data_), size_(other.size_), is_owner_(other.is_owner_)
    {
      other.is_owner_ = false;
    }
             
    buffer&  buffer::operator=(const buffer &other)
    {
      size_ = other.size_;
      data_ = static_cast<std::uint8_t*>(malloc(size_));
      memcpy( data_, other.data_, size_ );

      is_owner_ = true;

      return *this;
    }
             
    buffer&  buffer::operator=(buffer &&other)
    {
      data_ = other.data_;
      size_ = other.size_;
      is_owner_ = other.is_owner_;
      other.is_owner_ = false;

      return *this;
    }
      
    buffer::~buffer()
    {
      if(is_owner_)
        free(data_);
    }
  
    buffer::size_type buffer::size() const { return size_; }
  
    buffer::pointer buffer::data() { return data_; }
  
    buffer::const_pointer buffer::data() const { return data_; }
  
    buffer::iterator buffer::begin()
    {
#ifdef WIN32
	    return stdext::make_checked_array_iterator(data_, size_);
#else
	    return data_;
#endif
    }
  
    buffer::const_iterator buffer::begin() const
  	{
#ifdef WIN32
	    return stdext::make_checked_array_iterator<const std::uint8_t*>(data_, size_);
#else
	    return data_;
#endif
	  }
  
    buffer::const_iterator buffer::cbegin() const
    {
#ifdef WIN32
  	  return stdext::make_checked_array_iterator<const std::uint8_t*>(data_, size_);
#else
	    return data_;
#endif
    }
  
    buffer::iterator buffer::end()
    {
#ifdef WIN32
	  return stdext::make_checked_array_iterator(data_, size_, size_);
#else
	  return data_ +size_;
#endif
    }
  
    buffer::const_iterator buffer::end() const
    {
#ifdef WIN32
	  return stdext::make_checked_array_iterator<const std::uint8_t*>(data_, size_, size_);
#else
	  return data_ + size_;
#endif
    }
  
    buffer::const_iterator buffer::cend() const
    {
#ifdef WIN32
	    return stdext::make_checked_array_iterator<const std::uint8_t*>(data_, size_, size_);
#else
	    return data_ + size_;
#endif
    }
  
    buffer buffer::sub_buffer(size_t offset, size_t length)
    {
      return buffer(data_ + offset, length);
    }
  
    bool buffer::is_owner() const
    {
      return is_owner_;
    }

    std::uint8_t& buffer::operator[](std::size_t i)
    {
      return data_[i];
    }

    const std::uint8_t& buffer::operator[](std::size_t i) const
    {
      return data_[i];
    }
    
    void buffer::resize(size_t new_size)
    {
      if(is_owner_)
        data_ = static_cast<std::uint8_t*>(realloc(data_, new_size));
      else
      {
        std::uint8_t *data_new = static_cast<std::uint8_t*>(malloc(new_size));
        memcpy(data_new, data_, size_);
        data_ = data_new;
        is_owner_ = true;
      }
      
      size_ = new_size;
    }
      
    void buffer::append(const buffer& buf)
    {
        auto size_old = size_;
        resize(size_ + buf.size());
        memcpy(data_ + size_old, buf.data(), buf.size());
    }
  
    bool buffer::operator==(const buffer& other) const
    {
      if(size() != other.size())
        return false;
      return std::equal(begin(), end(), other.begin());
    }
  
    bool buffer::operator!=(const buffer& other) const
    {
      return !(*this == other);
    }
  
    bool buffer::operator<(const buffer& other) const
    {
      return std::lexicographical_compare(begin(),end(),other.begin(),other.end());
    }
  
    bool buffer::operator<=(const buffer& other) const
    {
      return other > *this;
    }
  
    bool buffer::operator>(const buffer& other) const
    {
      return other <= *this;
    }
  
    bool buffer::operator>=(const buffer& other) const
    {
      return other < *this;
    }
    
  }
}


