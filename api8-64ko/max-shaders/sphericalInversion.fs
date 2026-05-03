#version 330
out vec4 fragColor;

in vec2 fragCoord;

uniform float elapsed;
uniform vec2 resolution;

vec3 hsb2rgb(in vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

    return c.z + c.y * (rgb-0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

void main() {
    float s = 50.;
    
    float hue = elapsed * 0.1;
    vec3 hsb = vec3(hue, 0.4, 0.6);

    vec3 rgb = hsb2rgb(hsb);

    vec3 c = hsb2rgb(vec3(hsb.x, hsb.y, hsb.z));
    vec3 blue = hsb2rgb(vec3(hsb.x + 0.5, hsb.y, hsb.z));

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
        fragColor = vec4(c, 1.);
    } else {
        fragColor = vec4(blue, 1.);;
    }
    return;
}