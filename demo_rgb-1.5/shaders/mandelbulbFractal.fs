#version 330
out vec4 fragColor;

in float vDepth;

vec3 hsb2rgb(in vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

    return c.z + c.y * (rgb-0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main() {
    float brightness = map(vDepth, 0, 20, 0., 1.);
    vec3 hsb = vec3(brightness, 1.0, 0.5); 

    fragColor = vec4(hsb2rgb(hsb), 1.0);
}