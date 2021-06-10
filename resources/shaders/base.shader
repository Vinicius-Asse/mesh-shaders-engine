#shader vertex
#version 450

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 aCol;

out vec4 vCol;

uniform mat4 MVP;

void main(){

    gl_Position = MVP * vPos;
    vCol = aCol;
}

#shader fragment
#version 450

out vec4 color; 

in vec4 vCol; 

void main(){
    color = vCol;
}