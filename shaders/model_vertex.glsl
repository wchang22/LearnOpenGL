#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coords;

out V_DATA {
    vec3 normal;
    vec3 position;
    vec2 texture_coords;
    vec4 position_light_space;
} vs_out;

layout (std140, binding = 0) uniform Matrices {
    mat4 world_space;
    mat4 light_space;
    mat4 model;
};

void main() {
    vs_out.position = vec3(model * vec4(in_position, 1.0));
    vs_out.normal = mat3(transpose(inverse(model))) * in_normal;
    vs_out.texture_coords = in_texture_coords;
    vs_out.position_light_space = light_space * vec4(vs_out.position, 1.0);

    gl_Position = world_space * model * vec4(in_position, 1.0);
}
