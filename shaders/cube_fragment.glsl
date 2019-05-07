#version 450 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in vec3 normal;
in vec3 position;

out vec4 frag_color;

uniform vec3 view_position;

uniform mat4 model;
uniform mat4 view;

uniform Material material;
uniform Light light;

void main() {
    vec3 model_position = vec3(model * vec4(position, 1.0));
    vec3 model_normal = normalize(mat3(transpose(inverse(model))) * normal);

    vec3 light_direction = normalize(light.position - model_position);
    vec3 eye_direction = normalize(view_position - model_position);
    vec3 half_vec = normalize(light_direction + eye_direction);

    vec3 ambient = light.ambient * material.ambient;
    vec3 diffuse = light.diffuse * material.diffuse *
                    vec3(max(dot(model_normal, light_direction), 0.0));
    vec3 specular = light.specular * material.specular *
                    vec3(pow(max(dot(model_normal, half_vec), 0.0), material.shininess));

    frag_color = vec4(ambient + diffuse + specular, 1.0);
}
