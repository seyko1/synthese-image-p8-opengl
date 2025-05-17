#version 330

in  vec2 vsoTexCoord;
out vec4 fragColor;

uniform sampler2D tex;
uniform float time;
uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

float noise(vec2 P) {
  vec2 Pi = ONE*floor(P)+ONEHALF;
  vec2 Pf = fract(P);             

  vec2 grad00 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  vec2 grad10 = texture(permTexture, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  vec2 grad01 = texture(permTexture, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  vec2 grad11 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  return n_xy;
}

void main(void) {
  float n = noise(vsoTexCoord * 50.);
  
  // Avancer de 2PI (6.28) en 7 sec
  // augmenter l'amplitude du cos à -2  2 puis -1,3 
  float t = cos(3.14 + 6.28 * time / 7.) * 3. + 1.;

  if (n - t > 0) {
    fragColor = vec4(0., 0., 0., 1.); 
  } else {
    vec4 c = texture(tex, vsoTexCoord);
    fragColor = vec4(c.rgb, 1.0);
  }
}
