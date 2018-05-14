#pragma once
#include "Coordinate_frame.h"

namespace Raytracer
{
  class IShape;
  class IMaterial;

  struct Intersection
  {
  public:
    bool is_valid = true;
    bool has_light = false;
    float distance;
    glm::vec3 L_e;
    glm::vec3 D;
    glm::vec3 P;
    Coordinate_frame geometry_ONB;
    Coordinate_frame shading_ONB;
    IShape* shape;
    IMaterial* material;

    Intersection();

    // remove copy constructor to prevent unexpected behaviour
    Intersection(const Intersection&) = delete;

    // explicit assignment operator due to pointer members
    Intersection& operator=(const Intersection& rvalue);
  };
}