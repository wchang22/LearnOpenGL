#version 450 core

in vec4 frag_pos;

layout (std140, binding = 9) uniform PointShadow {
  mat4 shadow_matrices[6];
  vec3 light_position;
  float far_plane;
};

void main() {
    float light_distance = distance(frag_pos.xyz, light_position) / far_plane;
    gl_FragDepth = light_distance;
}
