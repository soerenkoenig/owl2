
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
#include <memory>

#include "owl/export.hpp"
#include "owl/utils/handle.hpp"
#include "owl/utils/count_iterator.hpp"
#include "owl/utils/range_algorithm.hpp"


namespace owl
{
  namespace utils
  {
    struct OWL_API property_tag{};
  
    template <typename T>
    using property_handle = owl::utils::handle<property_tag>;
  
    class OWL_API property_base
    {
     public:
      property_base(const std::string& name)
        : name(name)
      {}
    
      virtual std::unique_ptr<property_base> clone() const = 0;
  
      std::string name;
    };
  
    template<typename T>
    class property : public property_base
    {
    public:
      property() = default;
      property(const std::string& name)
        : property_base(name)
      {}
    
      property(const property&) = default;
    
      std::unique_ptr<property_base> clone() const override
      {
        return std::unique_ptr<property>(*this);
      }
    
      T value;
    };
  
    class OWL_API property_container
    {
    public:
      property_container() = default;
    
      property_container(const property_container& other)
      {
       properties_.reserve(other.properties_.size());
      
        owl::utils::transform(other.properties_,
          std::back_inserter(properties_),
          [](const std::unique_ptr<property_base>& p)
          {
            return p ? p->clone() : nullptr;
          });
      }
    
      property_container& operator=(const property_container& other)
      {
        if(this == &other)
          return *this;
      
        properties_.clear();
        properties_.reserve(other.properties_.size());
      
        owl::utils::transform(other.properties_,
          std::back_inserter(properties_),
          [](const std::unique_ptr<property_base>& p)
          {
            return p ? p->clone() : nullptr;
          });
        return *this;
      }
    
      template <typename T>
      void add_property(property_handle<T>& p, const std::string& name = "")
      {
        auto it = owl::utils::find(properties_, nullptr);
        if(it == properties_.end())
        {
          properties_.emplace_back(std::make_unique<property<T>>(name));
          p = property_handle<T>{properties_.size()};
        }
        else
        {
          p = property_handle<T>{std::distance(properties_.begin(), it)};
          *it = std::make_unique<property<T>>(name);
        }
      }
    
      template <typename T>
      property_handle<T> get_property(const std::string& name = "") const
      {
        auto it = owl::utils::find_if(properties_, [&name](auto& p)
          {
            return p && p->name == name && dynamic_cast<property<T>>(p.get());
          });
        if(it == properties_.end())
          return property_handle<T>::invalid();
        return { std::distance(properties_.begin(), it) };
      }
    
      template <typename T>
      bool has_property(const std::string& name = "") const
      {
        return get_property<T>(name).is_valid();
      }
    
      template <typename T>
      void remove_property(property_handle<T>& p)
      {
        assert(is_valid<T>(p));
        auto it = properties_.begin() + p.index();
        *it = nullptr;
      
        if(std::all_of(it, properties_.end(), [](auto& prop){ return prop == nullptr;}))
          properties_.erase(it, properties_.end());
      
        p.invalidate();
      }
    
      template <typename T>
      T& operator[](const property_handle<T>& ph)
      {
        return static_cast<property<T>&>(*properties_[ph.index()]).value;
      }
    
      template <typename T>
      const T& operator[](const property_handle<T>& ph) const
      {
        return static_cast<const property<T>&>(*properties_[ph.index()]).value;
      }
    
      template <typename T>
      bool is_valid(const property_handle<T>& ph)
      {
        return ph.is_valid() && ph.index() < properties_.size() && properties_[ph.index()] != nullptr;
      }
    
      void remove_all_properties()
      {
        properties_.clear();
      }
    
    private:
      std::vector<std::unique_ptr<property_base>> properties_;
    };
  
    template <typename T, typename Tag>
    struct indexed_property_handle : public property_handle<T>
    {
      using property_handle<T>::property_handle;
      using handle_type = owl::utils::handle<Tag>;
      using value_type = T;
    };
  
  
    class OWL_API indexed_property_base
    {
    public:
      indexed_property_base(const std::string& name)
        : name(name)
      {}
    
      virtual void reserve(std::size_t n) = 0;
  
      virtual void resize(std::size_t n) = 0;
    
      virtual void move(std::size_t to, std::size_t from) = 0;
    
      virtual std::unique_ptr<indexed_property_base> clone() const = 0;
    
      std::string name;
    
      virtual ~indexed_property_base() {}
    
    };
  
  
    template<typename T>
    class indexed_property : public indexed_property_base
    {
    public:
      indexed_property(const indexed_property&) = default;
    
      indexed_property(const std::string& name, std::size_t n)
        : indexed_property_base(name)
        , values(n, T{})
      {}
    
      void reserve(std::size_t n) override
      {
        values.reserve(n);
      }
    
      void resize(std::size_t n) override
      {
        values.resize(n);
      }
    
      void move(std::size_t to, std::size_t from) override
      {
        values[to] = std::move(values[from]);
      }
    
      std::unique_ptr<indexed_property_base> clone() const override
      {
        return std::make_unique<indexed_property>(*this);
      }

      std::vector<T> values;
    };
  

    template <typename Tag>
    class indexed_property_container
    {
    public:
      template <typename T>
      void add_property(indexed_property_handle<T,Tag>& p, const std::string& name = "")
      {
        auto it = owl::utils::find(properties_, nullptr);
        if(it == properties_.end())
        {
          p = indexed_property_handle<T,Tag>(properties_.size());
          properties_.push_back(std::make_unique<indexed_property<T>>(name, num_elems_));
        }
        else
        {
          p = indexed_property_handle<T,Tag>(std::distance(properties_.begin(), it));
          *it = std::make_unique<indexed_property<T>>(name, num_elems_);
        }
      }
    
      template <typename T>
      void remove_property(indexed_property_handle<T,Tag>& p)
      {
        assert(is_valid(p));
        auto it = properties_.begin() + p.index();
        *it = nullptr;
        if(std::all_of(it, properties_.end(), [](auto& prop){ return prop == nullptr;}))
          properties_.erase(it, properties_.end());
        p.invalidate();
      }
    
      template <typename T>
      indexed_property_handle<T,Tag> get_property(const std::string& name = "") const
      {
        auto it = owl::utils::find_if(properties_, [&name](auto& p)
          {
            return p && p->name == name && dynamic_cast<indexed_property<T>>(p.get());
          });
        if(it == properties_.end())
          return indexed_property_handle<T,Tag>::invalid();
        return { std::distance(properties_.begin(), it) };
      }
    
      template <typename T>
      bool has_property(const std::string& name = "") const
      {
        return get_property<T>(name).is_valid();
      }
    
      template <typename T>
      std::vector<T>& operator[](const indexed_property_handle<T,Tag>& ph)
      {
        return static_cast<indexed_property<T>&>(*properties_[ph.index()]).values;
      }
    
      template <typename T>
      const std::vector<T>& operator[](const indexed_property_handle<T,Tag>& ph) const
      {
        return static_cast<const indexed_property<T>&>(*properties_[ph.index()]).values;
      }
    
      template <typename T>
      bool is_valid(const indexed_property_handle<T,Tag>& ph)
      {
        return ph.is_valid() && ph.index() < properties_.size() && properties_[ph.index()] != nullptr;
      }
    
      void remove_all_properties()
      {
        properties_.clear();
      }
    
      template <typename Func>
      void for_each_property(Func f)
      {
        for(auto& p: properties_)
          if(p)
            f(*p);
      }
    
      void move(std::size_t to, std::size_t from)
      {
        for_each_property([&](auto& p){ p.move(to, from); });
      }
    
      void reserve(std::size_t n)
      {
        for_each_property([&](auto& p){ p.reserve(n); });
      }
    
      void resize(std::size_t n)
      {
        for_each_property([&](auto& p){ p.resize(n); });
         num_elems_ = n;
      }
    
      void clear()
      {
        resize(0);
      }
    
      handle<Tag> add_elem()
      {
        auto h = owl::utils::handle<Tag> {num_elems_};
        resize(num_elems_ + 1);
        return h;
      }
    
      //elem interface
      auto add_elems(std::size_t n)
      {
        owl::utils::handle<Tag> first{num_elems_};
        resize(num_elems_ + n);
        return make_iterator_range(count_iterator<handle<Tag>>(first),
          count_iterator<handle<Tag>>(owl::utils::handle<Tag>(num_elems_)));
      }
    
      indexed_property_container() = default;
    
      indexed_property_container(const indexed_property_container& other)
      {
        *this = other;
      }
    
      indexed_property_container& operator=(const indexed_property_container& other)
      {
        if(this == &other)
          return *this;
        num_elems_ = other.num_elems_;
        properties_.clear();
        properties_.reserve(other.properties_.size());
      
        owl::utils::transform(other.properties_,
          std::back_inserter(properties_),
          [](const std::unique_ptr<indexed_property_base>& p)
          {
            return p ? p->clone() : nullptr;
          });
        return *this;
      }
    
      private:
      std::size_t num_elems_ = 0;

      std::vector<std::unique_ptr<indexed_property_base>> properties_;
    };
  }
}
