#version 330
layout(location=0)in vec3 pos;layout(location=1)in float depth;out float vDepth;uniform mat4 modelView,projection;void main(){gl_Position=projection*modelView*vec4(pos,1.);vDepth=depth;}