#version 450 core

#define NUM_POINT_LIGHTS 4

uniform float material_shininess;
uniform sampler2D material_diffuse;
uniform sampler2D material_specular;

struct SpotLight {
    vec3 position;
    vec3 direction;

    float inner_cutoff;
    float outer_cutoff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

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

in vec3 normal;
in vec3 position;
in vec2 texture_coords;

out vec4 frag_color;

uniform vec3 view_position;

uniform SpotLight spotlight;
uniform DirLight dir_light;
uniform PointLight point_light[NUM_POINT_LIGHTS];

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 eye_direction,
                    vec3 diffuse_texture, vec3 specular_texture, float shininess) {
    vec3 light_direction = normalize(-light.direction);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), shininess);

    return ambient + diffuse + specular;
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 eye_direction,
                      vec3 diffuse_texture, vec3 specular_texture, float shininess) {
    float light_distance = distance(light.position, frag_position);
    float attenuation = 1.0 / (light.attenuation[0] +
                               light.attenuation[1] * light_distance +
                               light.attenuation[2] * light_distance * light_distance);

    vec3 light_direction = normalize(light.position - frag_position);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), shininess);

    return (ambient + diffuse + specular) * attenuation;
}

vec3 calc_spotlight(SpotLight light, vec3 normal, vec3 frag_position, vec3 eye_direction,
                     vec3 diffuse_texture, vec3 specular_texture, float shininess) {
    float light_distance = distance(light.position, frag_position);
    float attenuation = 1.0 / (light.attenuation[0] +
                               light.attenuation[1] * light_distance +
                               light.attenuation[2] * light_distance * light_distance);

    vec3 light_direction = normalize(light.position - frag_position);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), shininess);

    float cos_theta = dot(-light_direction, normalize(light.direction));
    float epsilon = light.inner_cutoff - light.outer_cutoff;
    float intensity = clamp((cos_theta - light.outer_cutoff) / epsilon, 0.0, 1.0);

    return (ambient + diffuse + specular) * attenuation * intensity;
}

void main() {
    vec3 normal = normalize(normal);
    vec3 eye_direction = normalize(view_position - position);

    vec3 diffuse_texture = texture(material_diffuse, texture_coords).rgb;
    vec3 specular_texture = texture(material_specular, texture_coords).rgb;

    vec3 color = calc_dir_light(dir_light, normal, eye_direction,
                                diffuse_texture, specular_texture, material_shininess);

    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
        color += calc_point_light(point_light[i], normal, position, eye_direction,
                                  diffuse_texture, specular_texture, material_shininess);
    }

    color += calc_spotlight(spotlight, normal, position, eye_direction,
                            diffuse_texture, specular_texture, material_shininess);

    frag_color = vec4(color, 1.0);
}
