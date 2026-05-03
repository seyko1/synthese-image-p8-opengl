#version 330

in float lightIntensity;
in float dc;
in vec2 vsoTexCoord;

out vec4 fragColor;

uniform float elapsed;
uniform int drawSky;

uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
    return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

float noise(vec3 P) {
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

float map(float x, float in_min, float in_max, float out_min, float out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void tapis() {
    float angle = map(dc, 0, 1.4, 0, 12.6);
    float v = angle + elapsed;

    float r = map(sin(v), -1., 1., 0.4, 1.);
    float g = map(sin(v + 2), -1., 1., 0.4, 1.);
    float b = map(sin(v + 4), -1., 1., 0.4, 1.);
    fragColor = vec4(r, g, b, 1.0) * lightIntensity * lightIntensity;
}

void cylinder() {
    vec2 newCoord = vsoTexCoord * 400.;
    float x3 = newCoord.x * 6.28 / 400.;
    float f = 0.8;
    float r = 0.4 * f;

    float v1 = noise(vec3(
        cos(x3) * r,
        f * newCoord.y * 0.01 + -elapsed * 0.2 * 24.,
        sin(x3) * r
    )) * 0.5 + 0.5;
    
    float v2 = noise(vec3(
        cos(x3) * r,
        f * newCoord.y * 0.001 + -elapsed * 0.02 * 24.,
        sin(x3) * r
    )) * 0.5 + 0.5;
    float v3 = noise(vec3(
        cos(x3) * r * 0.1 + 2.,
        f * newCoord.y * 0.001 + -elapsed * 0.06 * 24.,
        sin(x3) * r * 0.1
    )) * 0.5 + 0.5;
    
    fragColor = vec4(v1, v1, v1, 1.);
    fragColor = vec4(
        1. - (v1 * v1 * v2 + v3) * 1.,
        1. - (v1 * v1 * v2 + v3) * 0.75,
        1. - (v1 * v1 * v2 + v3) * 0.5,
        1.0
    );
}

void main() {
  if (drawSky == 1) {
        cylinder();
    } else {
        tapis();
    }
}