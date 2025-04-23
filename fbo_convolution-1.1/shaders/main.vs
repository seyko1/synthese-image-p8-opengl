#version 330

// Coordonnée 3D représantant une position spatiale
layout(location = 0) in vec3 pos;
// Coordonnée 3D représantant un vecteur normal
layout(location = 1) in vec3 normal;
// Coordonnée 2D dans une texture
layout(location = 2) in vec2 texCoord;


void main() {
    gl_Position = vec4(pos, weight);
}