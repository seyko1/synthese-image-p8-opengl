#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 modelView, projection;
uniform float time;
uniform float speed;

out float vPosHeight;

uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
  // return t*t*(3.0-2.0*t); // Old fade, yields discontinuous second derivative
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

float noise(vec2 P)
{
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


void main() {
    float y = noise(vec2(
        pos.x * 10.0f,
        (pos.z * 10.0f) - time * speed
    ));

    vec3 p = pos + vec3(0.0, y, 0.0);

    gl_Position = projection * modelView * vec4(p, 1.0); 
    vPosHeight = p.y;
}