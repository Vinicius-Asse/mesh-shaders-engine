#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<glm/gtc/noise.hpp>

#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>

struct Point {
    glm::vec3 position;
    float value;
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

    double pointDencity;

    glm::vec3 worldBounds;

    std::vector<Vertex> vertexBuff;
    std::vector<GLint> indicesBuff;

    Point*** points;
    Mesh* mesh;

    void onCreate();

public:
    Program(SDL_Window*, glm::vec3, double);

    void start();
    void input(SDL_Event*);
    void update();
    void draw();

private:
    Point*** instantiatePoints(int, int, int);
    float generateRandomValue(int, int, int, float, glm::vec3);
    float remap(float, float, float, float, float);
    Mesh* generateMesh(Shader*);
    bool isValidLine(int, int, int);
    bool isValidCube(int, int, int);
    glm::vec3 interpolate(Point, Point);
    Vertex createVertex(glm::vec3, glm::vec3);
    glm::vec3 getNormalVector(glm::vec3, glm::vec3, glm::vec3);
};