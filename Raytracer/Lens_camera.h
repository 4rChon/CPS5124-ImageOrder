#pragma once
#include "Camera.h"

namespace Raytracer
{
  class Lens_camera
    : public Camera
  {
  private:
    const float f_p_;
    const float alpha_;
  public:
    Lens_camera(const float fov,
                const float aspect_ratio,
                const float distance,
                const glm::vec3& position,
                const glm::vec3& target,
                const float f_p,
                const float alpha);

    ~Lens_camera() override;

    Ray get_ray(const float x,
                const float y,
                const glm::vec2& jitter) override;
    Type get_type() const override { return Camera::Lens; }

  };
}