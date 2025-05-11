#version 330
out vec4 fragColor;

in vec2 fragCoord;

uniform float elapsed;
uniform vec2 resolution;

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main() {
    float s = 50.0;
    
    vec3 yellow = vec3(236.0 / 255.0, 202.0 / 255.0, 128.0 / 255.0);
    vec3 blue   = vec3(107.0 / 255.0, 178.0 / 255.0, 194.0 / 255.0);

    /* La position écran fragCoord (0, 0) est en haut à gauche, on recentre le repère 0,0 au milieu de l'écran */
    vec2 nc = (fragCoord - (resolution.xy / 2.0));
    
    // Distance entre le fragment et le centre
    float dc = length(nc);

    // Rayon variable dans le temps
    float r = 250.0 + cos(elapsed) * 100.0;
    // float r = 50.0 + elapsed * elapsed;

    float idc = r * (r / dc);
    
    vec2 ic = vec2(
        nc.x * idc / dc + cos(elapsed) * 300.0,
        nc.y * idc / dc + sin(elapsed * 0.7) * 700.0
    );

    int px = int(floor(ic.x / s));
    if (px < 0) px = -px;

    int py = int(floor(ic.y / s));
    if (py < 0) py = -py;
    
    /* Plus on est proche du centre dc, plus la couleur est lumineuse. */
    /* Diminuer l'intensité avec le temps */
    float lightIntensity = map(dc, 0.0, resolution.x / 4.0, 0.5, 1.2 - elapsed * 0.1);
    if (px % 2 == py % 2) {
        fragColor = vec4(yellow * lightIntensity, 1.0);
    } else {
        fragColor = vec4(blue * lightIntensity, 1.0);
    }
}