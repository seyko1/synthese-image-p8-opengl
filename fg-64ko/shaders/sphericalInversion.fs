#version 330
out vec4 fragColor;

in vec2 fragCoord;

uniform float elapsed;
uniform vec2 resolution;

void main()
{
    float s = 50.;
    
    vec3 yellow = vec3(236. / 255., 202. / 255., 128. / 255.);
    vec3 blue = vec3(107. / 255., 178. / 255., 194. / 255.);

    vec2 nc = (fragCoord - (resolution.xy / 2.0));
    float dc = length(nc);
        
    float p = 100.;
    float idc = p * (p / dc);
    vec2 ic = vec2(
        nc.x * idc / dc + cos(elapsed * 0.9) * 700.,
        nc.y * idc / dc + sin(elapsed * 0.8) * 400.
    );
    
    float t = cos(elapsed * 0.5) * 0.5 + 0.5;
    ic.x = nc.x + t * (ic.x - nc.x);
    ic.y = nc.y + t * (ic.y - nc.y);
    
    if (abs(ic.x) > resolution.x / 0.5 || abs(ic.y) > resolution.y / 0.5) {
        fragColor = vec4(1.0);
        return;
    }
    
    int px = int(floor(ic.x / s));
    if (px < 0) px = -px;

    int py = int(floor(ic.y / s));
    if (py < 0) py = -py;
    
    if (px % 2 == py % 2) {
        fragColor = vec4(yellow, 1.);
    } else {
        fragColor = vec4(blue, 1.);;
    }
    return;
}