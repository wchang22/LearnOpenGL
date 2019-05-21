#include "model.h"
#include "exception.h"

#include <utility>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Model::Model(const char* path)
{
  load_model(path);
}

void Model::draw(const Shader& shader) const
{
  for (const Mesh& mesh : meshes) {
    mesh.draw(shader);
  }
}

void Model::load_model(const std::string& path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

  if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    throw Exception::ModelException((std::string("Assimp Error: ") +
                                     importer.GetErrorString()).c_str());
  }

  directory.insert(directory.begin(), std::make_move_iterator(path.begin()),
                   std::make_move_iterator(path.begin() +
                                           static_cast<long>(path.find_last_of('/')) + 1));

  process_node(scene->mRootNode, scene);
}

void Model::process_node(aiNode* node, const aiScene* scene)
{
  for (unsigned int i = 0; i < node->mNumMeshes; i++) {
    aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.emplace_back(process_mesh(mesh, scene));
  }

  for (unsigned int i = 0; i < node->mNumChildren; i++) {
    process_node(node->mChildren[i], scene);
  }
}

Mesh Model::process_mesh(aiMesh* mesh, const aiScene* scene)
{
  std::vector<Vertex> vertices;
  std::vector<unsigned int> indices;
  Textures textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

    if (mesh->mTextureCoords[0]) {
      vertex.texture_coords = vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
    } else {
      vertex.texture_coords = vec2(0.0f);
    }

    vertices.emplace_back(std::move(vertex));
  }

  for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
    aiFace face = mesh->mFaces[i];

    for (unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.emplace_back(face.mIndices[j]);
    }
  }

  aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
  Textures diffuse_maps = load_material_textures(material, aiTextureType_DIFFUSE,
                                                 "texture_diffuse");
  Textures specular_maps = load_material_textures(material, aiTextureType_SPECULAR,
                                                  "texture_specular");

  textures.append(std::move(diffuse_maps));
  textures.append(std::move(specular_maps));

  return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

Textures Model::load_material_textures(aiMaterial* material, aiTextureType type,
                                       const std::string& type_name)
{
  Textures textures;

  for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
    aiString path;
    material->GetTexture(type, i, &path);
    textures.load_texture_from_image((directory + path.C_Str()).c_str(), type_name);
  }

  return textures;
}