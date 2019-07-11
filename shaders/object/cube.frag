#version 450 core

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_height1;
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

in V_DATA {
    vec3 position;
    vec2 texture_coords;
    vec3 tangent_light_pos;
    vec3 tangent_view_pos;
    vec3 tangent_frag_pos;
} fs_in;

layout (location = 0) out vec4 frag_color;
layout (location = 1) out vec4 bright_color;

layout (std140, binding = 1) uniform Lights {
    vec3 view_position;
    DirLight dir_light[5];
    PointLight point_light[5];
};

layout (std140, binding = 9) uniform PointShadow {
    mat4 shadow_matrices[6];
    vec3 light_position;
    float far_plane;
};

vec3 calc_point_light(PointLight light, vec3 normal, vec3 light_pos, vec3 frag_position, vec3 eye_direction,
                      vec3 diffuse_texture, vec3 specular_texture, float shadow) {
    float light_distance = distance(light_pos, frag_position);
    float attenuation = dot(light.attenuation,
                            vec3(1.0, light_distance, light_distance * light_distance));

    vec3 light_direction = normalize(light_pos - frag_position);
    vec3 half_vec = normalize(eye_direction + light_direction);

    vec3 ambient = light.ambient * diffuse_texture;
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

vec2 parallax_mapping(vec2 texture_coords, vec3 eye_direction) {
    const float height_scale = 0.1;
    const float min_layers = 8;
    const float max_layers = 32;
    const float num_layers = mix(max_layers, min_layers, abs(eye_direction.z));
    const float layer_depth = 1.0 / num_layers;
    const vec2 p = eye_direction.xy * height_scale;
    const vec2 delta_texture_coords = p / num_layers;

    float current_layer_depth = 0.0;
    vec2 current_texture_coords = texture_coords;
    float current_depth_map_value = texture(texture_height1, current_texture_coords).r;

    while (current_layer_depth < current_depth_map_value) {
        current_texture_coords -= delta_texture_coords;
        current_depth_map_value = texture(texture_height1, current_texture_coords).r;
        current_layer_depth += layer_depth;
    }

    vec2 prev_texture_coords = current_texture_coords + delta_texture_coords;
    float prev_layer_depth = current_layer_depth - layer_depth;
    float prev_depth_map_value = texture(texture_height1, prev_texture_coords).r;

    float after_depth_offset = -(current_depth_map_value - current_layer_depth);
    float prev_depth_offset = prev_depth_map_value - prev_layer_depth;
    float weight = after_depth_offset / (after_depth_offset + prev_depth_offset);

    return mix(current_texture_coords, prev_texture_coords, weight);
}

vec3 filter_bright_colors(vec3 color) {
    if (dot(color, vec3(0.2126, 0.7152, 0.0722)) > 1.0) {
        return color;
    } else {
        return vec3(0.0);
    }
}

void main() {
    vec3 eye_direction = normalize(fs_in.tangent_view_pos - fs_in.tangent_frag_pos);
    vec2 texture_coords = parallax_mapping(fs_in.texture_coords, eye_direction);

    vec3 normal = texture(texture_normal1, texture_coords).rgb;
    normal = normalize(normal * 2.0 - 1.0);

    vec3 diffuse_texture = texture(texture_diffuse1, texture_coords).rgb;
    float shadow = calc_shadow(point_light[0], fs_in.position, normal);

    vec3 color = calc_point_light(point_light[0], normal,
                                  fs_in.tangent_light_pos, fs_in.tangent_frag_pos, eye_direction,
                                  diffuse_texture, vec3(0.2), shadow);

    frag_color = vec4(color, 1.0);
    bright_color = vec4(filter_bright_colors(color), 1.0);
}
