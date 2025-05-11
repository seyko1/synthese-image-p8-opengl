#version 330
out vec4 fragColor;

in vec2 fragCoord;

uniform float elapsed;
uniform vec2 resolution;

void main() {
    float s = 50.0;
    
    vec3 yellow = vec3(236.0 / 255.0, 202.0 / 255.0, 128.0 / 255.0);
    vec3 blue   = vec3(107.0 / 255.0, 178.0 / 255.0, 194.0 / 255.0);

    /* la position écran fragCoord (0, 0) est en haut à gauche, on recentre le repère 0,0 au milieu de l'écran */
    vec2 nc = (fragCoord - (resolution.xy / 2.0));
    
    // distance entre le fragment et le centre
    float dc = length(nc);
        
    float p = 250.0 + cos(elapsed) * 100.;
    float idc = p * (p / dc);
    vec2 ic = vec2(
        nc.x * idc / dc + cos(elapsed) * 300.0,
        nc.y * idc / dc + sin(elapsed * 0.7) * 700.0
    );
   
 
    int px = int(floor(ic.x / s));
    if (px < 0) px = -px;

    int py = int(floor(ic.y / s));
    if (py < 0) py = -py;
    
    if (px % 2 == py % 2) {
        fragColor = vec4(yellow, 1.0);
    } else {
        fragColor = vec4(blue, 1.0);;
    }
}