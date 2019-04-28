#include "display.h"
#include "exception.h"

#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

const float vertices[] = {
  // positions          // colors           // texture coords
   0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
   0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
  -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
  -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
};

const unsigned int indices[] = {
  0, 1, 3,
  1, 2, 3,
};

static const char* texture_path_0 = "../assets/container.jpg";
static const char* texture_path_1 = "../assets/awesomeface.png";

Display::Display()
  : shaders(std::make_unique<Shader>("../shaders/vertex.glsl", "../shaders/fragment.glsl"))
{
  init_textures();
  init_buffers();
  init_shaders();
}

Display::~Display() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteTextures(num_textures, textures);
}

void Display::draw() const {
  const int num_vertices = sizeof (indices) / sizeof(unsigned int);

  const double time = glfwGetTime();
  const float color_offset = static_cast<float>(sin(time) / 2.0);
  glUniform4f(shaders->get_uniform_location("color"),
              color_offset, color_offset, color_offset, 1.0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);

  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, buffer_offset(0));
}

void Display::init_buffers() {
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  const int position_size = 3;
  const int color_size = 3;
  const int texture_size = 2;
  const int vertex_stride = (position_size + color_size + texture_size) * sizeof (float);

  const unsigned int position_location = 0;
  const void* position_offset = buffer_offset(0);
  glVertexAttribPointer(position_location, position_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, position_offset);
  glEnableVertexAttribArray(position_location);

  const unsigned int color_location = 1;
  const void* color_offset = buffer_offset(position_size * sizeof(float));
  glVertexAttribPointer(color_location, color_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, color_offset);
  glEnableVertexAttribArray(color_location);

  const unsigned int texture_location = 2;
  const void* texture_offset = buffer_offset((position_size + color_size) * sizeof(float));
  glVertexAttribPointer(texture_location, texture_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, texture_offset);
  glEnableVertexAttribArray(texture_location);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Display::init_textures() {
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  stbi_set_flip_vertically_on_load(true);

  int width, height, num_channels;
  unsigned char* image_0_data = stbi_load(texture_path_0,
                                        &width, &height, &num_channels, 0);

  if (!image_0_data) {
    throw Exception::DisplayException((std::string("Failed to load texture from") +
                                      std::string(texture_path_0)).c_str());
  }

  unsigned char* image_1_data = stbi_load(texture_path_1,
                                        &width, &height, &num_channels, 0);

  if (!image_1_data) {
    stbi_image_free(image_1_data);
    throw Exception::DisplayException((std::string("Failed to load texture from") +
                                      std::string(texture_path_1)).c_str());
  }

  const int mipmap_level = 0;
  const int texture_type = GL_RGB;
  const int image_0_format = GL_RGB;
  const int image_1_format = GL_RGBA;
  const int image_type = GL_UNSIGNED_BYTE;

  glGenTextures(num_textures, textures);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glTexImage2D(GL_TEXTURE_2D, mipmap_level, texture_type,
               width, height, 0, image_0_format, image_type, image_0_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
  glTexImage2D(GL_TEXTURE_2D, mipmap_level, texture_type,
               width, height, 0, image_1_format, image_type, image_1_data);
  glGenerateMipmap(GL_TEXTURE_2D);

  glBindTexture(GL_TEXTURE_2D, 0);

  stbi_image_free(image_0_data);
  stbi_image_free(image_1_data);
}

void Display::init_shaders() {
  shaders->use_shader_program();

  glUniform1i(shaders->get_uniform_location("texture0"), 0);
  glUniform1i(shaders->get_uniform_location("texture1"), 1);
}

void* Display::buffer_offset(int offset) {
  return static_cast<void*>(static_cast<char*>(nullptr) + offset);
}
