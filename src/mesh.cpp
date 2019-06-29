#include "mesh.h"
#include "exception.h"

#include <utility>

#include <glad/glad.h>

Mesh::Mesh(std::vector<Vertex>&& vertices,
           std::vector<unsigned int>&& indices,
           Textures&& textures)
  : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
  setup_mesh();
}

Mesh::Mesh(Mesh&& other)
  : vertices(std::move(other.vertices)),
    indices(std::move(other.indices)),
    textures(std::move(other.textures)),
    VAO(other.VAO),
    VBO(other.VBO),
    EBO(other.EBO)
{
  other.VAO = 0;
  other.VBO = 0;
  other.EBO = 0;
}

Mesh::~Mesh()
{
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
}

void Mesh::add_instanced_data(unsigned int vertex_attrib_pointer, size_t data_size,
                              unsigned int data_multiple)
{
  glBindVertexArray(VAO);

  int attrib_width = static_cast<int>(static_cast<float>(data_size) / sizeof (vec4) * 4);

  for (unsigned int i = vertex_attrib_pointer; i < vertex_attrib_pointer + data_multiple; i++) {
    glVertexAttribPointer(i, attrib_width, GL_FLOAT, GL_FALSE,
                          static_cast<int>(data_size * data_multiple),
                          reinterpret_cast<void*>((i - vertex_attrib_pointer) * data_size));
    glEnableVertexAttribArray(i);
    glVertexAttribDivisor(i, 1);
  }

  glBindVertexArray(0);
}

void Mesh::draw_instanced(const Shader& shader, unsigned int num_times) const
{
  textures.use_textures(shader);
  glBindVertexArray(VAO);
  glDrawElementsInstanced(GL_TRIANGLES, static_cast<int>(indices.size()),
                          GL_UNSIGNED_INT, reinterpret_cast<void*>(0), static_cast<int>(num_times));
  glBindVertexArray(0);
  glActiveTexture(GL_TEXTURE0);
}

void Mesh::setup_mesh()
{
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, static_cast<long>(vertices.size() * sizeof(Vertex)),
               &vertices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<long>(indices.size() * sizeof(unsigned int)),
               &indices[0], GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, position)));
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, normal)));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, texture_coords)));
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        reinterpret_cast<void*>(offsetof(Vertex, tangent)));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
