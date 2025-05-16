#version 330
uniform sampler2D tex;in vec2 vsoTexCoord;out vec4 fragColor;void main(void){vec4 b=texture(tex,vsoTexCoord);fragColor=vec4(b.rgb,1.);}