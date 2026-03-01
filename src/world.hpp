#pragma once
#include <memory>
#include <vector>

#include "entity.hpp"

class World {
public:
    World() {}

    void simulate(float deltaTime);

    const std::vector<std::unique_ptr<Entity>>& getEntities() const { return m_entities; }

    inline void addEntity(std::unique_ptr<Entity> entity) { m_entities.push_back(std::move(entity)); }

private:
    std::vector<std::unique_ptr<Entity>> m_entities;
};
