#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) writeonly uniform image2D out_image;
layout(binding = 2) uniform sampler2D skybox;

uniform ivec2 resolution;
uniform vec3  cam_pos;
uniform vec3  cam_front;
uniform vec3  cam_up;
uniform float cam_fov;

uniform vec3 light_dir = vec3(1.0, 1.0, 1.0);

#define MATH_PI 3.14159265

struct Sphere {
    vec3  center;
    float radius;
    vec3  color;
    float pad;
};
layout(std430, binding = 1) buffer Spheres {
    Sphere spheres[];
};

bool intersectSphere(vec3 ro, vec3 rd, Sphere sph, out float t, out vec3 normal) {

    vec3 oc = ro - sph.center;
    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc, oc) - sph.radius * sph.radius;

    float disc = b*b - 4.0*a*c;
    if (disc < 0.0) { t = -1.0; normal = vec3(0.0); return false; }

    float sqrtDisc = sqrt(disc);
    float t0 = (-b - sqrtDisc) / (2.0 * a);
    float t1 = (-b + sqrtDisc) / (2.0 * a);
    float chosen = (t0 > 0.0) ? t0 : t1;
    if (chosen < 0.0) { t = -1.0; normal = vec3(0.0); return false; }

    t = chosen;
    normal = normalize(ro + rd * t - sph.center);
    return true;
}

void main() {

    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    if (pixel.x >= resolution.x || pixel.y >= resolution.y) return;

    float fov_scale = tan(radians(cam_fov) * 0.5);

    vec2 uv = (vec2(pixel) + 0.5) / vec2(resolution);
    uv = uv * 2.0 - 1.0;
    uv.x *= float(resolution.x) / float(resolution.y);

    vec3 forward  = normalize(cam_front);
    vec3 right    = normalize(cross(forward, cam_up));
    vec3 up       = cross(right, forward);
    vec3 ray_dir = normalize(forward + uv.x * fov_scale * right + uv.y * fov_scale * up);
    // vec3 ray_dir  = normalize(forward + uv.x * right + uv.y * up);
    vec3 ray_ori  = cam_pos;

    vec3  color      = vec3(0.0);
    float closest_t  = 1e20;
    vec3  hit_normal = vec3(0.0);
    vec3  hit_color  = vec3(0.0);

    for (int i = 0; i < spheres.length(); ++i) {

        float t;
        vec3 normal;
        if (intersectSphere(ray_ori, ray_dir, spheres[i], t, normal)) {
            if (t < closest_t) { closest_t = t; hit_normal = normal; hit_color = spheres[i].color; }
        }
    }

    if (closest_t < 1e20) {

        vec3  hit_pos = ray_ori + ray_dir * closest_t;
        float diffuse = max(dot(hit_normal, normalize(light_dir)), 0.0);

        vec3  ambient = 0.15 * hit_color;
        vec3  diff    = diffuse * hit_color;
        float rim     = pow(1.0 - max(0.0, dot(normalize(cam_pos - hit_pos), hit_normal)), 2.0);

        color         = ambient + diff + 0.05 * rim;

    } else {
        vec2 sky_uv = vec2(atan(ray_dir.z, ray_dir.x) / (2.0 * MATH_PI) + 0.5, asin(ray_dir.y) / MATH_PI + 0.5);
        color       = texture(skybox, sky_uv).rgb;
    }

    imageStore(out_image, pixel, vec4(color, 1.0));
}
