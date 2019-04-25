#include "Area_light.h"

#include <algorithm>

#include "glm/gtx/norm.hpp"
#include "util.h"
#include "Sphere.h"
namespace Raytracer
{
  Area_light::Area_light(IShape& shape, const glm::vec3& L_e_)
    : shape_(&shape), ILight(L_e_)
  {}

  Area_light::~Area_light() {}

  glm::vec3 Area_light::sample_L(const glm::vec3& P, Light_sample& light_sample)
  {
    throw "Not implemented.";
  }

  glm::vec3 Area_light::sample_L(const std::vector<float>& xi,
                                 const glm::vec3& P,
                                 Light_sample& light_sample)
  {
    glm::vec3 surface_normal = shape_->surface_normal(P);
    glm::vec3 surface_point = ((Sphere*)shape_)->sample_cosine_weighted_facing_surface(xi[0], xi[1], surface_normal);

    glm::vec3 light_surface_to_point = surface_point - P;
    light_sample.direction = -glm::normalize(light_surface_to_point);
    light_sample.distance = glm::length(light_surface_to_point);
    light_sample.position = surface_point;

    float cos_theta_y = std::max(0.f, glm::dot(light_sample.direction, surface_normal));
    light_sample.PDF = glm::length2(light_surface_to_point)/(M_PI * cos_theta_y);

    return L_e_;
  }

}