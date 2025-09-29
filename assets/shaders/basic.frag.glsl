#version 430 core

out vec4 FragColor;
in vec2 vTexCoord;

uniform sampler2D screenTex;

void main() {

    FragColor = texture(screenTex, vTexCoord);
}

