#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coords;
layout (location = 3) in vec3 in_tangent;
layout (location = 4) in vec3 in_bitangent;

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

layout (std140, binding = 0) uniform Matrices {
    mat4 perspective;
    mat4 view;
};

layout (std430, binding = 1) buffer Model {
    mat4 model[];
};

layout (std140, binding = 2) uniform Lights {
    vec3 view_position;
    int num_dir_lights;
    int num_point_lights;
};

layout (std140, binding = 3) buffer DirLights {
    DirLight dir_light[];
};

layout (std140, binding = 4) buffer PointLights {
    PointLight point_light[];
};

uniform bool reverse_normal;

out V_DATA {
    vec3 position;
    vec2 texture_coords;
    vec3 t;
    vec3 b;
    vec3 n;
} vs_out;

void main() {
    vs_out.position = vec3(model[gl_InstanceID] * vec4(in_position, 1.0));
    vs_out.texture_coords = in_texture_coords;

    mat3 normal_mat = transpose(inverse(mat3(model[gl_InstanceID])));
    vec3 t = normalize(normal_mat * in_tangent);
    vec3 n = normalize(normal_mat * in_normal) * (reverse_normal ? -1 : 1);
    vec3 b = normalize(normal_mat * in_bitangent);

    vs_out.t = t;
    vs_out.b = b;
    vs_out.n = n;

    gl_Position = perspective * view * model[gl_InstanceID] * vec4(in_position, 1.0);
}
