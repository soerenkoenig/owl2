
//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include "owl/export.hpp"

namespace owl
{
  namespace utils
  {
    struct OWL_API non_copyable
    {
    protected:
      non_copyable() = default;
      ~non_copyable() = default;

      non_copyable(const non_copyable&) = delete;
      non_copyable& operator=(const non_copyable&) = delete;
    };
  }
}
