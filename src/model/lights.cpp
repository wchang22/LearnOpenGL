#include "lights.h"
#include "util/data.h"

Lights::Lights(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_light("../../assets/sphere.obj")
{
  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER,
               (1 + (DIR_NUM_ELEMS + POINT_NUM_ELEMS) * NUM_LIGHTS) * sizeof (vec4),
               nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 2, UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

Lights::~Lights()
{
  glDeleteBuffers(1, &UBO);
}

void Lights::add_dir_light(Lights::DirLight&& light)
{
  const size_t num_light = dir_lights.size();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((1 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.direction[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((2 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.ambient[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((3 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.diffuse[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((4 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.specular[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  dir_lights.emplace_back(std::move(light));
}

void Lights::add_point_light(Lights::PointLight&& light)
{
  const size_t num_light = point_lights.size();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER,
                  static_cast<long>((1 + (DIR_NUM_ELEMS * NUM_LIGHTS) + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.position[0]);
  glBufferSubData(GL_UNIFORM_BUFFER,
                  static_cast<long>((2 + (DIR_NUM_ELEMS * NUM_LIGHTS) + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.ambient[0]);
  glBufferSubData(GL_UNIFORM_BUFFER,
                  static_cast<long>((3 + (DIR_NUM_ELEMS * NUM_LIGHTS) + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.diffuse[0]);
  glBufferSubData(GL_UNIFORM_BUFFER,
                  static_cast<long>((4 + (DIR_NUM_ELEMS * NUM_LIGHTS) + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.specular[0]);
  glBufferSubData(GL_UNIFORM_BUFFER,
                  static_cast<long>((5 + (DIR_NUM_ELEMS * NUM_LIGHTS) + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.attenuation[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  point_lights.emplace_back(std::move(light));
}

vec3 Lights::get_point_light_pos(unsigned int i) const
{
  return point_lights.at(i).position;
}

void Lights::update() const
{
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (vec3), &camera->get_position()[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::draw(const Shader& shader) const
{
  model_light.draw_instanced(shader, static_cast<int>(dir_lights.size() + point_lights.size()));
}
