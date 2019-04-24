#pragma once
#include "IPrimitive.h"
#include "IShape.h"
#include "IMaterial.h"

namespace Raytracer
{
  class Geometric_primitive
    : public IPrimitive
  {
  private:
    IShape* shape_;
    IMaterial* material_;
  public:
    Geometric_primitive(IShape& shape, IMaterial& material);
    ~Geometric_primitive() override;

    bool intersects(const Ray& ray, Intersection& i_out);
    bool intersects(const Ray& ray);
  };
}