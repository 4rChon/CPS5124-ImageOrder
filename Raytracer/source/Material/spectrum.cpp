#pragma once
#include "Spectrum.h"

#include <glm/gtx/norm.hpp> // glm::length
#include <algorithm>        // std::max


namespace Raytracer
{
  namespace Spectrum
  {
    bool is_black(const glm::vec3 &brdf)
    {
      return glm::length2(brdf) == 0.f;
    }

    float max_component(const glm::vec3 &v)
    {
      return std::max(std::max(v.r, v.g), v.b);
    }
  }
}