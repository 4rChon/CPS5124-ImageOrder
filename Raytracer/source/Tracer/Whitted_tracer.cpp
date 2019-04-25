#include "Whitted_tracer.h"

#include <algorithm>

#include "Ad_hoc_material.h"
#include "shader.h"
#include "util.h"

namespace Raytracer
{
  Whitted_tracer::Whitted_tracer(const unsigned int dimensions[2], const unsigned int depth)
    :Tracer(dimensions, depth) 
  {}

  Whitted_tracer::~Whitted_tracer() {}

  glm::vec3 Whitted_tracer::trace(Scene& scene,
                                  Ray& ray,
                                  Sampler& sampler,
                                  const unsigned int depth)
  {
    Ray r;
    Intersection i;
    glm::vec3 i_D(0), i_S(0), i_T(0);
    Ad_hoc_material* material;

    // Find closest point of intersection
    if (scene.find_intersection(ray, i))
    {
      //Compute direct lighting (diffuse)
      i_D = scene.compute_direct(Shader::whitted_shader, sampler, i);
      if (depth > 0)
      {
        // Get material at point of intersection
        material = (Ad_hoc_material*)i.material;
        if (material->has_type(BxDF::Specular))
        {
          float eta = material->get_eta();
          float fresnel = 1.f;
          float normal_dot_incident = glm::dot(i.shading_ONB.W, ray.D);
          if (eta > 0.f)
          {
            if (normal_dot_incident < 0.f)
            {
              eta = 1.f / eta;
              fresnel = Util::fresnel_schlick(normal_dot_incident, eta, 1.f);
            }
            else
            {
              fresnel = Util::fresnel_schlick(normal_dot_incident, 1.f, eta);
            }
          }
          // Specular reflection
          if (material->has_type(BxDF::Reflect))
          {
            r.set(i.P, glm::reflect(ray.D, i.shading_ONB.W));
            i_S = fresnel * trace(scene, r, sampler, depth - 1) * material->k_S;
          }

          // Specular transmission
          if (material->has_type(BxDF::Transmit))
          {
            // Is entering or exiting material?
            if (normal_dot_incident < 0.f)
            {
              r.set(i.P, glm::normalize(glm::refract(ray.D, i.shading_ONB.W, eta)));
            }
            else
            {
              r.set(i.P, glm::normalize(glm::refract(ray.D, -i.shading_ONB.W, eta)));
            }

            i_T = (1 - fresnel) * trace(scene, r, sampler, depth - 1) * material->k_T;
          }
        }
      }
    }

    return i_D + i_S + i_T;
  }
}
