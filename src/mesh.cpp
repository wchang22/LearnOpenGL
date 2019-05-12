#include "mesh.h"
#include "exception.h"

#include <utility>

#include <glad/glad.h>

Mesh::Mesh(const std::vector<Vertex>& vertices,
           const std::vector<unsigned int>& indices,
           const std::vector<Texture>& textures)
  : vertices(std::move(vertices)), indices(std::move(indices)), textures(std::move(textures))
{
  setup_mesh();
}

Mesh::~Mesh()
{
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
}

void Mesh::draw(const Shader& shader)
{
  unsigned int num_diffuse = 1;
  unsigned int num_specular = 1;

  for (unsigned int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    std::string name = textures[i].type;

    if (name == "texture_diffuse") {
      number = std::to_string(num_diffuse++);
    } else if (name == "texture_specular") {
      number = std::to_string(num_specular++);
    } else {
      throw Exception::DisplayException("Invalid texture type");
    }

    glUniform1f(shader.get_uniform_location((name + number).c_str()), i);
  }

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, static_cast<int>(indices.size()),
                 GL_UNSIGNED_INT, reinterpret_cast<void*>(0));
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

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
