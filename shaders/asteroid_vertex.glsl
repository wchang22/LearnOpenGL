#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_texture_coords;
layout (location = 3) in mat4 in_model;

out vec2 texture_coords;

layout (std140, binding = 0) uniform Matrices {
    uniform mat4 view;
    uniform mat4 perspective;
};

void main() {
    texture_coords = in_texture_coords;

    gl_Position = perspective * view * in_model * vec4(in_position, 1.0);
}
