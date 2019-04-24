#include "Coordinate_frame.h"
#include <glm/gtx/norm.hpp>

namespace Raytracer
{
  void Coordinate_frame::look_at(glm::vec3 O, glm::vec3 T, glm::vec3 Y)
  {
    // observer position O = camera position P
    P = O;
    // forward vector W is a unit-length vector in the direction of the target
    W = glm::normalize(T - O);
    // Right vector: U = norm(YxW)
    U = glm::normalize(glm::cross(Y, W));
    // Up vector: V = norm(UxW)
    V = -glm::normalize(glm::cross(W, U));
  }

  void Coordinate_frame::make(glm::vec3 A)
  {
    P = A;
    W = glm::normalize(A);
    V = glm::vec3(-W.z, 0, W.y);

    if (W.z == 0 && W.y == 0)
      V = glm::vec3(0, -W.z, W.x);

    U = glm::normalize(glm::cross(V, W));
    V = -glm::normalize(glm::cross(W, U));
  }

  glm::mat3 Coordinate_frame::get_matrix() const
  {
    return glm::mat3(U, V, W);
  }
}