#include "model.h"
#include "util/exception.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Model::Model(const char* path)
{
  load_model(path);
}

void Model::draw(const Shader& shader, std::initializer_list<std::string_view> flags) const
{
  draw_instanced(shader, 1, flags);
}

void Model::draw_instanced(const Shader& shader, int num_times,
                           std::initializer_list<std::string_view> flags) const
{
  glEnable(GL_CULL_FACE);
  for (const Mesh& mesh : meshes) {
    mesh.draw_instanced(shader, num_times, flags);
  }
  glDisable(GL_CULL_FACE);
}

void Model::load_model(std::string_view path)
{
  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(path.data(),
                                           aiProcess_Triangulate |
                                           aiProcess_FlipUVs |
                                           aiProcess_CalcTangentSpace);

  if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
    throw ModelException(std::string("Assimp Error: ") + importer.GetErrorString());
  }

  directory = path.substr(0, static_cast<size_t>(path.find_last_of('/')) + 1);

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
  vertices.reserve(mesh->mNumVertices);
  std::vector<unsigned int> indices(mesh->mNumVertices);
  Textures textures;

  for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    vertex.position = vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
    vertex.normal = vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
    vertex.tangent = vec3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
    vertex.bitangent = vec3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

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
  Textures reflection_maps = load_material_textures(material, aiTextureType_AMBIENT,
                                                  "texture_reflection");
  Textures normal_maps = load_material_textures(material, aiTextureType_HEIGHT,
                                                "texture_normal");

  textures.append(std::move(diffuse_maps));
  textures.append(std::move(specular_maps));
  textures.append(std::move(reflection_maps));
  textures.append(std::move(normal_maps));

  return Mesh(std::move(vertices), std::move(indices), std::move(textures));
}

Textures Model::load_material_textures(aiMaterial* material, aiTextureType type,
                                       std::string_view type_name)
{
  Textures textures;

  for (unsigned int i = 0; i < material->GetTextureCount(type); i++) {
    aiString path;
    material->GetTexture(type, i, &path);
    textures.load_texture_from_image(directory + path.C_Str(), type_name);
  }

  return textures;
}
