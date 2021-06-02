#shader vertex
#version 450

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 aCol;

out vec4 vCol;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){

    gl_Position = proj * view * model * vPos;
    vCol = aCol;
}

#shader fragment
#version 450

out vec4 color; 

in vec4 vCol; 

void main(){
    color = vCol;
}