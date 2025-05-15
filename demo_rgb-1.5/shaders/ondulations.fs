#version 330

in float lightIntensity;
in float dc;

out vec4 fragColor;

uniform float elapsed;

float map(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void main() {
     float angle = map(dc, 0, 1.4, 0, 12.6);
     float v = angle + elapsed;

     float r = map(sin(v), -1., 1., 0.4, 1.);
     float g = map(sin(v + 2), -1., 1., 0.4, 1.);
     float b = map(sin(v + 4), -1., 1., 0.4, 1.);
     fragColor = vec4(r, g, b, 1.0) * lightIntensity * lightIntensity;
}