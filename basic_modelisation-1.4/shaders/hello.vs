#version 330

// Coordonnée 3D représantant une position spatiale
layout(location = 0) in vec3 pos;
// Coordonnée 3D représantant un vecteur normal
layout(location = 1) in vec3 normal;
// Coordonnée 2D dans une texture
layout(location = 2) in vec2 texCoord;

// Matrice 4D de translation reçue du CPU
uniform mat4 model, view, projection;

out float il;

void main() {
    // faire bouger les normales relativement au model dans l'espace
    vec3 n = normalize(transpose(inverse(view * model)) * vec4(normal, 0.0)).xyz;

    vec3 Ld = normalize(vec3(0.0, -0.2, -1.0));

    il = clamp(dot(n, -Ld), 0.0, 1.0);

    gl_Position = projection * view * model * vec4(pos, 1.0);
}