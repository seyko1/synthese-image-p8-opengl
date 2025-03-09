#version 330

// Coordonnée 3D représantant une position spatiale
layout(location = 0) in vec3 pos;
// Coordonnée 3D représantant un vecteur normal
layout(location = 1) in vec3 normal;
// Coordonnée 2D dans une texture
layout(location = 2) in vec2 texCoord;

// Matrice 4D de translation reçue du CPU
uniform mat4 model, view, projection;

uniform vec4 lumpos;

out float il;

void main() {
    // modéliser le sommet dans la scène
    vec4 modPos = model * vec4(pos, 1.0);

    vec3 Ld = normalize(modPos.xyz - lumpos.xyz);
    // faire bouger les normales relativement au model dans l'espace
    vec3 n = normalize(transpose(inverse(model)) * vec4(normal, 0.0)).xyz;

    il = 2.0 * clamp(dot(n, -Ld), 0.0, 1.0);

    gl_Position = projection * view * modPos;
}