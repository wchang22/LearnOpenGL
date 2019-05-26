#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 2) in vec2 in_texture_coords;

out vec2 texture_coords;

layout (std140, binding = 0) uniform Matrices {
    uniform mat4 view_perspective;
};

layout (std140, binding = 1) buffer Model {
    mat4 model[];
};

void main() {
    texture_coords = in_texture_coords;

    gl_Position = view_perspective * model[gl_InstanceID] * vec4(in_position, 1.0);
}
