#include "mesh.h"
#include "util/exception.h"

Mesh::Mesh(std::vector<Vertex>&& vertices,
           std::vector<unsigned int>&& indices,
           Textures&& textures)
  : textures(std::move(textures))
{
  mesh.start_setup();
  mesh.add_vertices(vertices.data(), static_cast<int>(vertices.size()),
                    vertices.size() * sizeof(Vertex));
  mesh.add_indices(indices.data(), static_cast<int>(indices.size()),
                   indices.size() * sizeof(unsigned int));
  mesh.add_vertex_attribs({ 3, 3, 2, 3, 3 });
  mesh.finalize_setup();
}

Mesh::Mesh(Mesh&& other) noexcept
  : textures(std::move(other.textures)),
    mesh(std::move(other.mesh))
{
}

void Mesh::draw(const Shader& shader, std::initializer_list<std::string_view> flags) const
{
  mesh.draw(shader, textures, flags);
}
