#version 450 core

#define NUM_POINT_LIGHTS 4

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D shadow_map;

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

in V_DATA {
    in vec3 normal;
    in vec3 position;
    in vec2 texture_coords;
    in vec4 position_light_space;
} fs_in;

out vec4 frag_color;

layout (std140, binding = 1) uniform Lights {
    DirLight dir_light;
    PointLight point_light[NUM_POINT_LIGHTS];
    vec3 view_position;
    float material_shininess;
};

vec3 calc_dir_light(DirLight light, vec3 normal, vec3 eye_direction,
                    vec3 diffuse_texture, vec3 specular_texture, float shininess, float shadow) {
    vec3 light_direction = normalize(-light.direction);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), shininess);

    return ambient + (1 - shadow) * (diffuse + specular);
}

vec3 calc_point_light(PointLight light, vec3 normal, vec3 frag_position, vec3 eye_direction,
                      vec3 diffuse_texture, vec3 specular_texture, float shininess, float shadow) {
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

    return (ambient + (1 - shadow) * (diffuse + specular)) * attenuation;
}

float calc_shadow(DirLight light, vec3 normal, vec4 position_light_space) {
    vec3 coords = position_light_space.xyz / position_light_space.w * 0.5 + 0.5;

    if (coords.z > 1) {
        return 0;
    }

    float bias = max(0.05 * (1.0 - abs(dot(light.direction, normal))), 0.01);

    float shadow = 0.0;
    vec2 texel_size = 1.5 / textureSize(shadow_map, 0);

    for (int x = -2; x <= 2; x++) {
        for (int y = -2; y <=2; y++) {
            float pcf_depth = texture(shadow_map, coords.xy + vec2(x, y) * texel_size).r;
            shadow += coords.z - bias > pcf_depth ? 1 : 0;
        }
    }

    return shadow / 25;
}

void main() {
    vec3 normal = normalize(fs_in.normal);
    vec3 eye_direction = normalize(view_position - fs_in.position);

    vec3 diffuse_texture = texture(texture_diffuse1, fs_in.texture_coords).rgb;
    vec3 specular_texture = texture(texture_specular1, fs_in.texture_coords).rgb;
    float shadow = calc_shadow(dir_light, normal, fs_in.position_light_space);

    vec3 color = calc_dir_light(dir_light, normal, eye_direction,
                                diffuse_texture, specular_texture, material_shininess, shadow);

//    for (int i = 0; i < NUM_POINT_LIGHTS; i++) {
//        color += calc_point_light(point_light[i], normal, fs_in.position, eye_direction,
//                                  diffuse_texture, specular_texture, material_shininess, shadow);
//    }

    frag_color = vec4(color, 1.0);
}
