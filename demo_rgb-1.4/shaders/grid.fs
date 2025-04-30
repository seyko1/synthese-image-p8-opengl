#version 330

in float lightIntensity;

out vec4 fragColor;

void main() {
     fragColor = vec4(lightIntensity);
}  