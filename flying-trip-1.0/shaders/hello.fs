#version 330
out vec4 fragColor;

in float vPosHeight;

void main() {
     float minGreyValue = 0.2f;
     float amplitude = 0.5f;
     fragColor = vec4(vec3(minGreyValue + vPosHeight * amplitude), 1.0);
}