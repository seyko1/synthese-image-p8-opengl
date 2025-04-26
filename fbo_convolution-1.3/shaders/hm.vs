#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 projection, modelView;

uniform sampler2D tex;

out vec2 tCoord;

void main() {
    /* On fait en sorte de changer la position du vertex en y
     * selon le texel correspondant.
     */
    vec3 p = pos + vec3(0.0, texture(tex, texCoord).r / 4.0, 0.0);
    gl_Position = projection * modelView * vec4(p, 1.0);
    tCoord = texCoord;
}