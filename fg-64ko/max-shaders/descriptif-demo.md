# Descriptif des animations de la démo 64ko

Résumé :

- Terrain en mouvement,
- Nuage de points 3D en motif fractal
- Simulation du jeu de la vie 3D qui réagit au son
- Un effet de distorsion de pixels
- Des ondulations animées qui réagissent au son

## Élévation de terrain procédurale avec bruit de perlin 2D

- Effet de mouvement en soustrayant le temps écoulé.  
- La teinte de couleur change selon les sommets (réduction de la variation et ajout d'une teinte bleue pour le ciel).

## Nuage de points 3D en motif fractal

- Calcul de la position des points à dessiner avec la formule Mandelbulb. ([lien vers skytopia.com](https://www.skytopia.com/project/fractal/2mandelbulb.html))  
- On dessine uniquement les points à la surface (profondeur d'itération `MAX_ITER`).  
- Utilisation d'un **Vertex Array Object (VAO)** et **Vertex Buffer Object (VBO)**.

## Jeu de la vie 3D (grille de cubes) dans un ciel étoilé (bruit de perlin)

- Première génération avec noyau central de cube en vie.  
- Détermination des nouvelles générations selon un **seuil appliqué à la moyenne d’échantillonnage du son**.  
- Ciel étoilé avec un bruit de Perlin 3D **très amplifié**.

## Inversion sphérique en damier (2D)

- Oscillation du centre.  
- Inversion de la distance du fragment au centre : plus elle est petite, plus l’inversion est grande.  
- Effet de **dilatation des pixels** et **oscillation animée** (distorsion + mouvement fluide).

## Waves pattern (calcul de hauteurs d'une grille), effet skyfall en bruit de perlin 3D

- Récupération des **moyennes d’échantillonnage du son** pour en faire des hauteurs.  
- Dessin d’une grille et envoi des hauteurs dans une **texture pour le shader**.  
- Calcul de la **distance au centre** pour déterminer la bonne hauteur à utiliser (coordonnée de texture).  
- Oscillation sinusoïdale pour donner un effet de **vagues dynamiques**.  
- Bruit de Perlin 3D **dans un cylindre** pour simuler l’effet de chute dans l’atmosphère.

## Transition de textes avec bruit de perlin 2D

- Effet de **masquage dynamique** du texte avec un bruit de Perlin 2D.  
- Déplacement d’une valeur de seuil dans le temps : si la valeur du bruit dépasse ce seuil → transition du pixel.
