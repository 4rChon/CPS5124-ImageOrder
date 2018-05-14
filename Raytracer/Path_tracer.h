#pragma once
#include "Tracer.h"

namespace Raytracer
{
  class Path_tracer
    : public Tracer
  {
  public:
    Path_tracer(const unsigned int dimensions[2], const unsigned int depth);
    ~Path_tracer();

    glm::vec3 trace(Scene& scene,
                    Ray& ray,
                    Sampler& sampler,
                    const unsigned int depth) override;
  };
}