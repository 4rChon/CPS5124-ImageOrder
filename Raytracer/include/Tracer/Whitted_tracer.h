#pragma once
#include "Tracer.h"

namespace Raytracer
{
  class Whitted_tracer
    : public Tracer
  {
  public:
    Whitted_tracer(const unsigned int dimensions[2], const unsigned int depth);
    ~Whitted_tracer() override;

    glm::vec3 trace(Scene& scene,
                    Ray& ray,
                    Sampler& sampler,
                    const unsigned int depth);
  };
}