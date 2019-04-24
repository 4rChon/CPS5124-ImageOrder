#pragma once
#include <glm/vec3.hpp>

namespace Raytracer
{
  struct Pixel
  {
    glm::vec3 colour;
    const int x, y;
    float depth;

    Pixel(const unsigned int x, const unsigned int y);
  };
}
