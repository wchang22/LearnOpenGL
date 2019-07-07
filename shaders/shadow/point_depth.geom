#version 450 core

layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

layout (std140, binding = 9) uniform PointShadow {
  mat4 shadow_matrices[6];
  vec3 light_position;
  float far_plane;
};

out vec4 frag_pos;

void main() {
    for (int face = 0; face < 6; face++) {
        gl_Layer = face;

        for (int i = 0; i < 3; i++) {
            frag_pos = gl_in[i].gl_Position;
            gl_Position = shadow_matrices[face] * frag_pos;
            EmitVertex();
        }

        EndPrimitive();
    }
}
