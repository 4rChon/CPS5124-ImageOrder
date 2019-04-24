#pragma once
#include <glm/vec3.hpp>

#define EPSILON 0.001f

namespace Raytracer
{
  struct Ray
  {
    glm::vec3 O;
    glm::vec3 D;
    float clip_min = EPSILON;
    float clip_max = 10000.f;

    Ray();

    Ray(const glm::vec3& O, const glm::vec3& D, const float clipping_distance[2]);
    Ray(const glm::vec3& O, const glm::vec3& D);
    void Ray::set(const glm::vec3& O, const glm::vec3& D);
  };
}