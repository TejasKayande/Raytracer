#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) writeonly uniform image2D outImage;

uniform ivec2 resolution;

struct Sphere {
    vec3  center;
    float radius;
    vec3  color;
};

#define NUM_SPHERES 3

Sphere G_spheres[NUM_SPHERES];

/**********************************************************************************
 * NOTE(Tejas)
 * return the distance from the origin of the Ray to the point of contact.
 * if any contact, otherwise returns -1
 *
 * if the origin of the ray is O(x,y,z) and Direction is D(x,y,z) then the
 * quadratic equation showing the relation between the sphere and the ray is
 * give as follows:
 *
 * sphere at the origin (0, 0, 0)
 * (Dx^2 + Dy^2 + Dz^2) + 2(OxDx + OyDy + OzDz)t + Ox^2 + Oy^2 + Oz^2 - r^2 = 0
 *
 * where t is the distance from ray origin to hit point
 *
 * comparing the above quadratic equation with ax^2 + bx + c = 0
 * we have,
 * x = t
 * a = (Dx^2 + Dy^2 + Dz^2)
 * b = 2(OxDx + OyDy + OzDz)t
 * c = Ox^2 + Oy^2 + Oz^2 - r^2 = 0
 **********************************************************************************/

bool intersectSphere(vec3 ro, vec3 rd, Sphere sph, out float t) {

    vec3 oc = ro - sph.center;
    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc, oc) - sph.radius * sph.radius;

    float discriminant = b*b - 4.0*a*c;
    if(discriminant < 0.0) {
        t = -1.0;
        return false;
    }

    float sqrtDisc = sqrt(discriminant);
    float t0 = (-b - sqrtDisc) / (2.0 * a);
    float t1 = (-b + sqrtDisc) / (2.0 * a);

    t = (t0 > 0.0) ? t0 : t1;
    if(t < 0.0) return false;

    return true;
}

void main() {


    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);

    if (pixel.x >= resolution.x || pixel.y >= resolution.y) return;

    vec2 uv = (vec2(pixel) + 0.5) / vec2(resolution);
    uv = uv * 2.0 - 1.0;

    vec3 rayOrigin = vec3(0.0, 0.0, 0.0);
    vec3 rayDir    = normalize(vec3(uv.x, uv.y, -1.0));


    {
        G_spheres[0] = Sphere(vec3(0.0, 0.0, -3.0), 1.0, vec3(1.0, 0.0, 0.0));
        G_spheres[1] = Sphere(vec3(2.0, 0.0, -4.0), 1.0, vec3(0.0, 1.0, 0.0));
        G_spheres[2] = Sphere(vec3(-2.0, 0.0, -4.0), 1.0, vec3(0.0, 0.0, 1.0));
    }


    vec3  color    = vec3(0.0);
    float closestT = 1e20;

    for (int i = 0; i < NUM_SPHERES; i++) {

        float t;

        if (intersectSphere(rayOrigin, rayDir, G_spheres[i], t)) {

            if (t < closestT) {

                closestT = t;

                vec3 hitPos = rayOrigin + rayDir * t;
                vec3 normal = normalize(hitPos - G_spheres[i].center);

                vec3 lightDir = normalize(vec3(-1.0, 2.0, 1.0));
                float diffuse = max(dot(normal, lightDir), 0.0);

                color = G_spheres[i].color * diffuse;
            }
        }
    }

    imageStore(outImage, pixel, vec4(color, 1.0));
}
