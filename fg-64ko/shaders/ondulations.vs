#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 projection, modelView;
uniform sampler2D tex;
uniform vec2 pas;
uniform float elapsed;

out float lightIntensity;
out float dc;

/* Tableau de décalage pour parcourir les 6 points voisins du vertex */
vec2 offset[7] = vec2[7](
    vec2(pas.x, 0.0),
    vec2(pas.x,  pas.y),
    vec2(0.0,  pas.y),
    vec2(-pas.x,  0.0),
    vec2(-pas.x, -pas.y),
    vec2(0.0, -pas.y),
    vec2(0.0,  0.0)
);

/* Donne la hauteur d'une coordonnée selon l'intensité de rouge du texel correspondant */
float hauteur(vec2 tc) {
    /* distance de la coordonnée du centre de la grille */
    float d = distance(vec2(0.5), tc);

    /* Normaliser y à l'echelle [-0.5; 0.5] de la grille ? */
    float y = d / (1.42 / 2.0);
    return texture(tex, vec2(1.0, y)).r / 4.0;
}

/* Calculer la normale d'un des triangles reliés au vertex.
 * On a besoin de deux de ses côtés en entrée pour calculer sa normale.
 */ 
vec3 tnormale(vec3 a, vec3 b) {
    return normalize(cross(a, b));
}

/* Position d'un sommet voisin = position initiale + offset, et modification de sa hauteur selon son intensité de rouge */
vec3 position(vec3 pi, vec2 tc, vec2 offset) {
    return pi + vec3(
        offset.x * 2.0,
        hauteur(tc),
        -(offset.y * 2.0)
    );
}

/* Calculer la normale du vertex, en moyennant les normales des triangles qui sont autour. */
vec3 normale(vec3 pi, vec2 tc) {
    // normale du sommet
    vec3 n = vec3(0);

    // On a 7 points constituant les 6 triangles autour du sommet.
    vec3 points[7];

    /* On parcourt nos 7 coordonnées spatiales  */
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
    vec3 Ld = normalize(vec3(0, -1, -1));

    dc = distance(vec2(0.5), texCoord);

    // modifier la hauteur du vertex en piochant une hauteur stockée dans tex.
    vec3 p = pos + vec3(0.0, hauteur(texCoord) * 2. + dc * sin(elapsed), 0.0);

    vec3 vsoNormal = normalize((transpose(inverse(modelView)) * vec4(normale(pos, texCoord), 1.0)).xyz);
    lightIntensity = dot(vsoNormal, -Ld);

    gl_Position = projection * modelView * vec4(p, 1.0);
}