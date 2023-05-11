#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec3 aColor;

uniform vec2 res;

uniform mat4 projection;
uniform mat4 model;

void main(){
    gl_Position = projection * model * vec4(aPos,1.0,1.0);
    //gl_Position = vec4(aPos/res*2-1, 1.0, 1.0);
}