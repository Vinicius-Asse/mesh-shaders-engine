#!vertex shader
#version 450

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 aCol;
layout(location = 2) in vec4 aNorm;

out vec4 vCol;
out vec3 vNorm;
out vec3 lDir;

uniform mat4 MVP;
uniform mat4 uModel;
uniform vec4 lightDir;

void main()
{
    gl_Position = MVP * vPos;

    vCol = aCol;
    vNorm = normalize(vec3(uModel * vec4(aNorm.xyz, 0.0)));
    lDir = normalize(-lightDir.xyz);
}

#!fragment shader
#version 450

out vec4 color; 

in vec4 vCol;
in vec3 vNorm;
in vec3 lDir;

uniform vec4 ligthCol;

void main()
{
    float ambient = 0.25;
    float diffuse = max(dot(vNorm, lDir), ambient);

    color = vCol;
}