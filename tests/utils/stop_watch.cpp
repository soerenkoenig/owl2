#include <sstream>
#include <thread>
#include <chrono>
#include "owl/utils/stop_watch.hpp"
#include "catch/catch.hpp"

namespace test
{
  TEST_CASE( "stop_watch", "[utils]")
  {

    using namespace owl::utils;
    using namespace std::chrono_literals;

    auto x = stop_watch();
    stop_watch s;
    CHECK_FALSE(s.is_running());

    s.start();
    CHECK(s.is_running());
    std::this_thread::sleep_for(0.25s);

    CHECK(Approx(s.elapsed_time()).margin(0.07) == 0.25);
    CHECK(Approx(s.elapsed_time(std::milli{})).margin(7) == 250);
    std::this_thread::sleep_for(0.25s);
    s.stop();
    CHECK_FALSE(s.is_running());

    CHECK(Approx(s.elapsed_time()).margin(0.05) == 0.5);
    CHECK(Approx(s.elapsed_time()*1000) == s.elapsed_time(std::milli{}));

    s.start();
    CHECK(s.is_running());
    std::this_thread::sleep_for(0.25s);
    CHECK(Approx(s.current_elapsed_time()).margin(0.05) == 0.25);
    CHECK(Approx(s.elapsed_time()).epsilon(0.05) == 0.75);

    s.restart();
    std::this_thread::sleep_for(0.25s);
    CHECK(Approx(s.current_elapsed_time()).margin(0.05) == 0.25);
    CHECK(Approx(s.elapsed_time()).epsilon(0.05) == 0.25);
  }
}


