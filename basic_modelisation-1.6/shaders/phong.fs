#version 330
out vec4 fragColor;

in vec4 modPos;
in vec3 modNormal;
in vec2 tCoord;

uniform vec4 lumpos;
uniform vec4 couleur;
// on aura besoin de la matrice view pour modifier le vecteur reflet
uniform mat4 view;
uniform sampler2D tex;
uniform int useTex;

void main() {
     if (useTex == 1) {
        fragColor = texture(tex, tCoord);
        return;
     }

     /* Etape 1 - Calcul et application de l'intensité de la lumière (lumière diffuse) à 85% */

     vec3 directionalLightVector = normalize(modPos.xyz - lumpos.xyz);
     
     float lightIntensity = 2.0 * clamp(dot(modNormal, -directionalLightVector), 0.0, 1.0);

     fragColor = 0.85 * lightIntensity * couleur;

     /* Etape 2 - Calcul et application du reflet de la lumière (lumière spéculaire) */

     // Déduire le vecteur reflet
     vec3 reflectVector = reflect(directionalLightVector, modNormal);

     // formule magique pour extraire et appliquer les rotations stockées dans view au vecteur de reflet
     reflectVector = normalize((transpose(inverse(view)) * vec4(reflectVector, 0.0)).xyz);

     // L'oeil est toujours orienté dans cette direction en phase finale (view model) ...
     const vec3 viewVector = vec3(0.0, 0.0, -1.0);

     // L'intensité de spéculaire est un produit scalaire entre le vecteur reflet et le vecteur vue
     // (on en met un des deux en négatif pour comparer leur colinéarité dans la même direction)
     float specularIntensity = clamp(dot(-reflectVector, viewVector), 0.0, 1.0);
     
     // On ajoute une exponentielle élevée (à 10 par exemple) pour éviter d'avoir une valeur trop progressive entre 0 et 1.
     // On veut une valeur qui montre en flêche lorsqu'elle est proche de 1.
     specularIntensity = pow(specularIntensity, 10.0);

     // La lumière spéculaire (blanche) vient s'ajouter (avec une intensité variable) à la lumière diffuse
     fragColor += specularIntensity * vec4(1.0);

     /* Etape 3 - Ajout d'une lumière ambiante à 15% */
     const vec4 ambiantLight = vec4(0.0, 1.0, 0.0, 1.0);

     fragColor += 0.15 * ambiantLight;
}