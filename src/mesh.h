#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "shader.h"

typedef glm::vec3 vec3;
typedef glm::vec2 vec2;

class Mesh
{
public:
  struct Vertex {
    vec3 position;
    vec3 normal;
    vec2 texture_coords;
  };

  struct Texture {
    unsigned int id;
    std::string type;
  };

  Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices,
       const std::vector<Texture>& textures);
  ~Mesh();

  void draw(const Shader& shader);

  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  std::vector<Texture> textures;

private:
  void setup_mesh();

  unsigned int VAO, VBO, EBO;
};

#endif // MESH_H
