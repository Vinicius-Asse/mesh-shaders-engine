#pragma once

#include<glad/glad.h>

struct Vec4
{
    float x, y, z, w;
};

struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
    GLfloat normals[3];
};

struct Point 
{
    float x, y, z, value;
    float vx, vy, vz;
};

struct Triangle 
{
    Vec4 ver0, ver1, ver2;
    Vec4 normal;
};