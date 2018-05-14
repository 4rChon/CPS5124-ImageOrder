#include <iostream>
#include <string>
#include <sys/stat.h>
#include <experimental/filesystem>

#include <gflags/gflags.h>

#include "json.h"
#include "Tracer.h"
#include "Whitted_tracer.h"
#include "Camera.h"
#include "Pinhole_camera.h"

namespace fs = std::experimental::filesystem::v1;

DEFINE_bool(batch, false, "Whether to render all files in the load folder");

static bool validate_load_path(const char* flagname, const std::string& value)
{
  struct stat buffer;

  if (stat(value.c_str(), &buffer) != 0)
    return false;
  else if (buffer.st_mode & S_IFDIR)
    return true;
  return false;
}

DEFINE_string(load_path, "./definitions/PT/", "Folder containing scene definition file.");
DEFINE_validator(load_path, &validate_load_path);

static bool validate_save_path(const char* flagname, const std::string& value)
{
  if (!fs::create_directory(fs::directory_entry(value)))
    return true;

  struct stat buffer;

  if (stat(value.c_str(), &buffer) != 0)
    return false;
  else if (buffer.st_mode & S_IFDIR)
    return true;
  return false;
}

DEFINE_string(save_path, "./renders/", "Save folder location.");
DEFINE_validator(save_path, &validate_save_path);

static bool validate_input_file(const char* flagname, const std::string& value)
{
  if (FLAGS_batch)
  {
    return true;
  }

  struct stat buffer;
  auto full_path = FLAGS_load_path + value;
  return stat(full_path.c_str(), &buffer) == 0;
}

DEFINE_string(input_file, "assignment_03.json", "Input filename");
DEFINE_validator(input_file, &validate_input_file);

DEFINE_string(output_file, "out.ppm", "Output filename");
DEFINE_bool(draw, false, "Whether to draw result after tracing is complete (NYI)");
DEFINE_bool(depth_map, false, "Whether to output a depth map alongside the coloured render");

static bool validate_tonemapper(const char* flagname, const std::string& value)
{
  return value.compare("sigmoid") == 0 || value.compare("linear") == 0 || value.compare("none") == 0;
}

DEFINE_string(tonemapper, "sigmoid", "Tone mapper to use (sigmoid or linear");
DEFINE_validator(tonemapper, &validate_tonemapper);

int main(int argc, char *argv[])
{
  gflags::ParseCommandLineFlags(&argc, &argv, true);

  std::vector<std::string> input_files;
  std::vector<std::string> output_files;
  if (FLAGS_batch) 
  {
    for (auto& p : fs::directory_iterator(FLAGS_load_path))
    {
      input_files.push_back(p.path().string());
      fs::path save_file(p.path().filename());
      save_file.replace_extension("ppm");
      output_files.push_back(save_file.string());
    }
  }
  else
  {
    input_files.push_back(FLAGS_load_path + FLAGS_input_file);
    output_files.push_back(FLAGS_output_file);
  }

  auto i = 0;
  for (auto& f : input_files)
  {
    auto document = Raytracer::Json::parse_json_document(f);
    Raytracer::Scene* scene = Raytracer::Json::get_scene(document);
    Raytracer::Camera* camera = Raytracer::Json::get_camera(document);
    Raytracer::Tracer* tracer = Raytracer::Json::get_renderer(document);
    Raytracer::Sampler* sampler = Raytracer::Json::get_sampler(document);

    if (!FLAGS_depth_map)
    {
      tracer->render(*scene, *camera, *sampler);

      if (FLAGS_tonemapper.compare("sigmoid") == 0)
        tracer->tone_map_sigmoid();
      else if (FLAGS_tonemapper.compare("linear") == 0)
        tracer->tone_map_linear();
      tracer->save(FLAGS_save_path, output_files[i]);
    }

    if (FLAGS_depth_map)
    {
      tracer->render_depth_map(*scene, *camera);
      tracer->tone_map_depth();
      tracer->save_depth(FLAGS_save_path, output_files[i]);
    }
    if (FLAGS_draw)
      tracer->draw();

    delete scene;
    delete camera;
    delete sampler;
    delete tracer;
    ++i;
  }
  
}