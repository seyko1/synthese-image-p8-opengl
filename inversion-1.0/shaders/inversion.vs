#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 modelView, projection;
uniform vec2 resolution;

out vec2 fragCoord;

void main() {
    gl_Position = projection * modelView * vec4(pos, 1.0);
    fragCoord = texCoord * resolution;
}