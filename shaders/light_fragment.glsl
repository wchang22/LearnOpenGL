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

out vec4 frag_color;

void main() {
    frag_color = vec4(point_light.ambient + point_light.diffuse + point_light.specular, 1.0);
}
