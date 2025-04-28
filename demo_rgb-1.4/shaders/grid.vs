#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 projection, modelView;
uniform sampler2D tex;

void main() {
    /* distance du vertex au centre de la grille */
    float d = (pos.x * pos.x) + (pos.z * pos.z);
    d = sqrt(d);

    /* Normaliser i sur une échelle [0;1] car la grille est carrée et centrée en 0,0 */
    /* distance max jusqu'à un coin : sqrt(2) = 1.42 (?) */
    float i = d / 1.42;

    // modifier la hauteur du vertex en piochant dans un index de la texture
    vec3 p = pos + vec3(0.0, texture(tex, vec2(1, i)).r / 4.0, 0.0);

    gl_Position = projection * modelView * vec4(p, 1.0);
}