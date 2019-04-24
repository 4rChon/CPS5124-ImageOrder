#include "Point_light.h"
#define _USE_MATH_DEFINES
#include <math.h>
namespace Raytracer
{
  Point_light::Point_light(const glm::vec3& position, const glm::vec3& L_e)
    : position_(position), ILight(L_e)
  {}

  Point_light::~Point_light() {}

  glm::vec3 Point_light::sample_L(const glm::vec3& P, Light_sample& light_sample)
  {
    glm::vec3 light_to_point = position_ - P;
    light_sample.direction = glm::normalize(light_to_point);
    light_sample.distance = glm::length(light_to_point);
    light_sample.position = position_;
    light_sample.PDF = 1.f/M_PI;
    return L_e_ / glm::length2(light_to_point);
  }

  glm::vec3 Point_light::sample_L(const std::vector<float>& xi,
                                  const glm::vec3& P,
                                  Light_sample& light_sample)
  {
    throw "Not implemented.";
  }
}