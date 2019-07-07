#version 450 core
layout (location = 0) in vec3 in_position;
layout (location = 1) in vec3 in_normal;
layout (location = 2) in vec2 in_texture_coords;
layout (location = 3) in vec3 in_tangent;

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

out V_DATA {
    vec3 position;
    vec2 texture_coords;
    vec3 tangent_light_pos;
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} vs_out;

layout (std140, binding = 0) uniform Matrices {
    mat4 world_space;
    mat4 model;
};

layout (std140, binding = 1) uniform Lights {
    DirLight dir_light;
    PointLight point_light;
    vec3 view_position;
    float material_shininess;
};

void main() {
    vs_out.position = vec3(model * vec4(in_position, 1.0));
    vs_out.texture_coords = in_texture_coords;

    mat3 normal_mat = transpose(inverse(mat3(model)));
    vec3 t = normalize(normal_mat * in_tangent);
    vec3 n = normalize(normal_mat * in_normal);
    t = normalize(t - dot(n, t) * n);
    vec3 b = cross(n, t);

    mat3 tbn = transpose(mat3(t, b, n));
    vs_out.tangent_light_pos = tbn * point_light.position;
    vs_out.tangent_view_pos = tbn * view_position;
    vs_out.tangent_frag_pos = tbn * vs_out.position;

    gl_Position = world_space * model * vec4(in_position, 1.0);
}
