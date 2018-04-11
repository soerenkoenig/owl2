//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright (c) 2018 Sören König. All rights reserved.
//

#pragma once

#if __has_include(<variant>)
#include <variant>
#else
#include "thirdparty/variant/variant.hpp"
namespace std
{
  using namespace mpark;
}
#endif

