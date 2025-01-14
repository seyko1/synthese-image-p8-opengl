#version 330

// Coordonnée 3D représantant une position spatiale
layout(location = 0) in vec3 pos;
// Coordonnée 3D représantant un vecteur normal
layout(location = 1) in vec3 normal;
// Coordonnée 2D dans une texture
layout(location = 2) in vec2 texCoord;

// Variable globale reçu du programme C côté CPU, en lecture seule, accessible par tout les processus côté GPU
uniform float weight;

void main() {
     // Variable attendue par openGL
     gl_Position = vec4(pos, weight);
}