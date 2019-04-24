#pragma once

#include <glm/vec3.hpp>

namespace Raytracer
{
  namespace Spectrum
  {
    bool is_black(const glm::vec3 &brdf);
    float max_component(const glm::vec3 &throughput);
  }
}