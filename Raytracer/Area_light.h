#pragma once
#include "ILight.h"
#include "IShape.h"

namespace Raytracer 
{
  class Area_light 
    : public ILight
  {
  private:
    IShape* shape_;
  public:
    Area_light(IShape& shape, const glm::vec3& L_e);
    ~Area_light();

    glm::vec3 sample_L(const glm::vec3& P, Light_sample& light_sample) override;
    glm::vec3 sample_L(const std::vector<float>& xi,
                       const glm::vec3& P,
                       Light_sample& light_sample) override;

    bool is_delta() const override { return false; }


  };
}