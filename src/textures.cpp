#include "textures.h"
#include "exception.h"

#include <algorithm>

#include <stb_image/stb_image.h>

Textures::Textures()
  : texture_ids(), texture_paths(), texture_types()
{
  //stbi_set_flip_vertically_on_load(true);
}

Textures::~Textures() {
  glDeleteTextures(static_cast<int>(texture_ids.size()), texture_ids.data());
}

Textures::Textures(Textures&& other)
  : texture_ids(std::move(other.texture_ids)),
    texture_paths(std::move(other.texture_paths)),
    texture_types(std::move(other.texture_types))
{
  other.texture_ids.clear();
}

void Textures::load_texture_from_image(const char* path, const std::string& type) {
  auto it = std::find(texture_paths.begin(), texture_paths.end(), std::string(path));

  if (it != texture_paths.end()) {
    unsigned int i = static_cast<unsigned int>(std::distance(texture_paths.begin(), it));
    texture_ids.emplace_back(texture_ids[i]);
    texture_paths.emplace_back(texture_paths[i]);
    texture_types.emplace_back(texture_types[i]);
    return;
  }

  int width, height, num_channels;
  unsigned char* image_data = stbi_load(path, &width, &height, &num_channels, 0);

  if (!image_data) {
    throw Exception::TextureException((std::string("Failed to load texture from ") +
                                      std::string(path)).c_str());
  }

  const int mipmap_level = 0;
  const int texture_type = GL_RGBA;
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
      throw Exception::TextureException((std::string("Invalid image type from ") +
                                         std::string(path)).c_str());
  }

  texture_types.emplace_back(std::move(type));
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

void Textures::load_cubemap(const std::vector<std::string>& faces)
{
  texture_types.emplace_back("texture_cubemap");
  texture_paths.emplace_back("");
  texture_ids.emplace_back(0);

  glGenTextures(1, &texture_ids.back());
  glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(texture_ids.size() - 1));
  glBindTexture(GL_TEXTURE_CUBE_MAP, texture_ids.back());

  int width, height, num_channels;

  for (unsigned int i = 0; i < faces.size(); i++) {
    const char* path = faces[i].c_str();
    unsigned char* image_data = stbi_load(path, &width, &height, &num_channels, 0);

    if (!image_data) {
      throw Exception::TextureException((std::string("Failed to load cubemap texture from ") +
                                         std::string(path)).c_str());
    }

    const int mipmap_level = 0;
    const int texture_type = GL_RGB;
    const GLenum image_type = GL_UNSIGNED_BYTE;

    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mipmap_level, texture_type,
                 width, height, 0, texture_type, image_type, image_data);
    stbi_image_free(image_data);
  }

  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Textures::use_textures(const Shader& shader) const {
  unsigned int num_diffuse = 1;
  unsigned int num_specular = 1;
  unsigned int num_cubemap = 1;

  for (unsigned int i = 0; i < texture_ids.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    std::string number;
    const std::string& name = texture_types[i];

    if (name == "texture_diffuse") {
      number = std::to_string(num_diffuse++);
    } else if (name == "texture_specular") {
      number = std::to_string(num_specular++);
    } else if (name == "texture_cubemap") {
      number = std::to_string(num_cubemap++);
    } else {
      throw Exception::TextureException("Invalid texture type");
    }

    glUniform1i(shader.get_uniform_location((name + number).c_str()), static_cast<GLint>(i));

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
