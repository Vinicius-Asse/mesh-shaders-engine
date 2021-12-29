#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<unordered_map>

#include<core/utils.hpp>
#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/computeshader.hpp>
#include<core/parameters.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>

#define LOG(msg) std::cout << msg << std::endl

struct Point {
    float x, y, z, value;
    float vx, vy, vz;
};

struct Vec4
{
    float x, y, z, w;
};

struct Triangle {
    Vec4 ver0, ver1, ver2;
    Vec4 normal;
};

class Program
{
private:
    std::vector<Vertex> vertexBuff;
    std::vector<GLint> indicesBuff;

    Point*** points;
    Mesh* mesh;

    Cube* wiredCube;

    Parameters *param;

    void onCreate();

public:
    Program(Parameters*);

    void start();
    void input(SDL_Event*);
    void update();
    void draw();

private:
    Point*** instantiatePoints(int, int, int);
    Point*** instantiatePointsGPU(int, int, int);
    Mesh* generateMesh(int, int, int, Shader*);
    Mesh* generateMeshGPU(int, int, int, Shader*);
    glm::vec3 interpolate(Point, Point);
    void smoothShading(std::vector<Triangle> triangles);
    void flatShading(std::vector<Triangle> triangles);
    bool pushUniqueVertices(std::unordered_map<glm::vec3, GLint>*, glm::vec3, glm::vec3, GLint);
};