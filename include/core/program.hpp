#pragma once

#include<SDL2/SDL.h>
#include<glm/glm.hpp>

#include<core/camera.hpp>
#include<core/cube.hpp>
#include<core/sphere.hpp>
#include<core/shader.hpp>


class Program
{
private:
    SDL_Window *window;

    void onCreate();

public:
    Program(SDL_Window*);
    //~Program();

    void start();
    void input(SDL_Event*);
    void update();
    void draw();
};