#include "owl/utils/stop_watch.hpp"


namespace owl
{
  namespace utils
  {
    stop_watch::stop_watch()
    {
      reset();
    }

    void stop_watch::reset()
    {
      elapsed_time_ = duration::zero();
      running_ = false;
    }

    void stop_watch::restart()
    {
      elapsed_time_ = duration::zero();
      start();
    }

    stop_watch::duration stop_watch::current_elapsed_duration() const
    {
      return is_running() ? clock_type::now() - last_start_ : duration::zero();
    }

    stop_watch::duration stop_watch::elapsed_duration() const
    {
      return is_running() ? elapsed_time_ + current_elapsed_duration() : elapsed_time_;
    }

    void stop_watch::start()
    {
      running_ = true;
      last_start_ = clock_type::now();
    }

    void stop_watch::stop()
    {
      elapsed_time_ += clock_type::now() - last_start_;
      running_ = false;
    }

    bool stop_watch::is_running() const
    {
      return running_;
    }

  }
}

