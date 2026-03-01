#include "camera.hpp"

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float speed = m_movementSpeed * deltaTime;

    // Project forward onto the horizontal plane (XZ)
    glm::vec3 forward = glm::vec3(m_front.x, 0.0f, m_front.z);
    forward = glm::normalize(forward);

    // Project right onto the horizontal plane (or recompute from world up and forward)
    glm::vec3 right = glm::vec3(m_right.x, 0.0f, m_right.z);
    right = glm::normalize(right);

    if (direction == FORWARD) m_position += forward * speed;
    if (direction == BACKWARD) m_position -= forward * speed;
    if (direction == RIGHT) m_position += right * speed;
    if (direction == LEFT) m_position -= right * speed;
    if (direction == UP) m_position.y += speed;
    if (direction == DOWN) m_position.y -= speed;
}

void Camera::processMouseMovement(float xoffset, float yoffset, bool constrainPitch) {

    xoffset *= m_mouseSensitivity;
    yoffset *= m_mouseSensitivity;

    m_yaw += xoffset;
    m_pitch += yoffset;

    if (constrainPitch) {
        if (m_pitch > 89.0f) {
            m_pitch = 89.0f;
        }
        if (m_pitch < -89.0f) {
            m_pitch = -89.0f;
        }
    }
    updateCameraVectors();
}

void Camera::updateCameraVectors() {
    glm::vec3 front;
    front.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    front.y = sin(glm::radians(m_pitch));
    front.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    m_front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    m_right =
        glm::normalize(glm::cross(front, m_worldUp)); // normalize the vectors, because their length gets closer to 0
                                                      // the more you look up or down which results in slower movement.
    m_up = glm::normalize(glm::cross(m_right, m_front));
}

glm::mat4 Camera::getViewMatrix() { return glm::lookAt(m_position, m_position + m_front, m_up); }

// processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
void Camera::processMouseScroll(float yoffset) {
    m_zoom -= (float)yoffset;
    if (m_zoom < 1.0f) m_zoom = 1.0f;
    if (m_zoom > 45.0f) m_zoom = 45.0f;
}
