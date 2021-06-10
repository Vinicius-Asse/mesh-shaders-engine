#ifndef MESH_CLASS
#define MESH_CLASS

#include<core/shader.hpp>
#include<core/camera.hpp>

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
    Shader *shader;

    Vertex *vertex;
    unsigned int indicesCount = 0;
    unsigned int currAttr = 0;
    unsigned int nxtAttr = 0;

    glm::mat4 model;

    void addAttribute(GLenum type, int count, bool normalized=false);

public:
    Mesh(GLint indices[], unsigned int indicesCount, Vertex *vertex, unsigned int vertexCount, Shader *shader);
    ~Mesh();

    unsigned int VAO, VBO, EBO;

    void translate(glm::vec3 position); //TODO: Abstrair transformações em classe (Transform)

    void bind();
    void unbind();

    void draw(Camera camera);
};

#endif