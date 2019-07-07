# version 450 core

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

layout (std140, binding = 1) uniform Lights {
    DirLight dir_light;
    PointLight point_light;
    vec3 view_position;
    float material_shininess;
};

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

vec3 filter_bright_colors(vec3 color) {
    if (dot(color, vec3(0.2126, 0.7152, 0.0722)) > 1.0) {
        return color;
    } else {
        return vec3(0.0);
    }
}

void main() {
    vec3 color = point_light.ambient + point_light.diffuse + point_light.specular;
    frag_color = vec4(color, 1.0);
    bright_color = vec4(filter_bright_colors(color), 1.0);
}
