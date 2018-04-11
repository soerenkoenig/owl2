//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2018 Sören König. All rights reserved.
//

#pragma once

#include <chrono>
#include <ratio>

#include "owl/export.hpp"

namespace owl
{
  namespace utils
  {
    /**
     * A stop watch class to make time measurements using std::chrono_high_resolution_clock as internal clock.
     */
    class OWL_API stop_watch
    {
    public:
      /**
       * clock_type of internal clock used to do time measurements
       */
      using clock_type = std::chrono::high_resolution_clock;
      /**
       * duration type of internal clock
       */
      using duration = clock_type::duration;
      /**
       * time_point type of internal clock
       */
      using time_point = clock_type::time_point;

      /**
       * Create a stop_watch and calls reset()
       */
      stop_watch();

      /**
       * Start time measurement.
       */
      void start();

      /**
       * Stop time measurement.
       */
      void stop();

      /**
       * @return true if stop watch is running
       */
      bool is_running() const;

      /**
       * Stop time measurement and set elapsed time to 0.0.
       */
      void reset();

      /**
       *  A shortcut to call reset() and start()
       */
      void restart();

      /**
       * Get the elapsed time since last call to start() in given Ratio of a second,
       * e.g. current_elapsed_time(std::milli{}) would return result in milli seconds
       * @tparam Ratio ratio of a second default is a ratio of 1
       * @return elapsed measured time in given Ratio of seconds since last call of start or restart,
       * 0.0 otherwise.
       */
      template<typename Ratio = std::ratio<1>>
      double current_elapsed_time(Ratio = {}) const
      {
        return std::chrono::duration_cast<std::chrono::duration<double, Ratio>>(current_elapsed_duration()).count();
      }

      /**
       * @return (accumulated) elapsed measured time in given ratio of seconds
       * e.g. elapsed_time(std::milli{}) would return result in milli seconds
       */
      template<typename Ratio = std::ratio<1>>
      double elapsed_time(Ratio = {}) const
      {
        return std::chrono::duration_cast<std::chrono::duration<double, Ratio>>(elapsed_duration()).count();
      }

    private:
      duration current_elapsed_duration() const;

      duration elapsed_duration() const;

      time_point last_start_;

      duration elapsed_time_;

      bool running_;
    };
  }
}




