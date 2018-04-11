#include "owl/color/gamma_correction.hpp"

namespace owl
{
  namespace color
  {
    double linear_2_gamma_corrected(double u, gamma_correction_model::s_rgb cm)
    {
      constexpr double a = 1.055;
      constexpr double b = -0.055;
      constexpr double c = 12.92;
      constexpr double d = 0.0031308;
      constexpr double gamma = 1.0 / 2.4;
  
      if(u < 0)
        return -linear_2_gamma_corrected(-u, cm);
      if(u < d)
        return c * u;
      return a * std::pow(u, gamma) + b;
    }

    double gamma_corrected_2_linear(double u, gamma_correction_model::s_rgb cm)
    {
      constexpr double a = 1 / 1.055;
      constexpr double b = 0.055 / 1.055;
      constexpr double c = 1 / 12.92;
      constexpr double d = 0.04045;
      constexpr double gamma = 2.4;
  
      if(u < 0)
        return -linear_2_gamma_corrected(-u, cm);
      if(u < d)
        return c * u;
      return std::pow(a * u + b, gamma);
    }

    double linear_2_gamma_corrected(double u, gamma_correction_model::adobe_rgb cm)
    {
      constexpr double gamma = 1.0/2.19921875;
      if(u < 0)
        return -linear_2_gamma_corrected(-u, cm);
      return std::pow(u, gamma);
    }

    double gamma_corrected_2_linear(double u, gamma_correction_model::adobe_rgb)
    {
      constexpr double gamma = 2.19921875;
      return std::pow(u, gamma);
    }

  }
}
