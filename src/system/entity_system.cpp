#include <core/system/entity_system.hpp>

EntitySystem* EntitySystem::Main = nullptr;

EntitySystem::EntitySystem() {
    if (Main == nullptr) setActive();
}

void EntitySystem::setActive(){
    Main = this;
}

void EntitySystem::registerEntity(Entity entity){
    entities.push_back(entity);
}

void EntitySystem::callStart(){
    for(Entity entity : entities) {
        entity.onStart();
    }
}

void EntitySystem::callUpdate(){
    for(Entity entity : entities) {
        entity.onUpdate();
    }
}

void EntitySystem::callDraw(){
    for(Entity entity : entities) {
        entity.onDraw();
    }
}