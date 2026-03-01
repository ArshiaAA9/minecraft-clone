#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;
layout(location = 2) in mat4 aInstanceModel;

out vec3 fragColor;

uniform mat4 view;
uniform mat4 projection;

void main() {
        // note that we read the multiplication from right to left
        gl_Position = projection * view * aInstanceModel * vec4(aPos, 1.0);
        fragColor = aColor;
}
