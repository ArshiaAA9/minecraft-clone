#pragma once
#include <glm/glm.hpp>

class Ray {
    glm::vec3 origin;
    glm::vec3 end;
};

// used as the return type of the rayCast function
struct HitTarget {
    enum Type { MISS, BLOCK, ENTITY } type;

    glm::ivec3 blockPos;
    glm::vec3 hitPoint;
    glm::vec3 faceNormal; // e.g., (1,0,0) for right face
    float distance;

    static HitTarget miss() { return {MISS}; }
};

struct Utils {

    // implement this
    // static inline bool boxRayIntersection(const Ray& ray) { return false; }
};
