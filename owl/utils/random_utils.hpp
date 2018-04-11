
//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once

#include <random>

namespace owl
{
  namespace utils
  {
    template<class Engine = std::mt19937, std::size_t N = Engine::state_size>
    auto create_seeded_engine() -> typename std::enable_if_t<!!N, Engine>
    {
      typename Engine::result_type random_data[N];
      std::random_device source;
      std::generate(std::begin(random_data), std::end(random_data), std::ref(source));
      std::seed_seq seeds(std::begin(random_data), std::end(random_data));
      return Engine(seeds);
    }
  }
}
