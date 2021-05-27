#ifndef MESH_CLASS
#define MESH_CLASS

#include<glad/glad.h>
#include<iostream>
#include<string>

struct Vertex
{
    float position[3];
};

class Mesh 
{
private:
    Vertex *vertex;
    unsigned int attrCount = 0;

    void addAttribute(unsigned int pos, unsigned int type, int count, bool normalized=false);

public:
    Mesh(Vertex *vertex, unsigned int count);

    unsigned int uId;

    void bind();
    void unbind();
};

#endif