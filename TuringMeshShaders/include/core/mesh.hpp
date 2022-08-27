#pragma once

#include<core/structs.h>
#include<core/shader.hpp>
#include<core/camera.hpp>

#include<glad/glad.h>
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<glm/gtx/string_cast.hpp>
#include<glm/gtx/hash.hpp>
#include<glm/gtc/random.hpp>
#include<iostream>
#include<string>
#include<vector>

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
    Mesh(std::vector<GLint> indices, std::vector<Vertex> vertex, Shader *shader);
    //~Mesh();

    unsigned int VAO, VBO, EBO;
    int type = GL_TRIANGLES;

    void translate(glm::vec3 position); //TODO: Abstrair transformações em classe (Transform)
    void rotate(glm::vec3 rotation); //TODO: Abstrair transformações em classe (Transform)
    void scale(glm::vec3 scale); //TODO: Abstrair transformações em classe (Transform)


    void bind();
    void unbind();

    void draw();
    void free();
};