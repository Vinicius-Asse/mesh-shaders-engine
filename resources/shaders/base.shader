#shader vertex
#version 450

layout(location = 0) in vec4 vPos;
layout(location = 1) in vec4 aCol;
layout(location = 2) in vec4 aNorm;

out vec4 vCol;
out vec4 vNorm;

uniform mat4 MVP;

void main()
{
    gl_Position = MVP * vPos;

    vCol = aCol;
    vNorm = aNorm;
}

#shader fragment
#version 450

out vec4 color; 

in vec4 vCol;
in vec4 vNorm;

uniform vec4 ligthDir;
uniform vec4 ligthCol;

void main()
{
    float ambient = 0.25f;
    vec3 normal = normalize(vNorm.xyz);
    vec3 ligthDirection = normalize(-ligthDir.xyz);
    float diffuse = max(dot(normal, ligthDirection), ambient);

    color = vCol * ligthCol * diffuse;
}