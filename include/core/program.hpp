#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<glm/gtc/noise.hpp>
#include<unordered_map>
#include<chrono>

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
};

struct vec4
{
    float x, y, z, w;
};

struct TriangleSSBO {
    vec4 ver0, ver1, ver2, normal;
};

struct Triangle {
    glm::vec3 ver0;
    glm::vec3 ver1;
    glm::vec3 ver2;
    glm::vec3 normal;
};

class Program
{
private:
    SDL_Window *window;

    std::vector<Vertex> vertexBuff;
    std::vector<GLint> indicesBuff;

    Point*** points;
    Mesh* mesh;

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
    GLfloat generateRandomValue(float, float, float, float, glm::vec3);
    GLfloat remap(float, float, float, float, float);
    Mesh* generateMesh(int, int, int, Shader*);
    Mesh* generateMeshGPU(int, int, int, Shader*);
    bool isValidLine(int, int, int);
    bool isValidCube(int, int, int);
    glm::vec3 interpolate(Point, Point);
    Vertex createVertex(glm::vec3, glm::vec3);
    glm::vec3 getNormalVector(glm::vec3, glm::vec3, glm::vec3);
    void smoothShading(std::vector<Triangle> triangles);
    void flatShading(std::vector<Triangle> triangles);
    bool pushUniqueVertices(std::unordered_map<glm::vec3, GLint>*, glm::vec3, glm::vec3, GLint);
};