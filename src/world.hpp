#pragma once
#include <SFML/System/Vector3.hpp>
#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

#include "camera.hpp"
#include "chunk.hpp"
#include "entity.hpp"
#include "glm/fwd.hpp"
#include "shader.hpp"

// provides the hash for glm::ivec2
namespace std {
template <>
struct hash<glm::ivec2> {
    size_t operator()(const glm::ivec2& v) const noexcept {
        size_t h1 = hash<int>()(v.x);
        size_t h2 = hash<int>()(v.y);
        return h1 ^ (h2 * 2654435761u + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
    }
};
} // namespace std

class World {
public:
    World() {}

    ~World() {}

    // update entity position, physics ,etc.
    void update(float deltaTime);
    // checks which chunks are visible to the camera and renders them
    void renderVisibleChunks(const Camera& camera, const Shader& shader) const;
    // return the blockID of the given block position inside world
    int8_t getBlockAt(const glm::vec3& worldPos) const;
    int8_t getBlockAt(int x, int y, int z) const;
    // generates a grid 2x2 of chunks and
    // adds them to the m_chunks and calls
    void genChunks();

    inline const std::vector<std::unique_ptr<Entity>>& getEntities() const { return m_entities; }

    inline void addEntity(std::unique_ptr<Entity> entity) { m_entities.push_back(std::move(entity)); }

private:
    inline static constexpr float GRAVITY = -9.8;
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>> m_chunks;
    std::vector<std::unique_ptr<Entity>> m_entities;
};
