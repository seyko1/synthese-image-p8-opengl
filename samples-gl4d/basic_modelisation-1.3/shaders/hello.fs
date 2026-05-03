#version 330
out vec4 fragColor;

in float il;

void main() {
     fragColor = il * vec4(0.0, 0.0, 1.0, 1.0);
}