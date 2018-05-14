#include "Pinhole_camera.h"

namespace Raytracer
{
  Pinhole_camera::Pinhole_camera(const float fov,
                                 const float aspect_ratio,
                                 const float distance,
                                 const glm::vec3& position,
                                 const glm::vec3& target)
    : Camera(fov,
             aspect_ratio,
             distance,
             position,
             target)
  {}

  Pinhole_camera::~Pinhole_camera() {}

  Ray Pinhole_camera::get_ray(const float x, const float y)
  {
    glm::vec3 C = F_.W * distance_ - F_.U * v_w_ - F_.V * v_h_;
    glm::vec3 D = glm::normalize(C + 2.f * (F_.U * x * v_w_ + F_.V * y * v_h_));

    return Ray(F_.P, D);
  }
}