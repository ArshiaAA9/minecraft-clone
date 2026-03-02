#version 430 core
out vec4 FragColor;
in vec2 TexCoord;

uniform vec2 iResolution;

uniform sampler2D textureAtlas;

void main() {
        FragColor = texture(textureAtlas, TexCoord);
}
