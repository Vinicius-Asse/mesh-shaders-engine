#pragma once

#include <core/mesh.hpp>
#include <glm/glm.hpp>
#include <vector>

static const std::vector<Vertex> vertices = {
    //     POSITION       |        COLLOR      |     NORMALS     //
    { -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f },   // FRONT Upper Left
    {  0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f },   // FRONT Upper Right
    { -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f },   // FRONT Botton Left
    {  0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f, 1.0f },   // FRONT Botton Right

    { -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f,-1.0f },   // BACK Upper Left
    {  0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f,-1.0f },   // BACK Upper Right
    { -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f,-1.0f },   // BACK Botton Left
    {  0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 0.0f,-1.0f },   // BACK Botton Right

    { -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f },   // UPPER Upper Left
    {  0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f },   // UPPER Upper Right
    { -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f },   // UPPER Botton Left
    {  0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f, 1.0f, 0.0f },   // UPPER Botton Right

    { -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f,-1.0f, 0.0f },   // BOTTON Upper Left
    {  0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  0.0f,-1.0f, 0.0f },   // BOTTON Upper Right
    { -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f,-1.0f, 0.0f },   // BOTTON Botton Left
    {  0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  0.0f,-1.0f, 0.0f },   // BOTTON Botton Right

    { -0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },   // LEFT Upper Left
    { -0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },   // LEFT Upper Right
    { -0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },   // LEFT Botton Left
    { -0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f, -1.0f, 0.0f, 0.0f },   // LEFT Botton Right

    {  0.5f,  0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },   // LEFT Upper Left
    {  0.5f, -0.5f,  0.5f,  1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },   // LEFT Upper Right
    {  0.5f,  0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },   // LEFT Botton Left
    {  0.5f, -0.5f, -0.5f,  1.0f,  1.0f,  1.0f,  1.0f, 0.0f, 0.0f },   // LEFT Botton Right
};

static std::vector<GLint> indexes = {
    //FRONT FACE
    0, 2, 1,
    1, 2, 3,
    //BACK FACE
    4, 5, 6,
    5, 7, 6,
    //UPPER FACE
    8, 9, 10,
    9, 11, 10,
    //BOTTON FACE
    12, 14, 13,
    13, 14, 15,
    //LEFT FACE
    16, 18, 17,
    17, 18, 19,
    //RIGHT FACE
    20, 21, 22,
    21, 23, 22
};

class Cube : public Mesh {

public:
    Cube();
    Cube(std::vector<GLint> indices, std::vector<Vertex> vertex, Shader *shader);
    //~Cube();

    static Cube* getInstance(glm::vec3 position, glm::vec3 scale, Shader *shader);

    void onUpdate();
};