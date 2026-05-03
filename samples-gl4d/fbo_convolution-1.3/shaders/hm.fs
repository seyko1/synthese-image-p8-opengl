#version 330

out vec4 fragColor;

in vec3 vsoNormal;
in float lightIntensity;

void main() {
     fragColor = vec4(lightIntensity);
}  