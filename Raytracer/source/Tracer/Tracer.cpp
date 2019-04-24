#define _CRT_SECURE_NO_WARNINGS
#include "Tracer.h"

#include <algorithm>
#include <iostream>
#include <Windows.h>
#include <ppl.h>

#include "gflags/gflags.h"

#include "util.h"

DEFINE_bool(progress, false, "Print progress every 1000 pixels");
DEFINE_bool(stratified_sampling, true, "Use a stratified sampling strategy");
DECLARE_bool(depth_map);


namespace Raytracer
{
  Tracer::Tracer(const unsigned int dimensions[2], const unsigned int depth)
  : depth_(depth),
    width_(dimensions[0]),
    height_(dimensions[1])
  {
    for (auto y = 0U; y < height_; ++y)
    {
      for (auto x = 0U; x < width_; ++x)
      {
        framebuffer_.push_back(Pixel(x, y));
      }
    }
  }

  Tracer::~Tracer() {}

  std::vector<Pixel> Tracer::render(Scene& scene,
    Camera& camera,
    Sampler& sampler)
  {
    auto i = 0;
    auto before = GetTickCount();
    for (auto& pixel : framebuffer_)
    {
      auto L = glm::vec3(0); // Initial radiance
      concurrency::parallel_for(size_t(0), sampler.spp, [&](size_t s)
      {
        auto spp_sample =
          FLAGS_stratified_sampling
          ? sampler.next_stratified_sample()
          : sampler.next_uniform_sample();
        float xx = (float)(pixel.x + spp_sample.jitter.x) / (float)width_;
        float yy = (float)(pixel.y + spp_sample.jitter.y) / (float)height_;

        if (camera.get_type() == Camera::Pinhole)
        {
          Ray ray = camera.get_ray(xx, yy);
          L += trace(scene, ray, sampler, depth_) * spp_sample.weight;
        }
        else if (camera.get_type() == Camera::Lens)
        {
          auto dof_sample = sampler.next_uniform_sample();
          Ray ray = camera.get_ray(xx, yy, dof_sample.jitter);
          L += trace(scene, ray, sampler, depth_) * spp_sample.weight;
        }
        else
        {
          throw "Invalid camera type.";
        }
      });

      pixel.colour = L;

      if (FLAGS_progress && i++ % 1000 == 0)
      {
        printf("%d/%d\n", i++, height_*width_);
      }
    }

    printf("elapsed time: %f seconds \n", double(GetTickCount() - before)/1000);

    return framebuffer_;
  }

  void Tracer::render_depth_map(Scene& scene, Camera& camera)
  {
    std::cout << "Rendering depthmap\n";

    for (auto& pixel : framebuffer_)
    {
      if (camera.get_type() == Camera::Pinhole)
      {
        Ray ray = camera.get_ray(pixel.x / (float)width_, pixel.y / (float)height_);
        Intersection i;

        if (scene.find_intersection(ray, i))
        {
          pixel.depth = i.distance;
        }
      }

      if (camera.get_type() == Camera::Lens)
      {
        Ray ray = camera.get_ray(pixel.x / (float)width_, pixel.y / (float)height_, glm::vec2(0.f, 0.f));
        Intersection i;

        if (scene.find_intersection(ray, i))
        {
          pixel.depth = i.distance;
        }
      }
    }
  }

  void Tracer::tone_map_linear()
  {
    auto max_colour = Util::max_colour(framebuffer_);
    for (auto& pixel : framebuffer_)
    {
      pixel.colour = 255.f * (pixel.colour / max_colour);
    }
  }

  void Tracer::tone_map_sigmoid()
  {
    for (auto& pixel : framebuffer_)
    {
      pixel.colour = 255.f * (pixel.colour / (1.f + pixel.colour));
    }
  }

  void Tracer::tone_map_depth()
  {
    for (auto& pixel : framebuffer_)
    {
      pixel.depth = 255.f * (1.f - (pixel.depth / (1.f + pixel.depth)));
    }
  }

  void Tracer::save(std::string path, std::string filename) const
  {
    auto output = path + "/" + filename;
    FILE* fp = fopen(output.c_str(), "wb+");
    (void)fprintf(fp, "P6\n%d %d\n255\n", width_, height_);

    for (auto& pixel : framebuffer_)
    {
      auto colour = pixel.colour;
      static unsigned char ppm_colour[3];
      ppm_colour[0] = (char)colour.r;
      ppm_colour[1] = (char)colour.g;
      ppm_colour[2] = (char)colour.b;
      (void)fwrite(ppm_colour, 1, 3, fp);
    }

    (void)fclose(fp);
  }

  void Tracer::save_depth(std::string path, std::string filename) const
  {
    auto output = path + "/depth_" + filename;

    FILE* fp = fopen(output.c_str(), "wb+");
    (void)fprintf(fp, "P6\n%d %d\n255\n", width_, height_);

    for (auto& pixel : framebuffer_)
    {
      static unsigned char ppm_colour[3];
      ppm_colour[0] = (char)pixel.depth;
      ppm_colour[1] = (char)pixel.depth;
      ppm_colour[2] = (char)pixel.depth;
      (void)fwrite(ppm_colour, 1, 3, fp);
    }

    (void)fclose(fp);
  }

  void Tracer::draw() const
  {

  }
}