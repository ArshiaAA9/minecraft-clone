#pragma once
#include <glm/gtc/matrix_transform.hpp>

#include "entity.hpp"
#include "utils.hpp"

class World;

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific
// input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 10.5f;
const float SENSITIVITY = .1f;
const float ZOOM = 45.0f;

class Camera : public Entity {
public:
    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH)
        : Entity(position, SPEED)
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_mouseSensitivity(SENSITIVITY)
        , m_zoom(ZOOM) {
        m_worldUp = up;
        m_yaw = yaw;
        m_pitch = pitch;
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : Entity(glm::vec3(posX, posY, posZ), SPEED)
        , m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_mouseSensitivity(SENSITIVITY)
        , m_zoom(ZOOM) {
        m_worldUp = glm::vec3(upX, upY, upZ);
        m_yaw = yaw;
        m_pitch = pitch;
        updateCameraVectors();
    }

    void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 getViewMatrix();

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void processMouseScroll(float yoffset);

    void processKeyboard(Camera_Movement direction, float deltaTime);

    const glm::vec3& getPosition() const { return m_position; }

    void setTargetedBlock(const World& world);

    inline const HitTarget& getCrosshairTarget() const { return m_crosshairTarget; }

private:
    void updateCameraVectors();

    HitTarget rayCast(const World& world) const;

    // camera Attributes
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // euler Angles
    float m_yaw;
    float m_pitch;

    // camera options
    float m_mouseSensitivity;
    float m_zoom;

    // the position of the block which the player is looking at
    HitTarget m_crosshairTarget;
};
