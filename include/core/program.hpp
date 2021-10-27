#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<glm/gtc/noise.hpp>
#include<unordered_map>

#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/computeshader.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>

struct Point {
    glm::vec3 position;
    double value;
};

struct SSBOPoint {
    float x, y, z, value;
};

// struct cmpVec3 {
//     size_t operator()(const glm::vec3& a) const {
//         return std::hash<int>()(a.x) ^ std::hash<int>()(a.y);
//     }

//     bool operator()(const glm::vec3& a, const glm::vec3& b) const {
//         return glm::all(glm::lessThan(a, b));
//     }
// };

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

    glm::vec3 worldBounds;

    std::vector<Vertex> vertexBuff;
    std::vector<GLint> indicesBuff;

    Point*** points;
    Mesh* mesh;

    void onCreate();

public:
    Program(SDL_Window*, glm::vec3);

    void start();
    void input(SDL_Event*);
    void update();
    void draw();

private:
    Point*** instantiatePoints(int, int, int);
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