#version 330
out vec4 fragColor;

in float vPosHeight;
uniform int isSky;

vec3 hsb2rgb(in vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

    return c.z + c.y * (rgb-0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

void main() {
     float minGreyValue = 0.2f;
     float amplitude = 0.5f;

     float value = minGreyValue + vPosHeight * amplitude;

     vec3 hsb = vec3(value, 0.4, 0.6);
     vec3 rgb = hsb2rgb(hsb);

     if (isSky == 1) {
        fragColor = vec4(rgb, 1.0) * 0.7 + vec4(0.1, 0.1, 0.5, 1.0);
     } else {
        fragColor = vec4(rgb, 1.0);
     }
}