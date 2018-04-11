//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright (c) 2018 Sören König. All rights reserved.
//

#pragma once

#ifdef _WIN32
#ifdef owl_EXPORTS
#define OWL_API __declspec(dllexport)
#else
#define OWL_API __declspec(dllimport)
#endif
#else
#define OWL_API
#endif



