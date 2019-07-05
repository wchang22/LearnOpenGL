#ifndef MODEL_H
#define MODEL_H

#include <string>

#include <assimp/scene.h>

#include "shader/shader.h"
#include "model/mesh.h"

typedef Mesh::Vertex Vertex;
typedef glm::mat4 mat4;

class Model
{
public:
  Model(const char* path);

  void add_instanced_array(void* array, size_t array_element_size, unsigned int amount,
                           unsigned int vertex_attrib_pointer);

  void draw(const Shader& shader) const;
  void draw_instanced(const Shader& shader, unsigned int num_times) const;

  std::vector<Mesh> meshes;

private:
  void load_model(std::string_view path);
  void process_node(aiNode* node, const aiScene* scene);
  Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
  Textures load_material_textures(aiMaterial* material, aiTextureType type,
                                  std::string_view type_name);

  std::string directory;
};

#endif // MODEL_H
