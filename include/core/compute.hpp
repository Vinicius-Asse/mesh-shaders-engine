#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<unordered_map>

#include<core/utils.hpp>
#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/program.hpp>
#include<core/computeshader.hpp>
#include<core/parameters.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>


class Compute
{
private:
    std::vector<Vertex> vertexBuff;
    std::vector<GLint> indicesBuff;

    Point *points;
    Mesh *mesh;

    Cube *wiredCube;

    Parameters *param;

    ComputeShader *computeShader;
    Shader *meshShader;

    void onCreate();

public:
    Compute(Parameters*, Point*);

    std::map<std::string, std::string> meshInfo;

    void start();
    void input(SDL_Event*);
    void update();
    void draw();
    void generateMesh();

private:
    glm::vec3 interpolate(Point, Point);
    void smoothShading(Triangle *triangles, int trizCount);
    void flatShading(Triangle *triangles, int trizCount);
    bool pushUniqueVertices(std::unordered_map<glm::vec3, GLint>*, glm::vec3, glm::vec3, GLint);
};