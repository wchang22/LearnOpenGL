# version 450 core

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

in vec2 texture_coords;

uniform sampler2D texture_screen1;
uniform sampler2D texture_screen2;
uniform sampler2D texture_screen3;
uniform sampler2D texture_screen4;
uniform sampler2D texture_screen5;
uniform sampler2D texture_screen6;
uniform sampler2D texture_screen7;
uniform sampler2D texture_screen8;
uniform sampler2D ssao;
uniform samplerCube shadow_map;

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
    int num_dir_lights;
    int num_point_lights;
};

layout (std140, binding = 3) buffer DirLights {
    DirLight dir_light[];
};

layout (std140, binding = 4) buffer PointLights {
    PointLight point_light[];
};

layout (std140, binding = 9) uniform PointShadow {
    mat4 shadow_matrices[6];
    vec3 light_position;
    float far_plane;
};

vec3 calc_point_light(PointLight light, vec3 normal, vec3 light_pos, vec3 frag_position, vec3 eye_direction,
                      vec3 diffuse_texture, vec3 specular_texture, float shadow, float ambient_occlusion) {
    float light_distance = distance(light_pos, frag_position);
    float attenuation = dot(light.attenuation,
                            vec3(1.0, light_distance, light_distance * light_distance));

    vec3 light_direction = normalize(light_pos - frag_position);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture * ambient_occlusion;
    vec3 diffuse = light.diffuse * diffuse_texture * max(dot(normal, light_direction), 0.0);
    vec3 specular = light.specular * specular_texture *
                    pow(max(dot(normal, half_vec), 0.0), 32);

    return (ambient + (1 - shadow) * (diffuse + specular)) / attenuation;
}

float calc_shadow(PointLight light, vec3 position, vec3 normal) {
    vec3 light_ray = position - light.position;
    float current_depth = length(light_ray);

    float bias = 0.1;
    const int samples = 20;
    float disk_radius = (1.0 + distance(view_position, position) / far_plane) / 25.0;
    float shadow = 0;

    const vec3 sample_offset_dirs[samples] = vec3[]
    (
       vec3( 1,  1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1,  1,  1),
       vec3( 1,  1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1,  1, -1),
       vec3( 1,  1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1,  1,  0),
       vec3( 1,  0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1,  0, -1),
       vec3( 0,  1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0,  1, -1)
    );

    for (int i = 0; i < samples; i++) {
        float closest_depth =
                texture(shadow_map, light_ray + sample_offset_dirs[i] * disk_radius).r * far_plane;
        if (current_depth - bias > closest_depth) {
            shadow += 1;
        }
    }

    return shadow / float(samples);
}

vec3 filter_bright_colors(vec3 color) {
    if (dot(color, vec3(0.2126, 0.7152, 0.0722)) > 1.0) {
        return color;
    } else {
        return vec3(0.0);
    }
}

void main()
{
    vec3 position = texture(texture_screen1, texture_coords).rgb;
    vec3 position_view = texture(texture_screen2, texture_coords).rgb;
    vec3 normal = texture(texture_screen3, texture_coords).rgb;
    vec3 normal_view = texture(texture_screen4, texture_coords).rgb;
    vec4 diffuse_specular = texture(texture_screen5, texture_coords);
    vec3 diffuse = diffuse_specular.rgb;
    vec3 specular = diffuse_specular.aaa;
    vec3 t = texture(texture_screen6, texture_coords).rgb;
    vec3 b = texture(texture_screen7, texture_coords).rgb;
    vec3 n = texture(texture_screen8, texture_coords).rgb;
    mat3 tbn = transpose(mat3(t, b, n));
    vec3 tangent_view_pos = tbn * view_position;
    vec3 tangent_frag_pos = tbn * position;
    vec3 eye_direction = normalize(tangent_view_pos - tangent_frag_pos);
    float ambient_occlusion = texture(ssao, texture_coords).r;

    float shadow = 0.0;

//    for (int i = 0; i < num_point_lights; i++) {
//        shadow += calc_shadow(point_light[i], position, normal);
//    }
    shadow = clamp(shadow, 0.0, 1.0);

    vec3 color = vec3(0.0);

    for (int i = 0; i < num_point_lights; i++) {
        vec3 tangent_light_pos = tbn * point_light[i].position;
        color += calc_point_light(point_light[i], normal, tangent_light_pos, tangent_frag_pos,
                                  eye_direction, diffuse, specular, shadow, ambient_occlusion);
    }

    frag_color = vec4(vec3(ambient_occlusion), 1.0);
    bright_color = vec4(filter_bright_colors(color), 1.0);
}
