#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_texture_coords;

uniform mat4 model;

out vec2 texture_coords;

layout (std140, binding = 0) uniform Matrices {
    uniform mat4 view_perspective;
};

void main() {
    texture_coords = in_texture_coords;

    gl_Position = view_perspective * model * vec4(in_position, 1.0);
}
