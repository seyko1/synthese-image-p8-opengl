#version 330

out vec4 fragColor;
in vec2 tCoord;
uniform sampler2D tex;

void main() {
     fragColor = texture(tex, tCoord);
}