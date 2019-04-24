#pragma once
#include "Tracer.h"

namespace Raytracer
{
  class Distributed_tracer
    : public Tracer
  {
  private:
    const unsigned int reflection_samples_ = 32;
    const unsigned int transmission_samples_ = 1;
    const unsigned int shadow_samples_ = 32;
  public:
    Distributed_tracer(const unsigned int dimensions[2], const unsigned int depth);
    ~Distributed_tracer() override;

    glm::vec3 trace(Scene& scene,
                    Ray& ray,
                    Sampler& sampler,
                    const unsigned int depth) override;
  };
}