#include "lights.h"
#include "util/data.h"

Lights::Lights(std::shared_ptr<Camera> camera)
  : camera(camera),
    model_light("../../assets/sphere.obj")
{
  glGenBuffers(1, &UBO);
  glGenBuffers(1, &dir_SSBO);
  glGenBuffers(1, &point_SSBO);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof (int) + sizeof (vec3), nullptr, GL_DYNAMIC_DRAW);
  glBindBufferBase(GL_UNIFORM_BUFFER, 2, UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, dir_SSBO);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, point_SSBO);
}

Lights::~Lights()
{
  glDeleteBuffers(1, &UBO);
  glDeleteBuffers(1, &dir_SSBO);
  glDeleteBuffers(1, &point_SSBO);
}

void Lights::add_dir_light(Lights::DirLight&& light)
{
  dir_lights.emplace_back(std::move(light));

  const size_t num_light = dir_lights.size();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, dir_SSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<long>(num_light * DIR_NUM_ELEMS * sizeof (vec4)),
               nullptr, GL_STATIC_DRAW);

  for (size_t i = 0; i < num_light; i++) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((0 + i * POINT_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &dir_lights[i].direction[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((1 + i * DIR_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &dir_lights[i].ambient[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((2 + i * DIR_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &dir_lights[i].diffuse[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((3 + i * DIR_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &dir_lights[i].specular[0]);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof (vec3), sizeof (int), &num_light);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::add_point_light(Lights::PointLight&& light)
{
  point_light_transforms.push_back({vec3(0.05f), {}, light.position});
  point_lights.emplace_back(std::move(light));

  const size_t num_light = point_lights.size();

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, point_SSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               static_cast<long>(num_light * POINT_NUM_ELEMS * sizeof (vec4)),
               nullptr, GL_STATIC_DRAW);

  for (size_t i = 0; i < num_light; i++) {
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((0 + i * POINT_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &point_lights[i].position[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((1 + i * POINT_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &point_lights[i].ambient[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((2 + i * POINT_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &point_lights[i].diffuse[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((3 + i * POINT_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &point_lights[i].specular[0]);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,
                    static_cast<long>((4 + i * POINT_NUM_ELEMS) * sizeof (vec4)),
                    sizeof (vec3), &point_lights[i].attenuation[0]);
  }

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof (vec4), sizeof (int), &num_light);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::update() const
{
  glBindBuffer(GL_UNIFORM_BUFFER, UBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof (vec3), &camera->get_position()[0]);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void Lights::draw(const Shader& shader) const
{
  Object::set_model_transforms(point_light_transforms);

  model_light.draw_instanced(shader, static_cast<int>(point_lights.size()));
}
