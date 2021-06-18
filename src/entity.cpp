#include <core/entity.hpp>

Entity::Entity() {
    id = 0; //TODO: Gerar id Unico para cada entidade
    EntitySystem::Main->registerEntity(*this);
    std::cout << "[" << std::to_string(id) << "] Entity Registred!" << std::endl;
}