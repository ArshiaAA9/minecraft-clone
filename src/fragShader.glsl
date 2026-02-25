#version 430 core
out vec4 FragColor;
in vec3 fragColor;

uniform vec2 iResolution;

void main() {
        FragColor = vec4(fragColor, 1.0);
}
