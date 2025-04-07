#version 330

// Coordonnée 3D représantant une position spatiale
layout(location = 0) in vec3 pos;
// Coordonnée 3D représantant un vecteur normal
layout(location = 1) in vec3 normal;
// Coordonnée 2D dans une texture
layout(location = 2) in vec2 texCoord;

// Matrice 4D de translation reçue du CPU
uniform mat4 model, view, projection;

out vec4 modPos;
out vec3 modNormal;
out vec2 tCoord;

void main() {
    // modéliser le sommet dans la scène
    modPos = model * vec4(pos, 1.0);

    // faire bouger les normales relativement au model dans l'espace
    modNormal = normalize(transpose(inverse(model)) * vec4(normal, 0.0)).xyz;

    gl_Position = projection * view * modPos;

    tCoord = vec2(2.0 * texCoord.x, 2.0 * texCoord.y);
}