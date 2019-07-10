#ifndef OBJECT_H
#define OBJECT_H

#include "shader/shader.h"
#include "shader/textures.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <memory>
#include <optional>

typedef glm::vec3 vec3;
typedef glm::mat4 mat4;

class Object
{
public:
  Object();
  Object(Object&& other);
  ~Object();

  void start_setup();
  void add_vertices(void* vertices, int num_vertices, size_t size, GLenum draw_type = GL_STATIC_DRAW);
  void add_indices(void* indices, int num_indices, size_t size, GLenum draw_type = GL_STATIC_DRAW);
  void add_vertex_attribs(const std::vector<int>& vertex_attrib_sizes);
  void finalize_setup();

  static void set_model_transform(std::optional<vec3> scale,
                           std::optional<std::pair<float, vec3>> rotate,
                           std::optional<vec3> translate);
  static void set_world_space_transform(mat4 perspective, mat4 view);

  void draw(const Shader& shader, const Textures& textures,
            const std::vector<std::string_view> flags = {}) const;
  void draw(const Shader& shader, const std::vector<std::string_view> flags = {}) const;

  void draw_instanced(const Shader& shader, int num_times, const Textures& textures,
                      const std::vector<std::string_view> flags = {}) const;
  void draw_instanced(const Shader& shader, int num_times,
                      const std::vector<std::string_view> flags = {}) const;

private:
  unsigned int VAO, VBO, EBO;
  int num_vertices, num_indices;

  static unsigned int UBO;
};

#endif // OBJECT_H
