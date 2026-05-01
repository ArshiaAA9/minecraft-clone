#include "camera.hpp"

#include <cmath>

#include "glm/geometric.hpp"
#include "utils.hpp"
#include "world.hpp"

void Camera::processKeyboard(Camera_Movement direction, float deltaTime) {
    float speed = m_state.godmode ? m_godmodeSpeed * deltaTime : m_movementSpeed * deltaTime;

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

void Camera::setTargetedBlock(const World& world) {
    HitTarget target = rayCast(world);
    m_crosshairTarget = target;
}

HitTarget Camera::rayCast(const World& world) const {
    // casts a ray in the direction the player is looking
    // if it finds a block in that direction
    // it sets the m_crosshairTarget to the possion of the targeted block
    //
    // this function should only get called when the angles of the camera are changed

    // formula for casting a ray
    // end = Start + ray.direction * ray.getDistance();

    float maxDistance = 4.0f;
    glm::vec3 dir = glm::normalize(m_front);
    glm::vec3 origin = m_position;

    // Current integer voxel coordinates
    int voxelX = static_cast<int>(std::floor(origin.x));
    int voxelY = static_cast<int>(std::floor(origin.y));
    int voxelZ = static_cast<int>(std::floor(origin.z));

    // Step direction
    glm::ivec3 step;
    step.x = (dir.x > 0) ? 1 : (dir.x < 0) ? -1 : 0;
    step.y = (dir.y > 0) ? 1 : (dir.y < 0) ? -1 : 0;
    step.z = (dir.z > 0) ? 1 : (dir.z < 0) ? -1 : 0;

    // tDelta: distance to cross one full voxel
    glm::vec3 tDelta;
    tDelta.x = (dir.x != 0) ? 1.0f / std::abs(dir.x) : std::numeric_limits<float>::infinity();
    tDelta.y = (dir.y != 0) ? 1.0f / std::abs(dir.y) : std::numeric_limits<float>::infinity();
    tDelta.z = (dir.z != 0) ? 1.0f / std::abs(dir.z) : std::numeric_limits<float>::infinity();

    // tMax: distance to first voxel boundary in each axis
    glm::vec3 tMax;
    if (dir.x > 0) {
        tMax.x = (voxelX + 1 - origin.x) * tDelta.x;
    } else if (dir.x < 0) {
        tMax.x = (origin.x - voxelX) * tDelta.x;
    } else {
        tMax.x = std::numeric_limits<float>::infinity();
    }

    if (dir.y > 0) {
        tMax.y = (voxelY + 1 - origin.y) * tDelta.y;
    } else if (dir.y < 0) {
        tMax.y = (origin.y - voxelY) * tDelta.y;
    } else {
        tMax.y = std::numeric_limits<float>::infinity();
    }

    if (dir.z > 0) {
        tMax.z = (voxelZ + 1 - origin.z) * tDelta.z;
    } else if (dir.z < 0) {
        tMax.z = (origin.z - voxelZ) * tDelta.z;
    } else {
        tMax.z = std::numeric_limits<float>::infinity();
    }

    // Traverse
    float currentDist = 0.0f;
    while (currentDist < maxDistance) {
        uint8_t block = world.getBlockAt(voxelX, voxelY, voxelZ);
        // Check if current voxel is solid (non-air)
        if (BlockRegistry::isSolid(block)) {
            // Hit detected
            glm::vec3 hitPos = origin + dir * currentDist;
            // Determine face normal based on which axis we just crossed
            // The smallest tMax indicates the axis we crossed to enter this voxel
            // But careful: when we first enter a solid voxel, the currentDist is the entry distance.
            // We can also compute the face by looking at the direction we came from.
            // A simpler approach: after we know we hit, the face normal is opposite to the step direction of the axis
            // that had the smallest tMax in the *previous* step. However, because we are inside the voxel now, we can
            // compute which face was hit by checking the entry point relative to block boundaries. One robust method:
            // compute the intersection point with the six planes of the block and pick the closest. But for speed, we
            // can record which axis we stepped when we entered this voxel. In the loop, before stepping, we have the
            // current tMax values. The smallest tMax indicates the axis we will step next. So if we are now inside a
            // solid voxel, the face we came through is the opposite of that step direction. To implement that, we need
            // to know which axis caused the step into this voxel. Let's restructure slightly: store the axis that
            // caused the last step. I'll show an alternative that computes the hit face from the entry point.

            // Compute exact hit point on block surface
            glm::vec3 entryPoint = hitPos; // The point where ray entered the block
            // Now find which face was hit:
            glm::vec3 faceNormal(0.0f);
            float epsilon = 0.001f; // small bias to avoid floating errors
            if (std::abs(entryPoint.x - voxelX) < epsilon) faceNormal = glm::vec3(-1, 0, 0);
            else if (std::abs(entryPoint.x - (voxelX + 1)) < epsilon)
                faceNormal = glm::vec3(1, 0, 0);
            else if (std::abs(entryPoint.y - voxelY) < epsilon)
                faceNormal = glm::vec3(0, -1, 0);
            else if (std::abs(entryPoint.y - (voxelY + 1)) < epsilon)
                faceNormal = glm::vec3(0, 1, 0);
            else if (std::abs(entryPoint.z - voxelZ) < epsilon)
                faceNormal = glm::vec3(0, 0, -1);
            else if (std::abs(entryPoint.z - (voxelZ + 1)) < epsilon)
                faceNormal = glm::vec3(0, 0, 1);
            else {
                // Should not happen, but fallback to direction opposite of step
                // based on which axis we stepped last (you'd need to track that)
            }

            HitTarget hit;
            hit.type = HitTarget::BLOCK;
            hit.blockPos = glm::ivec3(voxelX, voxelY, voxelZ);
            hit.hitPoint = entryPoint;
            hit.faceNormal = faceNormal;
            hit.distance = currentDist;
            return hit;
        }
        // Advance to next voxel: find smallest tMax
        if (tMax.x < tMax.y && tMax.x < tMax.z) {
            currentDist = tMax.x;
            voxelX += step.x;
            tMax.x += tDelta.x;
        } else if (tMax.y < tMax.z) {
            currentDist = tMax.y;
            voxelY += step.y;
            tMax.y += tDelta.y;
        } else {
            currentDist = tMax.z;
            voxelZ += step.z;
            tMax.z += tDelta.z;
        }
    }
    return HitTarget::miss();
}
