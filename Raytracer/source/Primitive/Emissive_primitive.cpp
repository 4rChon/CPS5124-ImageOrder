#include "Emissive_primitive.h"

namespace Raytracer
{
  Emissive_primitive::Emissive_primitive(IShape& shape, IMaterial& material, ILight& light)
    : shape_(&shape), material_(&material), light_(&light)
  {}
  Emissive_primitive::~Emissive_primitive() {}

  bool Emissive_primitive::intersects(const Ray& ray, Intersection& i_out)
  {
    if (shape_->intersects(ray, i_out))
    {
      i_out.has_light = true;
      i_out.L_e = light_->L_e();
      i_out.material = material_;
    }

    return i_out.is_valid;
  }

  bool Emissive_primitive::intersects(const Ray& ray)
  {
    return shape_->intersects(ray);
  }
}