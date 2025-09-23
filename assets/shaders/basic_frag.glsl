#version 430 core

out vec4 FragColor;
in vec2 vTexCoord;

uniform vec2 uResolution;
uniform vec2 uMouse;

void main() {

    vec2 uv = (vTexCoord * uResolution - 0.5 * uResolution) / uResolution.y;

    vec3 col = vec3(0.0);
    
    vec2 mouseNormal = (uMouse - 0.5 * uResolution) / uResolution.y;
    float d = length(uv - mouseNormal);
    
    col = vec3(1.0 - d);

    FragColor = vec4(col, 1.0);
}
