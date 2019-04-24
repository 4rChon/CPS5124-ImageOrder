#include "Ray.h"
namespace Raytracer
{
  Ray::Ray() {}

  Ray::Ray(const glm::vec3& O, const glm::vec3& D)
    : O(O), D(D)
  {}

  Ray::Ray(const glm::vec3& O,
           const glm::vec3& D,
           const float clipping_distance[2])
    : O(O),
      D(D),
      clip_min(clipping_distance[0]),
      clip_max(clipping_distance[1])
  {}

  void Ray::set(const glm::vec3& O, const glm::vec3& D)
  {
    this->O = O;
    this->D = D;
  }
}