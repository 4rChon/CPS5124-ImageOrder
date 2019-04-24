#include "Geometric_primitive.h"
namespace Raytracer
{
  Geometric_primitive::Geometric_primitive(IShape& shape, IMaterial& material)
    : shape_(&shape), material_(&material)
  {}
  Geometric_primitive::~Geometric_primitive() {}

  bool Geometric_primitive::intersects(const Ray& ray, Intersection& i_out)
  {
    if (shape_->intersects(ray, i_out))
    {
      i_out.has_light = false;
      i_out.material = material_;
    }

    return i_out.is_valid;
  }

  bool Geometric_primitive::intersects(const Ray& ray)
  {
    return shape_->intersects(ray);
  }
}