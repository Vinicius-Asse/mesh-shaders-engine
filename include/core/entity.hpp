#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <vector>

#include <core/system/entity_system.hpp>

class Entity {
public:
    unsigned int id;

    Entity();
    //~Entity();

    virtual void onStart() {};
    virtual void onUpdate(){};
    virtual void onDraw()  {};
};