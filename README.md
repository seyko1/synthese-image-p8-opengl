# Synthèse d'Image (OpenGL)

Ce repo rassemble quelques TPs réalisés dans le cadre du cours de programmation de carte graphique à l'Université Paris 8. Il contient également un projet d'animations réalisé pour la track 64ko du concours [API8](https://www.api8.fr/).

Chaque **squelette** de projet dans ce repository provient directement de la librairie [GL4Dummies](https://github.com/noalien/GL4Dummies) mise à disposition dans un cadre académique pour rendre la programmation avec OpenGL plus simple à appréhender (Merci Farès Belhadj 🙏)

## Animations Projet API8 (Demoscene 64ko)

### Élévation de terrain procédurale avec bruit de perlin 2D (`flyingTrip.c`)

- Effet de mouvement en soustrayant le temps écoulé.  
- La teinte de couleur change selon les sommets (réduction de la variation et ajout d'une teinte bleue pour le ciel).

### Nuage de points 3D en motif fractal (`mandelbulbFractal.c`)

- Calcul de la position des points à dessiner avec la formule Mandelbulb. ([lien vers skytopia.com](https://www.skytopia.com/project/fractal/2mandelbulb.html))  
- On dessine uniquement les points à la surface (profondeur d'itération `MAX_ITER`).  
- Utilisation d'un **Vertex Array Object (VAO)** et **Vertex Buffer Object (VBO)**.

### Jeu de la vie 3D (grille de cubes) dans un ciel étoilé (`gameoflife3D.c`)

- Première génération avec noyau central de cubes en vie.  
- Détermination des nouvelles générations selon un **seuil appliqué à la moyenne d’échantillonnage du son**.  
- Ciel étoilé avec un bruit de Perlin 3D **très amplifié**.

### Inversion sphérique 2D en damier (`sphericalInversion.c`) 

- Oscillation du centre.  
- Inversion de la distance du fragment au centre : plus elle est petite, plus l’inversion est grande.  
- Effet de **dilatation des pixels** et **oscillation animée** (distorsion + mouvement fluide).

### Waves pattern (calcul de hauteurs d'une grille), effet skyfall en bruit de perlin 3D (`ondulations.c`)

- Récupération des **moyennes d’échantillonnage du son** pour en faire des hauteurs.  
- Dessin d’une grille et envoi des hauteurs dans une **texture pour le shader**.  
- Calcul de la **distance au centre** pour déterminer la bonne hauteur à utiliser (coordonnée de texture).  
- Oscillation sinusoïdale pour donner un effet de **vagues dynamiques**.  
- Bruit de Perlin 3D **dans un cylindre** pour simuler l’effet de chute dans l’atmosphère.

### Transition de textes avec bruit de perlin 2D (`credit.c`)

- Effet de **masquage dynamique** du texte avec un bruit de Perlin 2D.  
- Déplacement d’une valeur de seuil dans le temps : si la valeur du bruit dépasse ce seuil → transition du pixel.
