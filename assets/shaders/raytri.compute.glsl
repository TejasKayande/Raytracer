#version 430

layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) writeonly uniform image2D outImage;

uniform ivec2 resolution;
uniform vec3  camPos;
uniform vec3  camFront;
uniform vec3  camUp;

#define NUM_SPHERES 3
#define NUM_CUBES   2

struct Sphere {
    vec3  center;
    float radius;
    vec3  color;
};

struct Cube {
    vec3 center;
    vec3 size;
    vec3 color;
};

Sphere G_spheres[NUM_SPHERES];
Cube   G_cubes[NUM_CUBES];

bool intersectSphere(vec3 ro, vec3 rd, Sphere sph, out float t, out vec3 normal) {

    vec3 oc = ro - sph.center;
    float a = dot(rd, rd);
    float b = 2.0 * dot(oc, rd);
    float c = dot(oc, oc) - sph.radius * sph.radius;

    float discriminant = b*b - 4.0*a*c;
    if(discriminant < 0.0) {
        t = -1.0;
        normal = vec3(0.0);
        return false;
    }

    float sqrtDisc = sqrt(discriminant);
    float t0 = (-b - sqrtDisc) / (2.0 * a);
    float t1 = (-b + sqrtDisc) / (2.0 * a);

    float chosen = (t0 > 0.0) ? t0 : t1;
    if(chosen < 0.0) {
        t = -1.0;
        normal = vec3(0.0);
        return false;
    }

    t = chosen;
    vec3 hitPos = ro + rd * t;
    normal = normalize(hitPos - sph.center);
    return true;
}

// NOTE(Tejas): source(https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm)
bool intersectTriangle(vec3 ro, vec3 rd, vec3 v0, vec3 v1, vec3 v2, out float t, out vec3 outNormal) {
    const float EPS = 1e-6;

    vec3 edge1 = v1 - v0;
    vec3 edge2 = v2 - v0;

    vec3 ray_cross_e2 = cross(rd, edge2);
    float det = dot(edge1, ray_cross_e2);
    if (det > -(EPS) && det < EPS) {
        t = -1.0; outNormal = vec3(0.0);
        return false;
    }

    float invDet = 1.0 / det;

    vec3  s = ro - v0;
    float u = invDet * dot(s, ray_cross_e2);
    if (u < 0.0 || u > 1.0) {
        t = -1.0; outNormal = vec3(0.0);
        return false;
    }

    vec3 s_cross_e1 = cross(s, edge1);
    float v = invDet * dot(rd, s_cross_e1);
    if (v < 0.0 || u + v > 1.0) {
        t = -1.0; outNormal = vec3(0.0);
        return false;
    }

    float tt = invDet * dot(edge2, s_cross_e1);
    if (tt <= EPS) { t = -1.0; outNormal = vec3(0.0); return false; }

    t = tt;
    outNormal = normalize(cross(edge1, edge2));

    if (dot(outNormal, rd) > 0.0) outNormal = -outNormal;
    return true;
}

bool intersectCube(vec3 ro, vec3 rd, Cube cube, out float bestT, out vec3 bestNormal) {

    vec3 h = cube.size * 0.5; // half extents
    vec3 c = cube.center;
    vec3 v[8];
    v[0] = c + vec3(-h.x, -h.y, -h.z);
    v[1] = c + vec3( h.x, -h.y, -h.z);
    v[2] = c + vec3( h.x,  h.y, -h.z);
    v[3] = c + vec3(-h.x,  h.y, -h.z);
    v[4] = c + vec3(-h.x, -h.y,  h.z);
    v[5] = c + vec3( h.x, -h.y,  h.z);
    v[6] = c + vec3( h.x,  h.y,  h.z);
    v[7] = c + vec3(-h.x,  h.y,  h.z);

    const ivec3 tris[12] = ivec3[12](
        ivec3(0,1,2), ivec3(0,2,3), // back face  (-z)
        ivec3(4,6,5), ivec3(4,7,6), // front face (+z)
        ivec3(0,4,5), ivec3(0,5,1), // bottom     (-y)
        ivec3(3,2,6), ivec3(3,6,7), // top        (+y)
        ivec3(1,5,6), ivec3(1,6,2), // right      (+x)
        ivec3(0,3,7), ivec3(0,7,4)  // left       (-x)
    );

    bestT = 1e20;
    bestNormal = vec3(0.0);

    bool hitAny = false;

    for (int i = 0; i < 12; ++i) {

        float ttri;
        vec3 ntri;

        if (intersectTriangle(ro, rd, v[tris[i].x], v[tris[i].y], v[tris[i].z], ttri, ntri)) {

            if (ttri < bestT && ttri > 0.0) {
                bestT      = ttri;
                bestNormal = ntri;
                hitAny     = true;
            }
        }
    }

    if (!hitAny) {
        bestT = -1.0;
        return false;
    }

    return true;
}

void main() {

    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    if (pixel.x >= resolution.x || pixel.y >= resolution.y) return;

    vec2 uv = (vec2(pixel) + 0.5) / vec2(resolution);
    uv = uv * 2.0 - 1.0;
    uv.x *= float(resolution.x) / float(resolution.y);

    {
        G_spheres[0] = Sphere(vec3(0.0, 0.0, -3.0), 1.0, vec3(1.0, 0.0, 0.0));
        G_spheres[1] = Sphere(vec3(2.0, 0.0, -4.0), 1.0, vec3(0.0, 1.0, 0.0));
        G_spheres[2] = Sphere(vec3(-2.0, 0.0, -4.0), 1.0, vec3(0.0, 0.0, 1.0));

        G_cubes[0] = Cube(vec3(0.0, -1.5, -2.5), vec3(1.2, 1.2, 1.2), vec3(1.0, 1.0, 1.0));
        G_cubes[1] = Cube(vec3(-3.0, 1.5, -5.0), vec3(1.0, 1.0, 1.0), vec3(1.0, 0.0, 1.0));
    }

    vec3 forward = normalize(camFront);
    vec3 right   = normalize(cross(forward, camUp));
    vec3 up      = cross(right, forward);

    float scale     = 1.0;
    vec3  rayDir    = normalize(forward + uv.x * scale * right + uv.y * scale * up);
    vec3  rayOrigin = camPos;

    vec3  color    = vec3(0.0);
    float closestT = 1e20;
    vec3  hitNormal = vec3(0.0);
    vec3  hitColor  = vec3(0.0);

    for (int i = 0; i < NUM_SPHERES; i++) {

        float t;
        vec3 normal;

        if (intersectSphere(rayOrigin, rayDir, G_spheres[i], t, normal)) {

            if (t < closestT) {
                closestT = t;
                hitNormal = normal;
                hitColor = G_spheres[i].color;
            }
        }
    }

    for (int i = 0; i < NUM_CUBES; ++i) {

        float t;
        vec3 normal;

        if (intersectCube(rayOrigin, rayDir, G_cubes[i], t, normal)) {

            if (t < closestT) {

                closestT = t;
                hitNormal = normal;
                hitColor = G_cubes[i].color;
            }
        }
    }

    if (closestT < 1e20) {

        vec3 hitPos = rayOrigin + rayDir * closestT;
        vec3 lightDir = normalize(vec3(1.0, 1.0, 1.0));
        float diffuse = max(dot(hitNormal, lightDir), 0.0);

        vec3 ambient = 0.15 * hitColor;
        vec3 diff    = diffuse * hitColor;
        color = ambient + diff;

        float rim = pow(1.0 - max(0.0, dot(normalize(camPos - hitPos), hitNormal)), 2.0);
        color += 0.05 * rim;

    } else {

        // for blue gradient
        float y = float(pixel.y) / float(resolution.y);
        color = vec3(y * 0.2, y * 0.9, y);
    }

    imageStore(outImage, pixel, vec4(color, 1.0));
}
