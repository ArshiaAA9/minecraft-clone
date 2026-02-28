#pragma once

#include <glm/gtc/matrix_transform.hpp>

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific
// input methods
enum Camera_Movement { FORWARD, BACKWARD, LEFT, RIGHT, UP, DOWN };

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = .1f;
const float ZOOM = 45.0f;

class Camera {
public:
    // constructor with vectors
    Camera(
        glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
        float yaw = YAW,
        float pitch = PITCH)
        : m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_movementSpeed(SPEED)
        , m_mouseSensitivity(SENSITIVITY)
        , m_zoom(ZOOM) {
        m_position = position;
        m_worldUp = up;
        m_yaw = yaw;
        m_pitch = pitch;
        updateCameraVectors();
    }

    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
        : m_front(glm::vec3(0.0f, 0.0f, -1.0f))
        , m_movementSpeed(SPEED)
        , m_mouseSensitivity(SENSITIVITY)
        , m_zoom(ZOOM) {
        m_position = glm::vec3(posX, posY, posZ);
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

private:
    void updateCameraVectors();

    // camera Attributes
    glm::vec3 m_position;
    glm::vec3 m_front;
    glm::vec3 m_up;
    glm::vec3 m_right;
    glm::vec3 m_worldUp;

    // euler Angles
    float m_yaw;
    float m_pitch;

    // camera options
    float m_movementSpeed;
    float m_mouseSensitivity;
    float m_zoom;
};
