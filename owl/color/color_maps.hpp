#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

#include "owl/math/matrix.hpp"
#include "owl/color/color.hpp"
#include "owl/utils/lin_space.hpp"


namespace owl
{
  namespace color
  {  
    namespace colormaps
    {
      /// possible colormaps are
      /// "gray" black .. white
      /// "bone" grayscale colormap with a higher blue component (looks like xray images)
      /// "jet" blue .. cyan .. yellow .. orange .. red
      /// "hot" black .. red .. yellow .. white
      /// "thermo" black .. magenta .. blue.. cyan..green .. yellow ..red.. white
      /// "hsv" red .. yellow .. green .. cyan .. blue .. magenta .. red
    
      //a linear grayscale colormap.
      template <typename Channel, typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
      rgb<Channel> grayscale(Scalar x)
      {
        return rgb<Channel>{channel_traits<Channel>::convert(x),
          channel_traits<Channel>::convert(x),
          channel_traits<Channel>::convert(x)};
      }
    
      //bone
      //is a grayscale colormap with a higher value for the blue component.
      //This colormap is useful for adding an "electronic" look to grayscale images.
      template <typename Channel, typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
      rgb<Channel> bone(Scalar x)
      {
        rgb<Channel> col(0, 0, 0);
        if(x < 3.0f / 4.0f)
          col(0) = channel_traits<Channel>::convert(7.0f / 8.0f * x);
        else
          col(0) = channel_traits<Channel>::convert(11.0f / 8.0f * x - 3.0f / 8.0f);
      
        if(x < 3.0f / 8.0f)
          col(1) = channel_traits<Channel>::convert(7.0f / 8.0f * x);
        else if(x < 3.0f / 4.0f)
          col(1) = channel_traits<Channel>::convert(29.0f / 24.0f * x - 1.0f / 8.0f);
        else
          col(1) = channel_traits<Channel>::convert(7.0f / 8.0f * x + 1.0f / 8.0f);

        if(x < 3.0f / 8.0f)
          col(2) = channel_traits<Channel>::convert(29.0f / 24.0f * x);
        else
          col(2) = channel_traits<Channel>::convert(7.0f / 8.0f * x + 1.0f / 8.0f);
      
        return col;
      }
    
      template <typename Channel, typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
      rgb<Channel> jet(Scalar x)
      {
        rgb<Channel> col{0, 0, 0};

        if((x >= 3.0f / 8.0f) && (x < 5.0f / 8.0f))
          col(0) = channel_traits<Channel>::convert(4.0f * x - 3.0f / 2.0f);
        else if((x >= 5.0f / 8.0f) && (x < 7.0f / 8.0f))
          col(0) = channel_traits<Channel>::convert(1.0f);
        else
          col(0) = channel_traits<Channel>::convert(-4.0f * x + 9.0f / 2.0f);
      
        if(x >= 1.0f / 8.0f && x < 3.0f / 8.0f)
          col(1) = channel_traits<Channel>::convert(4.0f * x - 1.0f / 2.0f);
        else if(x >= 3.0f/8.0f && x < 5.0f / 8.0f)
          col(1) = channel_traits<Channel>::convert(1.0f);
        else if(x >= 5.0f / 8.0f && x < 7.0f / 8.0f)
          col(1) = channel_traits<Channel>::convert(-4.0f * x + 7.0f / 2.0f);
      
        if(x < 1.0f / 8.0f)
          col(2) = channel_traits<Channel>::convert(4.0f * x + 1.0f / 2.0f);
        else if(x >= 1.0f / 8.0f && x < 3.0f / 8.0f)
          col(2) = channel_traits<Channel>::convert(1.0f);
        else if(x >= 3.0f / 8.0f && x < 5.0f / 8.0f)
          col(2) = channel_traits<Channel>::convert(-4.0f * x + 5.0f / 2.0f);
      
        return col;
      }
    
      template <typename Channel, typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
      rgb<Channel> hot(Scalar x)
      {
        rgb<Channel> col{0, 0, 0};

        if(x <= 1.0f / 3.0f)
          col(0) = channel_traits<Channel>::convert(3.0f * x);
        else
          col(0) = channel_traits<Channel>::convert(1.0f);
      
      
        if(x > 1.0f / 3.0f && x < 2.0f / 3.0f)
          col(1) = channel_traits<Channel>::convert(3.0f * x - 1.0f);
        else if(x >= 2.0 / 3.0f)
          col(1) = channel_traits<Channel>::convert(1.0f);
        else if(x >= 2.0f/3.0f)
          col(2) = channel_traits<Channel>::convert(3.0f * x - 2.0f);
      
        return col;
      }
    
    
      template <typename Channel, typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
      rgb<Channel> hsv(Scalar x)
      {
        rgb<Channel> col{0, 0, 0};

        if(x <= 1.0f / 5.0f)
          col(0) = channel_traits<Channel>::convert(1.0f);
        else if(x > 1.0f / 5.0f && x <= 2.0f / 5.0f)
          col(0) = -5.0f * x + 2.0f;
        else if(x > 4.0f / 5.0f)
          col(0) = channel_traits<Channel>::convert(5.0f * x - 4.0f);

        if(x <= 1.0f/5.0f)
          col(1) = channel_traits<Channel>::convert(5.0f * x);
        else if(x > 1.0f/5.0f && x <=3.0f / 5.0f)
          col(1) = channel_traits<Channel>::convert(1.0f);
        else if(x > 3.0f/5.0f && x <= 4.0f / 5.0f)
          col(1) = channel_traits<Channel>::convert(-5.0f * x + 4.0f);

        if(x > 2.0f / 5.0f && x <= 3.0f / 5.0f)
          col(2) = channel_traits<Channel>::convert(5.0f * x - 2.0f);
        else if(x > 3.0f / 5.0f && x <= 4.0f / 5.0f)
          col(2) = channel_traits<Channel>::convert(1.0f);
        else if(x > 4.0f / 5.0f)
          col(2) = channel_traits<Channel>::convert(-5.0f * x + 5.0f);
        return col;
      }
    
      template <typename Channel, typename Scalar, typename = std::enable_if_t<std::is_floating_point<Scalar>::value>>
      rgb<Channel> thermo(Scalar x)
      {
        rgb<Channel> col{0, 0, 0};
    
        if(x <= 1.0f / 7.0f)
          col(0) =  channel_traits<Channel>::convert(7.0f * x);
        else if(x > 1.0f / 7.0f && x <= 2.0f / 7.0f)
          col(0) =  channel_traits<Channel>::convert(-7.0f * x + 2.0f);
        if(x > 4.0f / 7.0f && x <= 5.0f / 7.0f)
          col(0) = channel_traits<Channel>::convert(7.0f * x - 4.0f);
        if(x > 5.0f / 7.0f)
          col(0) = channel_traits<Channel>::convert(1.0f);

        if(x >= 2.0f / 7.0f && x <= 3.0f / 7.0f)
          col(1) = channel_traits<Channel>::convert(7.0f * x - 2.0f);
        else if(x > 3.0f / 7.0f && x <= 5.0f / 7.0f)
          col(1) = channel_traits<Channel>::convert(1.0f);
        else if(x > 5.0f / 7.0f && x <= 6.0f / 7.0f)
          col(1) = channel_traits<Channel>::convert(-7.0f * x + 6.0f);
        else if(x > 6.0f/7.0f)
          col(1) = channel_traits<Channel>::convert(7.0f * x - 6.0f);

        if(x <= 1.0f / 7.0f)
          col(2) = channel_traits<Channel>::convert(7.0f * x);
        if(x > 1.0f / 7.0f && x <= 3.0f / 7.0f)
          col(2) = channel_traits<Channel>::convert(1.0f);
        if(x > 3.0f / 7.0f && x <= 4.0f / 7.0f)
          col(2) = channel_traits<Channel>::convert(-7.0f * x + 4.0f);
        if(x > 6.0f/7.0f)
          col(2) = channel_traits<Channel>::convert(7.0f * x - 6.0f);
        return col;
      }
    }

    enum class colormap
    {
      gray, bone, jet, hot, hsv, thermo
    };
  
    template <typename Channel>
    std::optional<std::vector<rgb<Channel>>> create_color_map(std::size_t steps = 256, colormap name = colormap::jet)
    {
      using color_type = rgb<Channel>;
    
      std::vector<color_type> map;
      map.resize(steps);
    
      std::vector<float> x(steps);
      utils::lin_space(x.begin(),x.end(), 0, 1);
    
        for(std::size_t i = 0; i < steps; i++)
        {
        switch(name)
        {
        case colormap::gray:
          map[i] = colormaps::grayscale<Channel>(x[i]);
          break;
        case colormap::bone:
          map[i] = colormaps::bone<Channel>(x[i]);
          break;
        case colormap::jet:
          map[i] = colormaps::jet<Channel>(x[i]);
          break;
        case colormap::hot:
          map[i] = colormaps::hot<Channel>(x[i]);
          break;
        case colormap::hsv:
          map[i] = colormaps::hsv<Channel>(x[i]);
          break;
        case colormap::thermo:
          map[i] = colormaps::thermo<Channel>(x[i]);
          break;
        default:
          return std::nullopt;
        }
      }
      return map;
    }
    

  }
}




