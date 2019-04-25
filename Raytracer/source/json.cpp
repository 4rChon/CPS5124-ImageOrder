#define _CRT_SECURE_NO_DEPRECATE
#define _SCL_SECURE_NO_WARNINGS

#include "Json.h"

#include <iostream>  // std::cout, std::endl

#include "Ad_hoc_material.h"
#include "Area_light.h"
#include "Distributed_tracer.h"
#include "Emissive_primitive.h"
#include "Geometric_primitive.h"
#include "Lens_camera.h"
#include "Path_tracer.h"
#include "Pinhole_camera.h"
#include "Point_light.h"
#include "Sphere.h"
#include "Util.h" // Util::
#include "Whitted_tracer.h"

namespace Raytracer
{
  namespace Json
  {
    rapidjson::Document parse_json_document(std::string path)
    {
      std::cout << "Parsing json file: " << path.c_str() << "\n";
      FILE* fp = fopen(path.c_str(), "rb");
      if (fp == NULL)
      {
        std::cout << "File error: " << stderr << "\n";
        exit(1);
      }

      fseek(fp, 0, SEEK_END);
      long l_size = ftell(fp);
      rewind(fp);

      char* buffer = (char*)malloc(sizeof(char)*l_size);
      if (buffer == NULL)
      {
        std::cout << "Memory error: " << stderr << "\n";
        exit(2);
      }

      size_t result = fread(buffer, 1, l_size, fp);
      if (result != l_size)
      {
        std::cout << "Reading error: " << stderr << "\n";
        exit(3);
      }

      rapidjson::Document document;
      rapidjson::ParseResult ok = document.Parse<rapidjson::ParseFlag::kParseStopWhenDoneFlag>(buffer);

      if (!ok) {
        std::cout << "JSON parse error: " << rapidjson::GetParseError_En(ok.Code()) << " " << ok.Offset();
        exit(1);
      }

      fclose(fp);
      std::cout << "Json parsed: " << path.c_str() << "\n";
      return document;
    }

    typedef rapidjson::GenericArray<false, rapidjson::GenericValue<rapidjson::UTF8<>>> json_array;
    json_array get_element_array(rapidjson::Document& document, const char* element)
    {
      assert(document.HasMember(element));
      assert(document[element].IsArray());
      auto element_array = document[element].GetArray();

      return element_array;
    }

    std::unordered_map<std::string, Camera*> init_cameras(rapidjson::Document& document)
    {
      auto cameras = get_element_array(document, "cameras");

      std::unordered_map<std::string, Camera*> scene_cameras;
      for (unsigned int i = 0; i < cameras.Size(); i++)
      {
        auto camera = cameras[i].GetObject();

        const char* id = camera["id"].GetString();
        std::string type = std::string(camera["type"].GetString());
        float fov = camera["fov"].GetFloat();
        float aspect = camera["aspect"].GetFloat();
        float distance = camera["distance"].GetFloat();
        auto p = camera["position"].GetArray();
        glm::vec3 position = glm::vec3(p[0].GetFloat(), p[1].GetFloat(), p[2].GetFloat());
        auto t = camera["target"].GetArray();
        glm::vec3 target = glm::vec3(t[0].GetFloat(), t[1].GetFloat(), t[2].GetFloat());

        if (type.compare("pinhole") == 0)
        {
          Camera* camera = new Pinhole_camera(fov, aspect, distance, position, target);
          scene_cameras.emplace(id, camera);
        }
        else if (type.compare("lens-based") == 0)
        {
          float fp = camera["focal distance"].GetFloat();
          float alpha = camera["alpha"].GetFloat();
          Camera* camera = new Lens_camera(fov, aspect, distance, position, target, fp, alpha);
          scene_cameras.emplace(id, camera);
        }
      }

      return scene_cameras;
    }

    std::unordered_map<std::string, IMaterial*> init_materials(rapidjson::Document& document)
    {
      auto materials = get_element_array(document, "materials");

      std::unordered_map<std::string, IMaterial*> scene_materials;
      for (unsigned int i = 0; i < materials.Size(); i++)
      {
        auto json_material = materials[i].GetObject();

        const char* id = json_material["id"].GetString();
        std::string types = std::string(json_material["type"].GetString());

        std::vector<std::string> type_list;
        Util::split(types, " ", type_list);

        int material_type = 0;
        glm::vec3 rho_D = glm::vec3(0);
        glm::vec3 rho_R = glm::vec3(0);
        glm::vec3 rho_T = glm::vec3(0);
        float beta = 0.f;
        float eta = 0.f;

        if (types.compare("diffuse") == 0)
        {
          auto r = json_material["rho"].GetArray();
          rho_D = glm::vec3(r[0].GetFloat(), r[1].GetFloat(), r[2].GetFloat());

          material_type = BxDF::Diffuse;
        }
        else if (types.compare("specular reflection") == 0)
        {
          auto r = json_material["rho"].GetArray();
          rho_R = glm::vec3(r[0].GetFloat(), r[1].GetFloat(), r[2].GetFloat());

          material_type = BxDF::Specular | BxDF::Reflect;
        }
        else if (types.compare("glossy reflection") == 0)
        {
          auto r = json_material["rho"].GetArray();
          rho_R = glm::vec3(r[0].GetFloat(), r[1].GetFloat(), r[2].GetFloat());
          rho_D = rho_R;
          beta = json_material["roughness"].GetFloat();

          material_type = BxDF::Glossy | BxDF::Reflect | BxDF::Specular;
        }
        else if (types.compare("fresnel dielectric") == 0)
        {
          auto r1 = json_material["rhoR"].GetArray();
          rho_R = glm::vec3(r1[0].GetFloat(), r1[1].GetFloat(), r1[2].GetFloat());
          auto r2 = json_material["rhoT"].GetArray();
          rho_T = glm::vec3(r2[0].GetFloat(), r2[1].GetFloat(), r2[2].GetFloat());
          eta = json_material["eta"].GetFloat();

          material_type = BxDF::Specular | BxDF::Reflect | BxDF::Transmit;
        }
        else
        {
          std::cout << "Unknown material type: " << types << std::endl;
          exit(1);
        }

        Ad_hoc_material* material = new Ad_hoc_material(material_type, rho_D, rho_R, rho_T, beta, eta);
        scene_materials.emplace(id, material);
      }

      return scene_materials;
    }

    std::unordered_map<std::string, IShape*> init_shapes(rapidjson::Document& document)
    {
      auto shapes = get_element_array(document, "shapes");

      std::unordered_map<std::string, IShape*> scene_shapes;
      for (unsigned int i = 0; i < shapes.Size(); i++)
      {
        auto shape = shapes[i].GetObject();

        const char* id = shape["id"].GetString();
        std::string type = std::string(shape["type"].GetString());
        auto c = shape["centre"].GetArray();
        glm::vec3 centre = glm::vec3(c[0].GetFloat(), c[1].GetFloat(), c[2].GetFloat());
        float radius = shape["radius"].GetFloat();

        if (type.compare("sphere") == 0)
        {
          Sphere* shape = new Sphere(radius, centre);
          scene_shapes.emplace(id, shape);
        }
      }

      return scene_shapes;
    }

    std::unordered_map<std::string, ILight*> init_lights(rapidjson::Document& document)
    {
      auto lights = get_element_array(document, "lights");

      std::unordered_map<std::string, ILight*> scene_lights;
      for (unsigned int i = 0; i < lights.Size(); i++)
      {
        auto light = lights[i].GetObject();

        const char* id = light["id"].GetString();
        std::string type = std::string(light["type"].GetString());
        auto Le = light["power"].GetArray();
        glm::vec3 power = glm::vec3(Le[0].GetFloat(), Le[1].GetFloat(), Le[2].GetFloat());

        if (type.compare("point") == 0)
        {
          auto p = light["position"].GetArray();
          glm::vec3 position = glm::vec3(p[0].GetFloat(), p[1].GetFloat(), p[2].GetFloat());

          Point_light* light_entity = new Point_light(position, power);
          scene_lights.emplace(id, light_entity);
        }
        else if (type.compare("area") == 0)
        {
          auto shapes = init_shapes(document);
          const char* shape = light["shape"].GetString();
          assert_exists(shapes, shape, "shape");

          Area_light* light_entity = new Area_light(*shapes[shape], power);
          scene_lights.emplace(id, light_entity);
        }
      }

      return scene_lights;
    }

    Scene* get_scene(rapidjson::Document& document)
    {
      // Get Scene json fields
      assert(document.HasMember("scene"));
      auto json_scene = document["scene"].GetObject();

      // Init scene elements
      auto materials = init_materials(document);
      auto shapes = init_shapes(document);
      auto lights = init_lights(document);

      assert(json_scene.HasMember("lights"));
      assert(json_scene["lights"].IsArray());
      auto light_ids = json_scene["lights"].GetArray();

      assert(json_scene.HasMember("primitives"));
      assert(json_scene["primitives"].IsArray());
      auto primitive_list = json_scene["primitives"].GetArray();

      Scene* scene = new Scene();
      for (unsigned int i = 0; i < primitive_list.Size(); i++)
      {
        const char* id = primitive_list[i]["id"].GetString();
        std::string type = primitive_list[i]["type"].GetString();
        
        const char* shape = primitive_list[i]["shape"].GetString();
        assert_exists(shapes, shape, "shape");
        
        const char* material = primitive_list[i]["material"].GetString();
        assert_exists(materials, material, "material");
        if (type.compare("geometric") == 0)
        {
          Geometric_primitive* p = new Geometric_primitive(*shapes[shape], *materials[material]);
          scene->add_primitive(*p);
        }
        else if (type.compare("emissive") == 0)
        {
          const char* light = primitive_list[i]["light"].GetString();
          assert_exists(lights, light, "light");
          Emissive_primitive* p = new Emissive_primitive(*shapes[shape], *materials[material], *lights[light]);
          scene->add_primitive(*p);
        }
      }

      for (unsigned int i = 0; i < light_ids.Size(); i++)
      {
        const char* id = light_ids[i].GetString();
        assert_exists(lights, id, "light");
        scene->add_light_source(*lights[id]);
      }

      return scene;
    }

    Sampler* get_sampler(rapidjson::Document& document)
    {
      // Get Scene json fields
      assert(document.HasMember("scene"));
      auto json_scene = document["scene"].GetObject();

      assert(json_scene.HasMember("renderer"));
      auto renderer = json_scene["renderer"].GetObject();
      int samples = renderer["samples"].GetInt();
      Sampler* sampler = new Sampler(samples);

      return sampler;
    }

    Camera* get_camera(rapidjson::Document& document)
    {
      assert(document.HasMember("scene"));
      auto json_scene = document["scene"].GetObject();

      // Init camera
      auto cameras = init_cameras(document);
      assert(json_scene.HasMember("camera"));
      const char* camera_id = json_scene["camera"].GetString();
      assert_exists(cameras, camera_id, "camera");
      return cameras[camera_id];
    }

    Tracer* get_renderer(rapidjson::Document& document)
    {
      assert(document.HasMember("scene"));
      auto json_scene = document["scene"].GetObject();

      assert(json_scene.HasMember("renderer"));
      auto renderer = json_scene["renderer"].GetObject();
      std::string renderer_type = std::string(renderer["type"].GetString());
      auto json_dims = renderer["dimensions"].GetArray();
      unsigned int dimensions[2] = { (unsigned int)json_dims[0].GetInt(), (unsigned int)json_dims[1].GetInt() };
      int depth = renderer["depth"].GetInt();
      if (depth > 20)
        depth = 20;
      const char* output = renderer["output"].GetString();

      if (renderer_type.compare("WRT") == 0)
      {
        return new Whitted_tracer(dimensions, (unsigned int)depth);
      }
      else if (renderer_type.compare("PT") == 0)
      {
        return new Path_tracer(dimensions, (unsigned int)depth);
      }

      std::cout << "Unknown tracer type: " << renderer_type << std::endl;
      exit(1);
    }
  }
}