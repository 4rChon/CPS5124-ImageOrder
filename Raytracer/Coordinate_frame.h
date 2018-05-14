#pragma once
#include <glm/vec3.hpp>
#include <glm/mat3x3.hpp>

namespace Raytracer
{
  struct Coordinate_frame
  {
    glm::vec3 U, V, W, P;

    Coordinate_frame() :
      U(glm::vec3(0)),
      V(glm::vec3(0)),
      W(glm::vec3(0)),
      P(glm::vec3(0))
    {}
    Coordinate_frame(glm::vec3 O,
                     glm::vec3 T,
                     glm::vec3 Y) { look_at(O, T, Y); }
    Coordinate_frame(glm::vec3 A) { make(A); }

    void look_at(glm::vec3 O,
                 glm::vec3 T,
                 glm::vec3 Y);
    void make(glm::vec3 A);
    glm::mat3 get_matrix() const;
  };
}