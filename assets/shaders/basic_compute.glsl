#version 430

layout (local_size_x = 16, local_size_y = 16) in;

layout (rgba32f, binding = 0) writeonly uniform image2D outImage;

uniform float time;
uniform ivec2 resolution;
uniform vec2 mouse;

void main() {
    ivec2 pixel = ivec2(gl_GlobalInvocationID.xy);
    
    if (pixel.x >= resolution.x || pixel.y >= resolution.y) return;
    
    // Normalize pixel coordinates
    vec2 uv = vec2(pixel) / vec2(resolution);
    
    // Compute distance from moving point (mouse)
    float d = distance(uv, mouse);
    
    // Animated color
    vec3 color = vec3(0.5 + 0.5 * sin(time + uv.x * 10.0),
                      0.5 + 0.5 * cos(time + uv.y * 10.0),
                      1.0 - smoothstep(0.0, 0.2, d));
    
    imageStore(outImage, pixel, vec4(color, 1.0));
}
