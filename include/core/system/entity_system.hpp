#pragma once

#include <vector>
#include <SDL2/SDL.h>
#include <core/entity.hpp>

class EntitySystem {
private:
    std::vector<Entity> entities;

public:
    static EntitySystem* Main;

    EntitySystem();
    //~EntitySystem();

    void setActive();

    void registerEntity(Entity);

    void callStart();
    void callUpdate();
    void callDraw();
};