#include "display.h"
#include "exception.h"
#include "window.h"

#include <cmath>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>
#include <omp.h>

typedef glm::mat4 mat4;
typedef glm::vec3 vec3;

const float vertices[] = {
  -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,

  -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
   0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
   0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

  -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
   0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
   0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
};

static unsigned int indices[36] = {};

const int NUM_CUBES = 2000;
static glm::vec3 cubePositions[NUM_CUBES];

static const char* texture_path_0 = "../../assets/container.jpg";
static const char* texture_path_1 = "../../assets/awesomeface.png";

Display::Display(std::shared_ptr<Camera> camera)
  : shaders(std::make_unique<Shader>("../../shaders/vertex.glsl", "../../shaders/fragment.glsl")),
    camera(camera)
{
  srand(static_cast<unsigned int>(time(nullptr)));

  init_textures();
  init_buffers();
  init_shaders();

  glBindVertexArray(VAO);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, textures[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, textures[1]);
}

Display::~Display() {
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteVertexArrays(1, &VAO);
  glDeleteTextures(num_textures, textures);
}

void Display::draw() const {
  static const int num_vertices = sizeof (indices) / sizeof(unsigned int);
  static const int model_location = shaders->get_uniform_location("model");
  const double time = glfwGetTime();

  mat4 view = camera->lookat();
  mat4 perspective = camera->perspective();

  glUniformMatrix4fv(shaders->get_uniform_location("perspective"),
                     1, GL_FALSE, &perspective[0][0]);
  glUniformMatrix4fv(shaders->get_uniform_location("view"),
                     1, GL_FALSE, &view[0][0]);

  mat4 rot[NUM_CUBES];

  #pragma omp parallel for
  for (int i = 0; i < NUM_CUBES; i++) {
    rot[i] = glm::rotate(mat4(1.0f), glm::radians(20.0f * i) + static_cast<float>(time),
                         vec3(1.0f, 0.3f, 0.5f));
  }

  #pragma omp parallel for
  for (int i = 0; i < NUM_CUBES; i++) {
    mat4 model = glm::translate(mat4(1.0f), cubePositions[i]) * rot[i];

    glUniformMatrix4fv(model_location, 1, GL_FALSE, &model[0][0]);

    glDrawElements(GL_TRIANGLES, num_vertices, GL_UNSIGNED_INT, buffer_offset(0));
  }
}

void Display::init_buffers() {
  for (unsigned int i = 0; i < 6; i++) {
    unsigned int index_bases[] = {2, 3, 0, 2, 1, 0};
    for (unsigned int j = 0; j < 6; j++) {
      indices[i * 6 + j] = index_bases[j] + i * 4;
    }
  }

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  const int position_size = 3;
  const int texture_size = 2;
  const int vertex_stride = (position_size + texture_size) * sizeof (float);

  const unsigned int position_location = 0;
  const void* position_offset = buffer_offset(0);
  glVertexAttribPointer(position_location, position_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, position_offset);
  glEnableVertexAttribArray(position_location);

  const unsigned int texture_location = 2;
  const void* texture_offset = buffer_offset((position_size) * sizeof(float));
  glVertexAttribPointer(texture_location, texture_size, GL_FLOAT, GL_FALSE,
                        vertex_stride, texture_offset);
  glEnableVertexAttribArray(texture_location);

  glBindVertexArray(0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  for (int i = 0; i < NUM_CUBES; i++) {
    cubePositions[i] = vec3(rand() % 100 - 50, rand() % 100 - 50, rand() % 100 - 50);
  }
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
  return reinterpret_cast<void*>(offset);
}
