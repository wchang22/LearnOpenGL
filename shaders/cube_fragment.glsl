#version 450 core

struct Material {
    float shininess;
};

uniform sampler2D diffuse;
uniform sampler2D specular;

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 position;
in vec2 texture_coords;

out vec4 frag_color;

uniform vec3 view_position;

uniform Material material;
uniform Light light;

void main() {
    vec3 normal = normalize(normal);

    vec3 light_direction = normalize(light.position - position);
    vec3 eye_direction = normalize(view_position - position);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 diffuse_texture = texture(diffuse, texture_coords).rgb;
    vec3 specular_diffuse = texture(specular, texture_coords).rgb;

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture *
                    max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_diffuse *
                    pow(max(dot(normal, half_vec), 0.0), material.shininess);

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
