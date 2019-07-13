#ifndef MODEL_H
#define MODEL_H

#include <assimp/scene.h>

#include "shader/shader.h"
#include "model/mesh.h"

typedef Mesh::Vertex Vertex;
typedef glm::mat4 mat4;

class Model
{
public:
  Model(const char* path);

  void draw(const Shader& shader, std::initializer_list<std::string_view> flags = {}) const;

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
