#ifndef DATA_H
#define DATA_H

#include <glm/glm.hpp>

#include <iterator>

typedef glm::vec2 vec2;
typedef glm::vec3 vec3;
typedef glm::mat2 mat2;
typedef glm::mat4 mat4;
typedef glm::mat3x2 mat3x2;

constexpr int NUM_AA_SAMPLES = 8;

inline void generate_cube_vertices(const float in[192], float out[504])
{
  for (unsigned int quad = 0; quad < 6; quad++) {
    unsigned int in_quad_offset = quad * 4 * 8;
    unsigned int out_quad_offset = quad * 6 * 14;

    vec3 pos[4];
    vec3 nm[4];
    vec2 uv[4];
    vec3 edge[2];
    vec2 delta_uv[2];
    vec3 tangent[2];
    vec3 bitangent[2];

    for (unsigned int vertex = 0; vertex < 4; vertex++) {
      unsigned int in_pos_offset = in_quad_offset + vertex * 8;
      pos[vertex] = vec3(in[in_pos_offset], in[in_pos_offset + 1], in[in_pos_offset + 2]);

      unsigned int in_nm_offset = in_pos_offset + 3;
      nm[vertex] = vec3(in[in_nm_offset], in[in_nm_offset + 1], in[in_nm_offset + 2]);

      unsigned int in_uv_offset = in_nm_offset + 3;
      uv[vertex] = vec2(in[in_uv_offset], in[in_uv_offset + 1]);
    }

    edge[0] = pos[1] - pos[0];
    edge[1] = pos[2] - pos[0];
    delta_uv[0] = uv[1] - uv[0];
    delta_uv[1] = uv[2] - uv[0];

    float f = 1.0f / (delta_uv[0].x * delta_uv[1].y - delta_uv[1].x * delta_uv[0].y);
    mat2 uv_adj_mat(delta_uv[1].y, -delta_uv[1].x, -delta_uv[0].y, delta_uv[0].x);
    mat3x2 edge_mat(edge[0].x, edge[1].x, edge[0].y, edge[1].y, edge[0].z, edge[1].z);
    mat3x2 tb_mat = f * uv_adj_mat * edge_mat;

    tangent[0] = glm::normalize(vec3(tb_mat[0][0], tb_mat[1][0], tb_mat[2][0]));
    bitangent[0] = glm::normalize(vec3(tb_mat[0][1], tb_mat[1][1], tb_mat[2][1]));

    edge[0] = pos[2] - pos[0];
    edge[1] = pos[3] - pos[0];
    delta_uv[0] = uv[2] - uv[0];
    delta_uv[1] = uv[3] - uv[0];

    f = 1.0f / (delta_uv[0].x * delta_uv[1].y - delta_uv[1].x * delta_uv[0].y);
    uv_adj_mat = mat2(delta_uv[1].y, -delta_uv[1].x, -delta_uv[0].y, delta_uv[0].x);
    edge_mat = mat3x2(edge[0].x, edge[1].x, edge[0].y, edge[1].y, edge[0].z, edge[1].z);
    tb_mat = f * uv_adj_mat * edge_mat;

    tangent[1] = glm::normalize(vec3(tb_mat[0][0], tb_mat[1][0], tb_mat[2][0]));
    bitangent[1] = glm::normalize(vec3(tb_mat[0][1], tb_mat[1][1], tb_mat[2][1]));

    float quad_vertices[] = {
      pos[0].x, pos[0].y, pos[0].z, nm[0].x, nm[0].y, nm[0].z, uv[0].x, uv[0].y,
      tangent[0].x, tangent[0].y, tangent[0].z, bitangent[0].x, bitangent[0].y, bitangent[0].z,

      pos[1].x, pos[1].y, pos[1].z, nm[1].x, nm[1].y, nm[1].z, uv[1].x, uv[1].y,
      tangent[0].x, tangent[0].y, tangent[0].z, bitangent[0].x, bitangent[0].y, bitangent[0].z,

      pos[2].x, pos[2].y, pos[2].z, nm[2].x, nm[2].y, nm[2].z, uv[2].x, uv[2].y,
      tangent[0].x, tangent[0].y, tangent[0].z, bitangent[0].x, bitangent[0].y, bitangent[0].z,


      pos[0].x, pos[0].y, pos[0].z, nm[0].x, nm[0].y, nm[0].z, uv[0].x, uv[0].y,
      tangent[1].x, tangent[1].y, tangent[1].z, bitangent[1].x, bitangent[1].y, bitangent[1].z,

      pos[2].x, pos[2].y, pos[2].z, nm[2].x, nm[2].y, nm[2].z, uv[2].x, uv[2].y,
      tangent[1].x, tangent[1].y, tangent[1].z, bitangent[1].x, bitangent[1].y, bitangent[1].z,

      pos[3].x, pos[3].y, pos[3].z, nm[3].x, nm[3].y, nm[3].z, uv[3].x, uv[3].y,
      tangent[1].x, tangent[1].y, tangent[1].z, bitangent[1].x, bitangent[1].y, bitangent[1].z
    };

    std::copy(std::begin(quad_vertices), std::end(quad_vertices), out + out_quad_offset);
  }
}

constexpr float CUBE_VERTICES[] = {
  -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
   0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
  -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

  -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
   0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

  -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
  -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
  -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

   0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
   0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
   0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

  -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
   0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
   0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

  -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
   0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
   0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
  -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
};

constexpr unsigned int CUBE_INDICES[] = {
  0, 1, 2, 2, 3, 0,
  4, 5, 6, 6, 7, 4,
  8, 9, 10, 10, 11, 8,
  12, 13, 14, 14, 15, 12,
  16, 17, 18, 18, 19, 16,
  20, 21, 22, 22, 23, 20,
};

constexpr float SKYBOX_VERTICES[] = {
  // positions
  -1.0f,  1.0f, -1.0f,
  -1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f, -1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,

  -1.0f, -1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f, -1.0f,  1.0f,
  -1.0f, -1.0f,  1.0f,

  -1.0f,  1.0f, -1.0f,
   1.0f,  1.0f, -1.0f,
   1.0f,  1.0f,  1.0f,
   1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f,  1.0f,
  -1.0f,  1.0f, -1.0f,

  -1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f, -1.0f,
   1.0f, -1.0f, -1.0f,
  -1.0f, -1.0f,  1.0f,
   1.0f, -1.0f,  1.0f
};

constexpr float QUAD_VERTICES[] = {
  -1.0f,  1.0f,  0.0f, 1.0f,
  -1.0f, -1.0f,  0.0f, 0.0f,
   1.0f, -1.0f,  1.0f, 0.0f,

  -1.0f,  1.0f,  0.0f, 1.0f,
   1.0f, -1.0f,  1.0f, 0.0f,
   1.0f,  1.0f,  1.0f, 1.0f
};

#endif // DATA_H
