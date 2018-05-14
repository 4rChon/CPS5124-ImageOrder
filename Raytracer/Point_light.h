#pragma once
#include "ILight.h"
#include <glm/gtx/norm.hpp>

namespace Raytracer
{
  class Point_light
    : public ILight
  {
  private:
    glm::vec3 position_;
  public:
    Point_light(const glm::vec3& position, const glm::vec3& L_e);
    ~Point_light();

    glm::vec3 sample_L(const glm::vec3& P, Light_sample& light_sample) override;
    glm::vec3 sample_L(const std::vector<float>& xi,
                       const glm::vec3& P,
                       Light_sample& light_sample) override;
    bool is_delta() const override { return true; }
  };
}