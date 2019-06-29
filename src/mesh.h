#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "shader.h"
#include "textures.h"

typedef glm::vec4 vec4;
typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

class Mesh
{
public:
  struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
    vec3 tangent;
  };

  Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Textures&& textures);
  Mesh(Mesh&& other);
  ~Mesh();

  void add_instanced_data(unsigned int vertex_attrib_pointer, size_t data_size,
                          unsigned int data_multiple);
  void draw_instanced(const Shader& shader, unsigned int num_times) const;

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Textures textures;

private:
  void setup_mesh();

  unsigned int VAO, VBO, EBO;
};

#endif // MESH_H
