#version 330
out vec4 fragColor;

uniform vec4 color;
uniform int isAlive;

vec3 hsb2rgb(in vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

    return c.z + c.y * (rgb-0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

void main() {
     if (isAlive == 1) {
        fragColor = vec4(hsl2rgb(vec3(color.xyz)), 1.0);
     } else {
        fragColor = color;
     }
}