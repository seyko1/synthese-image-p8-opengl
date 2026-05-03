#version 330
out vec4 fragColor;

in vec3 vPos;

uniform vec4 color;
uniform int isAlive;
uniform int useNoise;

uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

float noise(vec3 P)
{
  vec3 Pi = ONE*floor(P)+ONEHALF;
  vec3 Pf = fract(P);    

  float perm00 = texture(permTexture, Pi.xy).a ;
  vec3  grad000 = texture(permTexture, vec2(perm00, Pi.z)).rgb * 4.0 - 1.0;
  float n000 = dot(grad000, Pf);
  vec3  grad001 = texture(permTexture, vec2(perm00, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n001 = dot(grad001, Pf - vec3(0.0, 0.0, 1.0));

  float perm01 = texture(permTexture, Pi.xy + vec2(0.0, ONE)).a ;
  vec3  grad010 = texture(permTexture, vec2(perm01, Pi.z)).rgb * 4.0 - 1.0;
  float n010 = dot(grad010, Pf - vec3(0.0, 1.0, 0.0));
  vec3  grad011 = texture(permTexture, vec2(perm01, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n011 = dot(grad011, Pf - vec3(0.0, 1.0, 1.0));

  float perm10 = texture(permTexture, Pi.xy + vec2(ONE, 0.0)).a ;
  vec3  grad100 = texture(permTexture, vec2(perm10, Pi.z)).rgb * 4.0 - 1.0;
  float n100 = dot(grad100, Pf - vec3(1.0, 0.0, 0.0));
  vec3  grad101 = texture(permTexture, vec2(perm10, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n101 = dot(grad101, Pf - vec3(1.0, 0.0, 1.0));

  float perm11 = texture(permTexture, Pi.xy + vec2(ONE, ONE)).a ;
  vec3  grad110 = texture(permTexture, vec2(perm11, Pi.z)).rgb * 4.0 - 1.0;
  float n110 = dot(grad110, Pf - vec3(1.0, 1.0, 0.0));
  vec3  grad111 = texture(permTexture, vec2(perm11, Pi.z + ONE)).rgb * 4.0 - 1.0;
  float n111 = dot(grad111, Pf - vec3(1.0, 1.0, 1.0));

  vec4 n_x = mix(vec4(n000, n001, n010, n011),
                 vec4(n100, n101, n110, n111), fade(Pf.x));

  vec2 n_xy = mix(n_x.xy, n_x.zw, fade(Pf.y));

  float n_xyz = mix(n_xy.x, n_xy.y, fade(Pf.z));

  return n_xyz;
}

vec3 hsb2rgb(in vec3 c) {
    vec3 rgb = clamp(abs(mod(c.x * 6.0 + vec3(0.0, 4.0, 2.0), 6.0) - 3.0) - 1.0, 0.0, 1.0);

    return c.z + c.y * (rgb-0.5) * (1.0 - abs(2.0 * c.z - 1.0));
}

void main() {
      if (useNoise == 1) {
         float n = noise(vPos * 300.0);
         float stars = step(0.80, n);

         vec3 starColor = vec3(1.0);
         vec3 skyColor = vec3(0.02, 0.02, 0.05);

         /* Interpolation linéaire entre les deux couleurs */
         fragColor = vec4(mix(skyColor, starColor, stars), 1.0);
         return;
      }

     if (isAlive == 1) {
        fragColor = vec4(hsb2rgb(color.xyz), 0.7);
     } else {
        fragColor = color;
     }
}