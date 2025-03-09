#version 330
out vec4 fragColor;

in vec4 modPos;
in vec3 modNormal;

uniform vec4 lumpos;
uniform vec4 couleur;

void main() {
     vec3 Ld = normalize(modPos.xyz - lumpos.xyz);
     
     float il = 2.0 * clamp(dot(modNormal, -Ld), 0.0, 1.0);

     fragColor = il * couleur;
}