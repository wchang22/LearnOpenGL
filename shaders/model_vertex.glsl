#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coords;

out VS_OUT {
    vec3 normal;
    vec3 position;
    vec2 texture_coords;
} vs_out;

uniform mat4 model;

layout (std140, binding = 0) uniform Matrices {
    uniform mat4 view;
    uniform mat4 perspective;
};

void main() {
    vs_out.position = vec3(model * vec4(in_position, 1.0));
    vs_out.normal = mat3(transpose(inverse(model))) * in_normal;
    vs_out.texture_coords = in_texture_coords;

    gl_Position = perspective * view * model * vec4(in_position, 1.0);
}
