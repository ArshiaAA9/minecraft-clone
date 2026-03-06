#pragma once
#include <glm/glm.hpp>

struct EntityState {
    bool jumping;
    bool falling;
    bool walking;
    bool godmode = false;
};

class Entity {
public:
    Entity(glm::vec3 position, float movementSpeed)
        : m_position(position)
        , m_velocity(0.0f)
        , m_movementSpeed(movementSpeed)
        , m_state{} //(zero‑initializes all bools)
    {}

    virtual ~Entity() = default;

    inline glm::vec3 getPosition() const { return m_position; }

    inline glm::vec3 getVelocity() const { return m_velocity; }

    inline void setVelocity(glm::vec3 velocity) { m_velocity = velocity; }

    inline void moveTo(glm::vec3 position) {
        if (isValidPosition(position)) m_position = position;
    }

    inline void move(glm::vec3 amount) { m_position += amount; }

    // TODO: implement checks for this function
    // if entity is inside object etc...
    inline bool isValidPosition(glm::vec3) const { return true; }

    // no setter for state because Entity should manage it own state
    const EntityState& getState() const { return m_state; }

    void switchGodmode();

protected:
    glm::vec3 m_position;
    glm::vec3 m_velocity;
    float m_movementSpeed;
    EntityState m_state;
};
