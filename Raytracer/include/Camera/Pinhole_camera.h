#pragma once
#include "Camera.h"
#include "Ray.h"

namespace Raytracer
{
  class Pinhole_camera
    : public Camera
  {
  public:
    Pinhole_camera(const float fov,
                   const float aspect_ratio,
                   const float distance,
                   const glm::vec3& position,
                   const glm::vec3& target);

    ~Pinhole_camera() override;

    Ray get_ray(const float x, const float y) override;
    Type get_type() const override { return Camera::Pinhole; }
  };
}