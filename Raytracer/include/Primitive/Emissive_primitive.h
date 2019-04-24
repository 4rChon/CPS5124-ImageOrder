#pragma once
#pragma once
#include "IPrimitive.h"
#include "IShape.h"
#include "IMaterial.h"
#include "ILight.h"

namespace Raytracer
{
  class Emissive_primitive
    : public IPrimitive
  {
  private:
    IShape* shape_;
    IMaterial* material_;
    ILight* light_;
  public:
    Emissive_primitive(IShape& shape, IMaterial& material, ILight& light);
    ~Emissive_primitive() override;

    bool intersects(const Ray& ray, Intersection& i_out);
    bool intersects(const Ray& ray);
  };
}