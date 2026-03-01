#pragma once
#include <glm/glm.hpp>

#include "glm/fwd.hpp"

class Entity {
public:
    Entity(glm::vec3 position, float movementSpeed)
        : m_position(position)
        , m_movementSpeed(movementSpeed) {}

    virtual ~Entity() = default;

    inline glm::vec3 getPosition() const { return m_position; }

    inline glm::vec3 getVelocity() const { return m_velocity; }

    inline void setVelocity(glm::vec3 velocity) { m_velocity = velocity; }

    inline void moveTo(glm::vec3 position) {
        if (isValidPosition(position)) m_position = position;
    }

    inline void move(glm::vec3 amount) { m_position += amount; }

    // TODO: implement checks for this function
    // right now i dont know what the constraints should be i leave it empty
    inline bool isValidPosition(glm::vec3) const { return true; }

protected:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_movementSpeed;
};
