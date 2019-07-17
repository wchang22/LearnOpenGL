#include "textures.h"
#include "util/exception.h"

#include <algorithm>
#include <unordered_map>

#include <stb_image/stb_image.h>

Textures::~Textures() {
  clear();
}

Textures::Textures(Textures&& other) noexcept
  : texture_ids(std::move(other.texture_ids)),
    texture_paths(std::move(other.texture_paths)),
    texture_types(std::move(other.texture_types))
{
  other.texture_ids.clear();
}

Textures& Textures::operator=(Textures&& other) noexcept
{
  texture_ids = std::move(other.texture_ids);
  texture_paths = std::move(other.texture_paths);
  texture_types = std::move(other.texture_types);
  other.texture_ids.clear();
  return *this;
}

void Textures::load_texture_from_image(std::string_view path, std::string_view type) {
  if (auto it = std::find(texture_paths.begin(), texture_paths.end(), path);
      it != texture_paths.end()) {
    unsigned int i = static_cast<unsigned int>(std::distance(texture_paths.begin(), it));
    texture_ids.emplace_back(texture_ids[i]);
    texture_paths.emplace_back(texture_paths[i]);
    texture_types.emplace_back(texture_types[i]);
    return;
  }

  int width, height, num_channels;
  unsigned char* image_data = stbi_load(path.data(), &width, &height, &num_channels, 0);

  if (!image_data) {
    throw TextureException("Failed to load texture from " + std::string(path));
  }

  constexpr int mipmap_level = 0;
  const int texture_type = (type == "texture_normal" || type == "texture_height")
                            ? GL_RGBA : GL_SRGB_ALPHA;
  const GLenum image_type = GL_UNSIGNED_BYTE;
  GLenum image_format;

  switch (num_channels) {
    case 1:
      image_format = GL_RED;
      break;
    case 3:
      image_format = GL_RGB;
      break;
    case 4:
      image_format = GL_RGBA;
      break;
    default:
      stbi_image_free(image_data);
      throw TextureException("Invalid image type from " + std::string(path));
  }

  texture_types.emplace_back(type);
  texture_paths.emplace_back(path);
  texture_ids.emplace_back(0);
  glGenTextures(1, &texture_ids.back());

  glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(texture_ids.size() - 1));
  glBindTexture(GL_TEXTURE_2D, texture_ids.back());
  glTexImage2D(GL_TEXTURE_2D, mipmap_level, texture_type,
               width, height, 0, image_format, image_type, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image_data);
}

void Textures::load_cubemap(std::initializer_list<const char*> faces)
{
  texture_types.emplace_back("texture_cubemap");
  texture_paths.emplace_back("");
  texture_ids.emplace_back(0);

  glGenTextures(1, &texture_ids.back());
  glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(texture_ids.size() - 1));
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ids.back());

  int width, height, num_channels;

  unsigned int i = 0;
  for (const auto path : faces) {
    unsigned char* image_data = stbi_load(path, &width, &height, &num_channels, 0);

    if (!image_data) {
      throw TextureException("Failed to load cubemap texture from " + std::string(path));
    }

    constexpr int mipmap_level = 0;
    constexpr int texture_type = GL_SRGB;
    constexpr int image_format = GL_RGB;
    constexpr GLenum image_type = GL_UNSIGNED_BYTE;

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i++, mipmap_level, texture_type,
                 width, height, 0, image_format, image_type, image_data);
    stbi_image_free(image_data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Textures::add_texture(std::string_view type, unsigned int id)
{
  texture_ids.emplace_back(id);
  texture_paths.emplace_back("");
  texture_types.emplace_back(type);
  external_ids.emplace(id);
}

void Textures::use_textures(const Shader& shader) const {
  std::unordered_map<std::string, unsigned int> texture_counts;

  for (unsigned int i = 0; i < texture_ids.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    const std::string& name = texture_types[i];

    auto it = texture_counts.find(name);

    if (it == texture_counts.end()) {
      glUniform1i(shader.get_uniform_location(name + "1"), static_cast<GLint>(i));
      texture_counts[name] = 1;
    } else {
      glUniform1i(shader.get_uniform_location(name + std::to_string(++it->second)),
                  static_cast<GLint>(i));
    }

    if (name == "texture_cubemap") {
      glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ids[i]);
    } else {
      glBindTexture(GL_TEXTURE_2D, texture_ids[i]);
    }
  }
}

void Textures::append(Textures&& other)
{
  texture_ids.reserve(other.texture_ids.size());
  texture_ids.insert(texture_ids.end(),
                     std::make_move_iterator(other.texture_ids.begin()),
                     std::make_move_iterator(other.texture_ids.end()));
  texture_paths.reserve(other.texture_paths.size());
  texture_paths.insert(texture_paths.end(),
                       std::make_move_iterator(other.texture_paths.begin()),
                       std::make_move_iterator(other.texture_paths.end()));
  texture_types.reserve(other.texture_types.size());
  texture_types.insert(texture_types.end(),
                       std::make_move_iterator(other.texture_types.begin()),
                       std::make_move_iterator(other.texture_types.end()));

  other.texture_ids.clear();
}

void Textures::append(const Textures& other)
{
  texture_ids.reserve(other.texture_ids.size());
  texture_ids.insert(texture_ids.end(),
                     other.texture_ids.begin(),
                     other.texture_ids.end());
  texture_paths.reserve(other.texture_paths.size());
  texture_paths.insert(texture_paths.end(),
                       other.texture_paths.begin(),
                       other.texture_paths.end());
  texture_types.reserve(other.texture_types.size());
  texture_types.insert(texture_types.end(),
                       other.texture_types.begin(),
                       other.texture_types.end());
}

void Textures::clear()
{
  for (auto id : texture_ids) {
    if (external_ids.find(id) == external_ids.end()) {
      glDeleteTextures(1, &id);
    }
  }

  texture_ids.clear();
  texture_paths.clear();
  texture_types.clear();
  external_ids.clear();
}

size_t Textures::size() const
{
  return texture_ids.size();
}
