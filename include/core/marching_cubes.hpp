#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<unordered_map>

#include<core/program.hpp>
#include<core/utils.hpp>
#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/parameters.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>

class MarchingCubes : public Program
{
private:
    std::vector<Vertex> vertexBuff;
    std::vector<GLint> indicesBuff;

    Point* points;
    Mesh* mesh;

    Cube* wiredCube;

    void onCreate();

public:
    MarchingCubes(Parameters*, Shader*, Point*);

    void start();
    void input(SDL_Event*);
    void update();
    void draw();

private:
    Shader* baseShader;

    Point getPoint(int x, int y, int z);
    Mesh* generateMesh(int, int, int, Shader*);
    glm::vec3 interpolate(Point, Point);
    void smoothShading(std::vector<Triangle> triangles);
    void flatShading(std::vector<Triangle> triangles);
    bool pushUniqueVertices(std::unordered_map<glm::vec3, GLint>*, glm::vec3, glm::vec3, GLint);
};