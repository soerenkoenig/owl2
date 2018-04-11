#pragma once

#include <vector>
#include "owl/optional.hpp"
#include "owl/color/color.hpp"
#include "owl/color/color_conversion.hpp"

namespace owl
{
  namespace color
  {
    struct color_names
    {
      static std::vector<std::string> names;
      static std::vector<rgb8u> values;

      template<typename Channel>
      static std::optional<rgb<Channel>> lookup(const std::string& name)
      {
        auto it = std::lower_bound(names.begin(), names.end(), name);
        if(*it != name)
          return std::nullopt;
        return convert<rgb<Channel> >(*std::next(values.begin(), std::distance(names.begin(), it)));
      }
    };
  }
}
