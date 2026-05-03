#version 330

out vec4 fragColor;
in vec2 tCoord;

uniform sampler2D tex;
uniform vec2 pas;

// Représente les deux matrices de convolution hozizontale Gx et verticale Gy
// https://fr.wikipedia.org/wiki/Filtre_de_Sobel
vec2 G[9] = vec2[9](vec2(-1, -1), vec2(0, -2), vec2(1, -1),
                    vec2(-2,  0), vec2(0,  0), vec2(2, 0),
                    vec2(-1,  1), vec2(0,  2), vec2(1,  1));

// Représente les 9 voisins d'un texel pour une convolution 3x3
vec2 offset[9] = vec2[9](vec2(-pas.x,  pas.y), vec2(0,  pas.y), vec2(pas.x,  pas.y),
                         vec2(-pas.x,  0),     vec2(0,  0),     - vec2(pas.x, 0),
                         vec2(-pas.x, -pas.y), vec2(0, -pas.y), vec2(pas.x, -pas.y));

// Calculer deux résultats de gradient, un x et un y sur deux vecteurs 3D de couleur 
vec3 sobel(sampler2D texel) {
     vec3 sobel[2] = vec3[2](vec3(0), vec3(0));

     for (int i = 0; i < 9; i++) {
          vec3 couleur_voisin = texture(texel, tCoord + offset[i]).rgb;
          
          sobel[0] += G[i].x * couleur_voisin;
          sobel[1] += G[i].y * couleur_voisin;
     }
     // triple sobel pour chaque composante
     return vec3(
          abs(sobel[0].r + abs(sobel[1].r)),
          abs(sobel[0].g + abs(sobel[1].g)),
          abs(sobel[0].b + abs(sobel[1].b))
     );
}

void main() {
     fragColor = vec4(sobel(tex), 1.0);
}