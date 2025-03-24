#version 330
out vec4 fragColor;

in vec4 modPos;
in vec3 modNormal;

uniform vec4 lumpos;
uniform vec4 couleur;
// on aura besoin de la matrice view pour modifier le vecteur reflet
uniform mat4 view;

void main() {
     vec3 Ld = normalize(modPos.xyz - lumpos.xyz);
     
     float il = 2.0 * clamp(dot(modNormal, -Ld), 0.0, 1.0);

     // L'oeil est toujours orienté dans cette direction en phase final (view model) ...
     const vec3 viewVector = vec3(0.0, 0.0, -1.0);

     // déduire le vecteur reflet
     vec3 reflect = reflect(Ld, modNormal);
     // formule magique pour extraire et appliquer les rotations stockées dans view aux vecteurs de reflet
     reflect = normalize((transpose(inverse(view)) * vec4(reflect, 0.0)).xyz);
     
     // L'intensité de spéculaire est un produit scalaire entre le vecteur reflet et le vecteur vue
     // (on en met un des deux en négatif pour comparer leur colinéarité dans la même direction)
     float specularIntensity = clamp(dot(-reflect, viewVector), 0.0, 1.0);
     
     // On ajoute une exponentielle (à 10 par exemple) pour éviter d'avoir une valeur trop progressive entre 0 et 1.
     // On veut une valeur qui montre en flêche lorsqu'elle est proche de 1
     specularIntensity = pow(specularIntensity, 10.0);

     // La lumière spéculaire (blanche) vient s'ajouter (avec une intensité variable) à la lumière diffuse
     fragColor = specularIntensity * vec4(1.0) + il * couleur;
}