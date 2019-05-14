#ifndef MODEL_H
#define MODEL_H

#include <string>

#include <assimp/scene.h>

#include "shader.h"
#include "mesh.h"

typedef Mesh::Vertex Vertex;

class Model
{
public:
  Model(const char* path);

  void draw(const Shader& shader) const;

private:
  void load_model(const std::string& path);
  void process_node(aiNode* node, const aiScene* scene);
  Mesh process_mesh(aiMesh* mesh, const aiScene* scene);
  Textures load_material_textures(aiMaterial* material, aiTextureType type,
                                              const std::string& type_name);

  std::vector<Mesh> meshes;
  std::string directory;
};

#endif // MODEL_H
