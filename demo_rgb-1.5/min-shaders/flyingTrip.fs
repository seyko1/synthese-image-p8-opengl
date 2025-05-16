version 330
out vec4 fragColor;in float vPosHeight;uniform int isSky;vec3 b(in vec3 d){vec3 e=clamp(abs(mod(d.x*6.+vec3(.0,4.,2.),6.)-3.)-1.,.0,1.);return d.z+d.y*(e-.5)*(1.-abs(2.*d.z-1.));}void main(){float f=.2f;float g=.5f;float h=f+vPosHeight*g;vec3 i=vec3(h,.4,.6);vec3 j=b(i);if(isSky==1){fragColor=vec4(j,1.)*.7+vec4(.1,.1,.5,1.);}else{fragColor=vec4(j,1.);}}
