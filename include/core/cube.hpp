#pragma once

#include <core/mesh.hpp>
#include <glm/glm.hpp>
#include <vector>

static const std::vector<Vertex> vertices = {
    //     POSITION       |        COLLOR     //
    { -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  1.0f },   // FRONT Upper Left
    {  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  1.0f },   // FRONT Upper Right
    { -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  0.0f },   // FRONT Botton Left
    {  0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f },   // FRONT Botton Right
    { -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  1.0f },   // BACK Upper Left
    {  0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  1.0f },   // BACK Upper Right
    { -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  0.0f },   // BACK Botton Left
    {  0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f }    // BACK Botton Right
};

static std::vector<GLint> indexes = {
    //FRONT FACE
    0, 1, 2,
    1, 2, 3,
    //BACK FACE
    4, 5, 6,
    5, 6, 7,
    //UPPER FACE
    0, 1, 4,
    4, 1, 5,
    //BOTTON FACE
    2, 3, 6,
    6, 3, 7,
    //RIGHT FACE
    1, 3, 7,
    1, 7, 5,
    //LEFT FACE
    0, 2, 6,
    0, 6, 4
};

class Cube : public Mesh {

public:
    Cube();
    Cube(std::vector<GLint> indices, std::vector<Vertex> vertex, Shader *shader);
    //~Cube();

    static Cube getInstance(glm::vec3 position, glm::vec3 scale, Shader *shader);

    void onUpdate();
};