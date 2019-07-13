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

layout (std140, binding = 2) uniform Lights {
    vec3 view_position;
    DirLight dir_light[5];
    PointLight point_light[5];
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
    vec3 color = point_light[0].ambient + point_light[0].diffuse + point_light[0].specular;
    frag_color = vec4(color, 1.0);
    bright_color = vec4(filter_bright_colors(color), 1.0);
}
