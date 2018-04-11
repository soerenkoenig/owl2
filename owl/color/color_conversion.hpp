
//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include "owl/color/color.hpp"

namespace owl
{
  namespace color
  {
    template <typename ColorDestination, typename ColorSource>
    ColorDestination convert(const ColorSource& col);
  
  namespace detail
  {
    template <typename ColorDestination, typename ColorSource>
    struct color_conversion;
  
    template <typename T1, typename T2>
    struct color_conversion<rgb<T2,true>, rgb<T1,true>>
    {
      static rgb<T2,true> convert(const rgb<T1,true>& col)
      {
        return rgb<T2,true> {channel_traits<T2>::convert(col(0)),
          channel_traits<T2>::convert(col(1)),
          channel_traits<T2>::convert(col(2)),
          channel_traits<T2>::convert(col(3))
        };
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<rgb<T2>, rgb<T1>>
    {
      static rgb<T2> convert(const rgb<T1>& col)
      {
        return rgb<T2> {channel_traits<T2>::convert(col(0)),
          channel_traits<T2>::convert(col(1)),
          channel_traits<T2>::convert(col(2))};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<bgr<T2>, bgr<T1>>
    {
      static bgr<T2> convert(const bgr<T1>& col)
      {
        return bgr<T2> {channel_traits<T2>::convert(col(0)),
          channel_traits<T2>::convert(col(1)),
          channel_traits<T2>::convert(col(2))};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<hsv<T2>, hsv<T1>>
    {
      static hsv<T2> convert(const hsv<T1>& col)
      {
        return hsv<T2> {channel_traits<T2>::convert(col(0)),
          channel_traits<T2>::convert(col(1)),
          channel_traits<T2>::convert(col(2))};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<cmyk<T2>, cmyk<T1>>
    {
      static cmyk<T2> convert(const cmyk<T1>& col)
      {
        return cmyk<T2> {channel_traits<T2>::convert(col(0)),
          channel_traits<T2>::convert(col(1)),
          channel_traits<T2>::convert(col(2)),
          channel_traits<T2>::convert(col(3))};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<rgb<T2>, cmyk<T1>>
    {
      static rgb<T2> convert(const cmyk<T1>& col)
      {
         double one_minus_k = 1.0 - channel_traits<double>::convert(col.k());
         double r = (1.0 - channel_traits<double>::convert(col.c())) * one_minus_k;
         double g = (1.0 - channel_traits<double>::convert(col.c())) * one_minus_k;
         double b = (1.0 - channel_traits<double>::convert(col.c())) * one_minus_k;
         return { channel_traits<T2>::convert(r),
          channel_traits<T2>::convert(g),
          channel_traits<T2>::convert(b) };
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<cmyk<T2>, rgb<T1>>
    {
      static cmyk<T2> convert(const rgb<T1>& col)
      {
        double r = channel_traits<double>::convert(col.r());
        double g = channel_traits<double>::convert(col.g());
        double b = channel_traits<double>::convert(col.b());
      
        double one_minus_k = std::max({ r, g, b });
        T2 zero = channel_traits<T2>::convert(0.0);
        T2 one = channel_traits<T2>::convert(1.0);
        if(one_minus_k == 0)
          return {zero, zero, zero, one};
      
        double K  = 1.0 - one_minus_k;
        double C = (one_minus_k - col.r())/one_minus_k;
        double M = (one_minus_k - col.g())/one_minus_k;
        double Y = (one_minus_k - col.b())/one_minus_k;
      
        return {channel_traits<T2>::convert(C),
         channel_traits<T2>::convert(M),
         channel_traits<T2>::convert(Y),
         channel_traits<T2>::convert(K)};
      }
    };
  
  template <typename T1, typename T2>
    struct color_conversion<bgr<T2>, cmyk<T1>>
    {
      static bgr<T2> convert(const cmyk<T1>& col)
      {
         double one_minus_k = 1.0 - channel_traits<double>::convert(col.k());
         double r = (1.0 - channel_traits<double>::convert(col.c())) * one_minus_k;
         double g = (1.0 - channel_traits<double>::convert(col.c())) * one_minus_k;
         double b = (1.0 - channel_traits<double>::convert(col.c())) * one_minus_k;
         return { channel_traits<T2>::convert(b),
          channel_traits<T2>::convert(g),
          channel_traits<T2>::convert(r) };
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<cmyk<T2>, bgr<T1>>
    {
      static cmyk<T2> convert(const bgr<T1>& col)
      {
        double r = channel_traits<double>::convert(col.r());
        double g = channel_traits<double>::convert(col.g());
        double b = channel_traits<double>::convert(col.b());
      
        double one_minus_k = std::max({ r, g, b });
        T2 zero = channel_traits<T2>::convert(0.0);
        T2 one = channel_traits<T2>::convert(1.0);
        if(one_minus_k == 0)
          return {zero, zero, zero, one};
      
        double K  = 1.0 - one_minus_k;
        double C = (one_minus_k - col.r())/one_minus_k;
        double M = (one_minus_k - col.g())/one_minus_k;
        double Y = (one_minus_k - col.b())/one_minus_k;
      
        return {channel_traits<T2>::convert(C),
         channel_traits<T2>::convert(M),
         channel_traits<T2>::convert(Y),
         channel_traits<T2>::convert(K)};
      }
    };
  
  
    template <typename T1, typename T2>
    struct color_conversion<rgb<T2>, hsv<T1>>
    {
      static rgb<T2> convert(const hsv<T1>& col)
      {
        double hue = channel_traits<double>::convert(col.h);
        double sat = channel_traits<double>::convert(col.s);
        double val = channel_traits<double>::convert(col.v);

        double x = 0.0, y = 0.0, z = 0.0;
      
        if(hue == 1)
          hue = 0;
        else
          hue *= 6;

        int i = static_cast<int>(std::floor(hue));
        double f = hue - i;
        double p = val * ( 1 - sat );
        double q = val * ( 1 - ( sat * f ) );
        double t = val * ( 1 - ( sat * ( 1 - f ) ) );

        switch(i)
        {
        case 0:
          x = val; y = t; z = p;
          break;
        case 1:
          x = q; y = val; z = p;
          break;
        case 2:
          x = p; y = val; z = t;
          break;
        case 3:
          x = p; y = q; z = val;
          break;
        case 4:
          x = t; y = p; z = val;
          break;
        case 5:
          x = val; y = p; z = q;
          break;
        }
        return rgb<T2>(channel_traits<T2>::convert(x),
         channel_traits<T2>::convert(y),
         channel_traits<T2>::convert(z));
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<hsv<T2>, rgb<T1>>
    {
      static hsv<T2> convert(const rgb<T1>& col)
      {
        double x = channel_traits<double>::convert(col.r());
        double y = channel_traits<double>::convert(col.g());
        double z = channel_traits<double>::convert(col.b());

        double max = (x > y) ? ((x > z) ? x : z) : ((y > z) ? y : z);
        double min = (x < y) ? ((x < z) ? x : z) : ((y < z) ? y : z);
        double range = max - min;
        double val = max;
        double sat = 0;
        double hue = 0;
  
        if(max != 0)
          sat = range / max;
    
        if(sat != 0)
        {
          double h;

          if(x == max)
            h = (y - z) / range;
          else if( y == max )
            h = 2.0 + (z - x) / range;
          else
            h = 4.0 + (x - y) / range;

          hue = h / 6.0;

          if(hue < 0.0)
            hue += 1.0;
        }
        return {channel_traits<T2>::convert(hue),
         channel_traits<T2>::convert(sat),
         channel_traits<T2>::convert(val)};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<rgb<T2>, bgr<T1>>
    {
      static rgb<T2> convert(const bgr<T1>& col)
      {
        return {channel_traits<T2>::convert(col.r()),
         channel_traits<T2>::convert(col.g()),
         channel_traits<T2>::convert(col.b())};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<rgb<T2>, gray<T1>>
    {
      static rgb<T2> convert(const gray<T1>& col)
      {
        T2 v = channel_traits<T2>::convert(col);
        return {v, v, v};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<bgr<T1>, gray<T2>>
    {
      static bgr<T2> convert(const gray<T1>& col)
      {
        T2 v = channel_traits<T2>::convert(col);
        return {v, v, v};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<bgr<T2>, rgb<T1>>
    {
      static bgr<T2> convert(const rgb<T1>& col)
      {
        return {channel_traits<T2>::convert(col.b()),
         channel_traits<T2>::convert(col.g()),
         channel_traits<T2>::convert(col.r())};
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<gray<T2>, rgb<T1>>
    {
      static gray<T2> convert(const rgb<T1>& col)
      {
        rgb<double> cold = owl::color::convert<rgb<double>>(col);
        return convert<gray<T2>(0.2989 * cold.r() + 0.5870 * cold.g() + 0.1140 * cold.b());
      }
    };
  
    template <typename T1, typename T2>
    struct color_conversion<gray<T2>, bgr<T1>>
    {
      static gray<T2> convert(const bgr<T1>& col)
      {
        bgr<double> cold = owl::color::convert<bgr<double>>(col);
        return convert<gray<T2>(0.2989 * cold.r() + 0.5870 * cold.g() + 0.1140 * cold.b());
      }
    };
  
  
  
   /* template <typename T>
    struct color_conversion<rgba<T>, bgra<T>>
    {
      static rgba<T> convert(const bgra<T>& col)
      {
        return {col.r(), col.g(), col.b(), col.a()};
      }
    };
  
    template <typename T>
    struct color_conversion<bgra<T>, rgba<T>>
    {
      static bgra<T> convert(const rgba<T>& col)
      {
      return {col.b(), col.g(), col.r(), col.a()};
      }
    };*/

  }
  
  //strait rgba <-> premultiplied rgba
  //(r,g,b,a) <-> (ar, ag, ab, a);
  
  
    template <typename ColorDestination, typename ColorSource>
    ColorDestination convert(const ColorSource& col)
    {
      return detail::color_conversion<ColorDestination, ColorSource>::convert(col);
    }
  }
}
