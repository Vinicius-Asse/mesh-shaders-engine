#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>
#include<unordered_map>

#include<core/utils.hpp>
#include<core/camera.hpp>
#include<core/mesh.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/marching_cubes.hpp>
#include<core/parameters.hpp>
#include<core/shader.hpp>
#include<core/constants/tables.hpp>


class MarchingCubesMeshImpl : public Program
{
    int qX, qY, qZ;

    Cube *wiredCube;

    Shader *meshShader;

public:
    MarchingCubesMeshImpl(Parameters*, Shader*, Point*);

    void start();
    void input(SDL_Event*);
    void update();
    void draw();
    void executeMeshShader();
};