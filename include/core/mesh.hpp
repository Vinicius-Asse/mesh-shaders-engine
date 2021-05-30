#ifndef MESH_CLASS
#define MESH_CLASS

#include<glad/glad.h>
#include<iostream>
#include<string>

struct Vertex
{
    GLfloat position[3];
    GLfloat color[3];
};

class Mesh 
{
private:
    Vertex *vertex;
    unsigned int indicesCount = 0;
    unsigned int currAttr = 0;
    unsigned int nxtAttr = 0;

    void addAttribute(GLenum type, int count, bool normalized=false);

public:
    Mesh(GLint indices[], unsigned int indicesCount, Vertex *vertex, unsigned int vertexCount);
    ~Mesh();

    unsigned int VAO, VBO, EBO;

    void bind();
    void unbind();

    void draw();
};

#endif