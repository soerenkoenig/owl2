//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2018 Sören König. All rights reserved.
//

#pragma once

#include <vector>

#include "owl/utils/handle.hpp"
#include "owl/utils/count_iterator.hpp"
#include "owl/utils/map_iterator.hpp"
#include "owl/utils/range_algorithm.hpp"
#include "owl/color/color.hpp"
#include "owl/math/interval.hpp"
#include "owl/math/triangle.hpp"

namespace owl
{
  namespace image
  {
    struct pixel_tag{};
    using pixel_handle = owl::utils::handle<pixel_tag>;

    struct row_tag{};
    using row_handle = owl::utils::handle<row_tag>;

    struct column_tag{};
    using column_handle = owl::utils::handle<column_tag>;

    template <typename Color>
    class image
    {
    public:
      using color_type = Color;
      using iterator = typename std::vector<color_type>::iterator;
      using const_iterator = typename std::vector<color_type>::const_iterator;
      using size_type = typename std::vector<color_type>::size_type;
      using pointer = Color*;
      using const_pointer = const Color*;
      using value_type = Color;
      using reference = Color&;

      template <typename Range>
      using is_pixel_handle_range =  std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, pixel_handle>;

      template <typename Range>
      using is_row_handle_range =  std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, row_handle>;

      template <typename Range>
      using is_column_handle_range =  std::is_same<typename utils::container_traits<std::decay_t<Range>>::value_type, column_handle>;


      image() = default;

      image(size_type w, size_type h, color_type c = {})
        : width_{w}
        , height_{h}
        , data_{w * h, c}
      {
      }

      void resize(size_type w, size_type h, color_type c = {})
      {
        width_ = w;
        height_ = h;
        data_.resize(w * h, c);
      }

      pointer data()
      {
        return data_.data();
      }

      const_pointer data() const
      {
        return data_.data();
      }

      iterator begin()
      {
        return data_.begin();
      }

      iterator end()
      {
        return data_.end();
      }

      const_iterator cbegin() const
      {
        return data_.cbegin();
      }

      const_iterator cend() const
      {
        return data_.cend();
      }

      const_iterator begin() const
      {
        return data_.begin();
      }

      const_iterator end() const
      {
        return data_.end();
      }

      size_type size() const
      {
        return data_.size();
      }

      const size_type& width() const
      {
        return width_;
      }

      const size_type& height() const
      {
        return height_;
      }

      color_type& color(pixel_handle p)
      {
        return data_[p.index()];
      }

      const color_type& color(pixel_handle p) const
      {
        return data_[p.index()];
      }

      template <typename PixelHandleRange, typename = std::enable_if_t<is_pixel_handle_range<PixelHandleRange>::value>>
      auto colors(PixelHandleRange&& pixels) const
      {
        return utils::map_range([this](pixel_handle p)->const auto&{ return color(p); },
                                std::forward<PixelHandleRange>(pixels));
      }

      template <typename PixelHandleRange, typename = std::enable_if_t<is_pixel_handle_range<PixelHandleRange>::value>>
      auto colors(PixelHandleRange&& pixels)
      {
        return utils::map_range([this](pixel_handle p)->auto&{ return color(p); },
                                std::forward<PixelHandleRange>(pixels));
      }

      pixel_handle pixel(size_type col_x, size_type row_y) const
      {
        return pixel_handle(row_y * width_ + col_x);
      }

      pixel_handle pixel(const math::vector<size_type,2>& pos) const
      {
        return pixel_handle(pos.y() * width_ + pos.x());
      }

      std::pair<column_handle, row_handle> position(pixel_handle p) const
      {
        return {column(p), row(p)};
      }

      column_handle column(pixel_handle p) const
      {
        return column_handle(p.index() % width_);
      }

      column_handle column(std::size_t x) const
      {
        return x < num_columns() ? column_handle(x) : column_handle::invalid();
      }

      row_handle row(pixel_handle p) const
      {
        return row_handle(p.index() / width_);
      }

      row_handle row(std::size_t y) const
      {
        return y < num_rows() ? row_handle(y) : row_handle::invalid();
      }

      bool is_inside(size_type x, size_type y) const
      {
        return (x < width_ && y < height_);
      }

      std::size_t num_pixels() const
      {
        return width_ * height_;
      }

      auto pixels() const
      {
        return utils::make_counting_range(pixel_handle(0), pixel_handle(num_pixels()));
      }

      std::size_t num_rows() const
      {
        return height_;
      }

      std::size_t num_columns() const
      {
        return width_;
      }

      auto rows() const
      {
        return utils::make_counting_range(row_handle(0), row_handle(num_rows()));
      }

      auto columns() const
      {
        return utils::make_counting_range(column_handle(0), column_handle(num_columns()));
      }

      auto pixels(row_handle r) const
      {
        return utils::make_counting_range(pixel_handle(r.index() * width_), pixel_handle((r.index() + 1) * width_));
      }

      auto pixels(column_handle c) const
      {
        return utils::make_handle_iterator_range(pixel_handle(c.index()), pixel_handle(c.index() + num_pixels()),
                                                 [this](pixel_handle p){ return pixel_handle(p.index() + width_); });
      }

      math::rectangle<std::size_t> region(std::size_t x, std::size_t y, std::size_t w, std::size_t h)
      {
        return math::rectangle<std::size_t>(math::vector<std::size_t,2>(x,y), math::vector<std::size_t,2>(x+w,y+h));
      }



      auto pixels(const math::rectangle<std::size_t>& region) const
      {
        struct region_stepper
        {
          pixel_handle operator()(pixel_handle p)
          {
            if(p == x_end_)
            {
              x_end_ += large_step_;
              return x_begin_ += large_step_;
            }
            return ++p;
          }
          pixel_handle x_begin_;
          pixel_handle x_end_;
          pixel_handle::difference_type large_step_;
        };


        pixel_handle first = pixel(region.lower_bound);
        return utils::make_handle_iterator_range(first,pixel(region.lower_bound.x(), region.upper_bound.y() + 1),
                                                 region_stepper{first, pixel(region.upper_bound.x(), region.lower_bound.y()), width_});
      }

      /*
      template<typename Scalar>
      auto pixels(const math::triangle<Scalar,2>& triangle) const
      {

      }*/

    private:
      size_type width_;
      size_type height_;
      std::vector<color_type> data_;
    };

    using rgb8u_image = image<owl::color::rgb8u>;


    inline rgb8u_image create_grid(std::size_t n_x, std::size_t n_y, std::size_t spacing = 4)
    {
      std::size_t w = n_x + (n_x - 1) * spacing;
      std::size_t h = n_y + (n_y - 1) * spacing;
      rgb8u_image img(w,h);

      owl::utils::fill(img.colors(img.pixels()), color::rgb8u(255,255,255));

      for(std::size_t x = 0; x < w; x += spacing+1)
        utils::fill(img.colors(img.pixels(img.column(x))), color::rgb8u(0,0,0));

      for(std::size_t y = 0; y < h; y += spacing+1)
        utils::fill(img.colors(img.pixels(img.row(y))), color::rgb8u(0,0,0));

      for(std::size_t i = 0; i < 4; ++i)
      {
        utils::fill(img.colors(img.pixels(img.column(i))), color::rgb8u(50,50,50));
        utils::fill(img.colors(img.pixels(img.column(w-1-i))), color::rgb8u(50,50,50));
        utils::fill(img.colors(img.pixels(img.row(i))), color::rgb8u(50,50,50));
        utils::fill(img.colors(img.pixels(img.row(h-1-i))), color::rgb8u(50,50,50));
        utils::fill(img.colors(img.pixels(img.column(w/2 -2+i))), color::rgb8u(50,50,50));
        utils::fill(img.colors(img.pixels(img.row(h/2-2+i))), color::rgb8u(50,50,50));
      }

      // utils::fill(img.colors(img.pixels(img.region(10,10,30,30))), color::rgb8u(50,50,50));

      return img;
    }


    inline rgb8u_image create_grid_color(std::size_t n_x, std::size_t n_y, std::size_t spacing = 4, const  color::rgb8u& x_color = {255, 100, 100}, const color::rgb8u& y_color = {100, 100, 255})
    {
      std::size_t w = n_x + (n_x - 1) * spacing;
      std::size_t h = n_y + (n_y - 1) * spacing;
      rgb8u_image img(w, h);

      owl::utils::fill(img.colors(img.pixels()), color::rgb8u(155,155,155));

      for(std::size_t x = w/2 -2; x < w/2+3; ++x)
        owl::utils::fill(img.colors(img.pixels(img.column(x))), y_color);

      for(std::size_t y = h/2 -3; y < h/2+3; ++y)
        owl::utils::fill(img.colors(img.pixels(img.row(y))), x_color);

      return img;
    }


  }
}




