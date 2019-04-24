#pragma once
#include <unordered_map> // std::unordered_map

#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/error/en.h>

#include "Camera.h"
#include "IMaterial.h"
#include "IShape.h"
#include "Tracer.h"
#include "Scene.h"

namespace Raytracer
{
  namespace Json
  {
    rapidjson::Document parse_json_document(std::string path);
    typedef rapidjson::GenericArray<false, rapidjson::GenericValue<rapidjson::UTF8<>>> json_array;
    json_array get_element_array(rapidjson::Document& document, const char* element);
    std::unordered_map<std::string, Camera*> init_cameras(rapidjson::Document& document);
    std::unordered_map<std::string, IMaterial*> init_materials(rapidjson::Document& document);
    std::unordered_map<std::string, IShape*> init_shapes(rapidjson::Document& document);
    std::unordered_map<std::string, ILight*> init_lights(rapidjson::Document& document);
    Scene* get_scene(rapidjson::Document& document);
    Sampler* get_sampler(rapidjson::Document& document);
    Camera* get_camera(rapidjson::Document& document);
    Tracer* get_renderer(rapidjson::Document& document);

    template<typename T>
    void assert_exists(std::unordered_map<std::string, T> map, const char* key, const char* object_type)
    {
      if (map.find(key) == map.end())
      {
        std::cout << key << " is not a valid " << object_type << " object.\n";
        exit(1);
      }
    }
  }
}