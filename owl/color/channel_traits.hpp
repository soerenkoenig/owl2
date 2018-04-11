//
//           .___.
//           {o,o}
//          ./)_)
//      owl --"-"---
//
//  Copyright © 2017 Sören König. All rights reserved.
//

#pragma once
namespace owl
{
  namespace color
  {
    template<typename T> class channel_traits
    {
    public:
      static constexpr bool is_specialized = false;
 
      static constexpr T min() noexcept { return T(); }
      static constexpr T max() noexcept { return T(); }
    };

    template<> class channel_traits<std::uint8_t>
    {
    public:
      static constexpr bool is_specialized = true;
 
      static constexpr std::uint8_t min() noexcept { return 0; }
      static constexpr std::uint8_t max() noexcept { return 255; }
      static constexpr std::uint8_t convert(std::uint8_t v) { return v; }
      static constexpr std::uint8_t convert(std::uint16_t v) { return static_cast<std::uint8_t>( v / 257 ); }
      static constexpr std::uint8_t convert(float v){ return std::clamp(v, 0.0f, 1.0f) * 255; }
      static constexpr std::uint8_t convert(double v){ return std::clamp(v, 0.0, 1.0) * 255; }
    };

    template<> class channel_traits<std::uint16_t>
    {
    public:
      static constexpr bool is_specialized = true;
 
      static constexpr std::uint16_t min() noexcept { return 0; }
      static constexpr std::uint16_t max() noexcept { return 65535; }
      static constexpr std::uint16_t convert(std::uint8_t v) { return static_cast<std::uint16_t>( ( v << 8 ) | v ); }
      static constexpr std::uint16_t convert(std::uint16_t v) { return v; }
      static constexpr std::uint16_t convert(float v){ return std::clamp(v, 0.0f, 1.0f) * 65535; }
      static constexpr std::uint16_t convert(double v){ return std::clamp(v, 0.0, 1.0) * 65535; }
    };

    template<> class channel_traits<float>
    {
    public:
      static constexpr bool is_specialized = true;
 
      static constexpr float min() noexcept { return 0.0f; }
      static constexpr float max() noexcept { return 1.0f; }
      static constexpr float convert(std::uint8_t v) { return v / 255.0f; }
      static constexpr float convert(std::uint16_t v) { return v / 65535.0f; }
      static constexpr float convert(float v){ return v; }
      static constexpr float convert(double v){ return static_cast<float>(v); }
    };

    template<> class channel_traits<double>
    {
    public:
      static constexpr bool is_specialized = true;
 
      static constexpr double min() noexcept { return 0.0; }
      static constexpr double max() noexcept { return 1.0; }
      static constexpr double convert(std::uint8_t v) { return v / 255.0; }
      static constexpr double convert(std::uint16_t v) { return v / 65535.0; }
      static constexpr double convert(float v){ return static_cast<double>(v); }
      static constexpr double convert(double v){ return v; }
    };
  }
}
