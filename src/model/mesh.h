#ifndef MESH_H
#define MESH_H

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "shader/shader.h"
#include "shader/textures.h"
#include "model/object.h"

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
    vec3 bitangent;
  };

  Mesh(std::vector<Vertex>&& vertices, std::vector<unsigned int>&& indices, Textures&& textures);
  Mesh(Mesh&& other) noexcept;

  void draw(const Shader& shader, std::initializer_list<std::string_view> flags = {}) const;

private:
  Textures textures;
  Object mesh;
};

static_assert (std::is_nothrow_move_constructible<Mesh>::value, "Mesh not move constructible");

#endif // MESH_H
