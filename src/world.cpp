#include "world.hpp"

void World::simulate(float deltaTime) {
    for (const auto& entity : m_entities) {
        entity->move(entity->getVelocity() * deltaTime);
    }
}
