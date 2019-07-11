#ifndef LIGHTS_H
#define LIGHTS_H

#include "model/object.h"
#include "display/camera.h"
#include "shader/shader.h"

#include <glad/glad.h>

#include <memory>

typedef glm::vec3 vec3;
typedef glm::vec4 vec4;
typedef glm::mat4 mat4;

class Lights
{
public:
  struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
  };

  struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
  };

  Lights(std::shared_ptr<Camera> camera);
  ~Lights();

  void add_dir_light(DirLight&& light);
  void add_point_light(PointLight&& light);
  vec3 get_point_light_pos(unsigned int i) const;

  void update() const;
  void draw(const Shader& shader) const;

private:
  Object create_light_object();

  unsigned int UBO;
  std::shared_ptr<Camera> camera;
  std::vector<std::pair<Object, PointLight>> point_lights;
  std::vector<std::pair<Object, DirLight>> dir_lights;
};

#endif // LIGHTS_H
