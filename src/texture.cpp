#include "texture.h"
#include "exception.h"

#include <stb_image/stb_image.h>

Texture::Texture()
  : textures()
{
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);
}

Texture::~Texture() {
  glDeleteTextures(static_cast<int>(textures.size()), textures.data());
}

void Texture::load_texture_from_image(const char* path, GLenum image_format) {
  int width, height, num_channels;
  unsigned char* image_data = stbi_load(path, &width, &height, &num_channels, 0);

  if (!image_data) {
    throw Exception::DisplayException((std::string("Failed to load texture from") +
                                      std::string(path)).c_str());
  }

  const int mipmap_level = 0;
  const int texture_type = GL_RGB;
  const GLenum image_type = GL_UNSIGNED_BYTE;

  textures.push_back(0);
  glGenTextures(1, &textures.back());

  glActiveTexture(GL_TEXTURE0 + static_cast<GLenum>(textures.size() - 1));
  glBindTexture(GL_TEXTURE_2D, textures.back());
  glTexImage2D(GL_TEXTURE_2D, mipmap_level, texture_type,
               width, height, 0, image_format, image_type, image_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image_data);
}

void Texture::use_textures() {
  for (unsigned int i = 0; i < textures.size(); i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures[i]);
  }
}
