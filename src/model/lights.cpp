#include "lights.h"
#include "util/data.h"

Lights::Lights(std::shared_ptr<Camera> camera)
  : camera(camera)
{
  glGenBuffers(1, &UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 46 * sizeof (vec4), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 1, UBO);
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

  dir_lights.emplace_back(create_light_object(), std::move(light));
}

void Lights::add_point_light(Lights::PointLight&& light)
{
  const size_t num_light = point_lights.size();

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((21 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.position[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((22 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.ambient[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((23 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.diffuse[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((24 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.specular[0]);
  glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long>((25 + num_light) * sizeof (vec4)),
                  sizeof (vec3), &light.attenuation[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  point_lights.emplace_back(create_light_object(), std::move(light));
}

vec3 Lights::get_point_light_pos(unsigned int i) const
{
  return point_lights.at(i).second.position;
}

void Lights::update() const
{
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (vec3), &camera->get_position()[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::draw(const Shader& shader) const
{
  for (const auto& [obj, dir_light] : dir_lights) {
    obj.draw(shader);
  }
  for (const auto& [obj, point_light] : point_lights) {
    obj.draw(shader);
  }
}

Object Lights::create_light_object()
{
  Object light;
  light.start_setup();
  light.add_vertices(CUBE_VERTICES, 24, sizeof(CUBE_VERTICES));
  light.add_indices(CUBE_INDICES, 36, sizeof (CUBE_INDICES));
  light.add_vertex_attribs({ 3, 3, 2 });
  light.finalize_setup();
  return light;
}
