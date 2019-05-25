#version 450 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in V_DATA {
    vec3 normal;
    vec3 position;
    vec2 texture_coords;
} gs_in[];

in vec3 c_normal[];

out V_DATA {
    out vec3 normal;
    out vec3 position;
    out vec2 texture_coords;
} gs_out;

uniform float time;

vec3 get_normal()
{
   vec3 a = vec3(gl_in[0].gl_Position - gl_in[1].gl_Position);
   vec3 b = vec3(gl_in[2].gl_Position - gl_in[1].gl_Position);
   return normalize(cross(a, b));
}

vec4 explode(vec4 position, vec3 normal) {
    float magnitude = 2.0;
    vec3 direction = magnitude * clamp((10 * sin(time) + 10.0 / 2.0), 0, 20) * normal;
    return position + vec4(direction, 0.0);
}

void gen_vertex(int index) {
    gl_Position = gl_in[index].gl_Position;
    gs_out.normal = gs_in[index].normal;
    gs_out.position = gs_in[index].position;
    gs_out.texture_coords = gs_in[index].texture_coords;
    EmitVertex();

    gl_Position = gl_in[index].gl_Position + vec4(c_normal[index], 0.0) * 0.1;
    gs_out.normal = gs_in[index].normal;
    gs_out.position = gs_in[index].position;
    gs_out.texture_coords = gs_in[index].texture_coords;
    EmitVertex();

    EndPrimitive();
}

void main() {
    for (int i = 0; i < 3; i++) {
        gen_vertex(i);
    }
}
