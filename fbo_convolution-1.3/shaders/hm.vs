#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 projection, modelView;

uniform sampler2D tex;
uniform vec2 pas;

out vec2 tCoord;
out vec3 vsoNormal;
out float lightIntensity;

/* tableau de décalage pour parcourir les 6 points voisins d'un sommet */
/* voir schéma Farès vidéséance 16 */
vec2 offset[7] = vec2[7](
    vec2(pas.x, 0),
    vec2(pas.x,  pas.y),
    vec2(0,  pas.y),
    vec2(-pas.x,  0),
    vec2(-pas.x, -pas.y),
    vec2(0, -pas.y),
    vec2(0,  0)
);

/* Donner la hauteur d'un sommet selon l'intensité de rouge du texel correspondant */
float hauteur(vec2 tc) {
    return texture(tex, tc).r / 4.0;
}

/* Calculer la normale d'un des triangles reliés au sommet.
 * On a besoin de deux de ses côtés en entrée pour calculer sa normale.
 */ 
vec3 tnormale(vec3 a, vec3 b) {
    return normalize(cross(a, b));
}

/* Position d'un sommet voisin = position initiale + décalage, et modification de sa hauteur selon son intensité de rouge */
vec3 position(vec3 pi, vec2 tc, vec2 offset) {
    return pi + vec3(
        offset.x * 2.0,
        hauteur(tc),
        -(offset.y * 2.0)
    );
}

/* Calculer la normale d'un sommet, en moyennant les normales des triangles qui sont autour. */
vec3 normale(vec3 pi, vec2 tc) {
    // normale du sommet
    vec3 n = vec3(0);

    // On a 7 points constituant les 6 triangles autour du sommet.
    vec3 points[7];

    /* On parcourt nos 7 coordonées spatiales  */
    for (int i = 0; i < 7; ++i) {
        points[i] = position(pi, tc + offset[i], offset[i]);
    }

    /* On parcourt chaque points pour trouver les deux côtés de chaque triangles */
    for (int i = 0; i < 6; ++i) {
        /* Calculer 2 côtés du triangle lié au point i 
         * Le point i est un voisin dans la texture
         */
        vec3 a = points[6];
        vec3 b = points[i];
        vec3 c = points[(i + 1) % 6];

        n += tnormale(b - a, c - b);
    }
    return n /= 6.0;
}                                                                                                                                                                        

void main() {
    vec3 Ld = vec3(0, -1, -1);

    /* On fait en sorte de changer la position du vertex en y
     * selon l'intensité de rouge du texel à cette position.
     */
    vec3 p = position(pos, texCoord, offset[6]);

    vsoNormal = normalize((transpose(inverse(modelView)) * vec4(normale(pos, texCoord), 1.0)).xyz);

    lightIntensity = dot(vsoNormal, -Ld);

    gl_Position = projection * modelView * vec4(p, 1.0);
    tCoord = texCoord;
}