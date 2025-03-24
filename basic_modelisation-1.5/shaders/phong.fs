#version 330
out vec4 fragColor;

in vec4 modPos;
in vec3 modNormal;

uniform vec4 lumpos;
uniform vec4 couleur;

void main() {
     vec3 Ld = normalize(modPos.xyz - lumpos.xyz);
     
     float il = 2.0 * clamp(dot(modNormal, -Ld), 0.0, 1.0);

     // on reprend le vecteur en dur à partir ce ce qui a été défini dans le lookAt...
     vec3 viewVector = normalize(vec3(0.0, -2.0, -4.0));

     // vecteur reflet
     vec3 reflect = reflect(Ld, modNormal);
     
     // L'intensité de spéculaire est un produit scalaire entre le vecteur reflet et le vecteur vue
     // (on en met un des deux en négatif pour comparer leur colinéarité dans la même direction)
     float specularIntensity = clamp(dot(-reflect, viewVector), 0.0, 1.0);
     
     // On ajoute une exponentielle (à 10 par exemple) pour éviter d'avoir une valeur trop progressive entre 0 et 1.
     // On veut une valeur qui montre en flêche lorsqu'elle est proche de 1
     specularIntensity = pow(specularIntensity, 10.0);

     // La lumière spéculaire (blanche) vient s'ajouter (avec une intensité variable) à la lumière diffuse
     fragColor = specularIntensity * vec4(1.0) + il * couleur;
}