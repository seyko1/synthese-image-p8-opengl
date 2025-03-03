#version 330
out vec4 fragColor;

in float il;

uniform vec4 couleur;

void main() {
     fragColor = il * couleur;
}