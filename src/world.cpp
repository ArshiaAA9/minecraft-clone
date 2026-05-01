#include "world.hpp"

#include <cstdint>
#include <iostream>
#include <memory>

#include "block-registry.hpp"
#include "shader.hpp"

void World::update(float deltaTime) {
    for (const auto& entity : m_entities) {
        // skip if player is in godmode
        if (entity->getState().godmode == true) continue;
        entity->move(entity->getVelocity() * deltaTime);
        glm::ivec3 entityPos = entity->getPosition();
        uint8_t blockUnderEntity = getBlockAt(entityPos.x, entityPos.y - 1, entityPos.z);

        glm::vec3 velocity = entity->getVelocity();
        // if block underneath is solid stop falling
        if (BlockRegistry::isSolid(blockUnderEntity)) {
            velocity.y = 0;
            entity->setVelocity(velocity);
            // std::cout << "camera position: " << entityPos.x << ',' << entityPos.y << ',' << entityPos.z << '\n';

            int cx = static_cast<int>(std::floor(entityPos.x / Chunk::WIDTH));
            int cz = static_cast<int>(std::floor(entityPos.z / Chunk::DEPTH));
            // std::cout << "chunk pos: " << cx << ',' << cz << '\n';

        } else if (BlockRegistry::isTransparent(blockUnderEntity)) {
            velocity.y += GRAVITY * deltaTime;
            entity->setVelocity(velocity);
        }
    }
}

// NOTE: for now render all chunks
// later only render the visible chunks
void World::renderVisibleChunks(const Camera& camera, const Shader& shader) const {
    for (const auto& [coord, chunkPtr] : m_chunks) {
        int cx = coord.x;
        int cz = coord.y;

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(cx * Chunk::WIDTH, 0.0f, cz * Chunk::DEPTH));

        shader.setModel(model);
        chunkPtr->render();
    }
}

uint8_t World::getBlockAt(const glm::vec3& worldPos) const { return getBlockAt(worldPos.x, worldPos.y, worldPos.z); }

uint8_t World::getBlockAt(int x, int y, int z) const {
    // Convert world position to chunk coordinates
    int cx = static_cast<int>(std::floor(static_cast<float>(x) / Chunk::WIDTH));
    int cz = static_cast<int>(std::floor(static_cast<float>(z) / Chunk::DEPTH));

    auto it = m_chunks.find(glm::ivec2(cx, cz));
    if (it == m_chunks.end()) return 0; // air if chunk not loaded

    // Convert to local block coordinates inside the chunk
    int lx = static_cast<int>(x) - cx * Chunk::WIDTH;
    int ly = static_cast<int>(y);
    int lz = static_cast<int>(z) - cz * Chunk::DEPTH;

    // Bounds check (y is always within 0..HEIGHT-1 if world is finite)
    if (ly < 0 || ly >= Chunk::HEIGHT) return 0;

    return it->second->getBlock(lx, ly, lz);
}

void World::genChunks() {
    // for now generate a dirt block
    // 1 for grass block
    uint8_t type = 1;

    // generate 256 chunks (16x16)
    for (int cx = 0; cx < Chunk::CHUNK_COLS; cx++) {
        for (int cz = 0; cz < Chunk::CHUNK_ROWS; cz++) {
            // chunk copies the coord so its ok to pass in a changing value
            std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(glm::ivec2(cx, cz));
            for (int x = 0; x < 16; x++) {
                for (int y = 0; y < 32; y++) {
                    for (int z = 0; z < 16; z++) {
                        if (y == 31) chunk->setBlock(x, y, z, type); // 1 for grass
                        else if (y > 15)
                            chunk->setBlock(x, y, z, 2); // 2 for dirt
                        else
                            chunk->setBlock(x, y, z, 3); // 3 for stone
                    }
                }
            }
            chunk->makeDirty();
            m_chunks[{cx, cz}] = std::move(chunk);
            // std::cout << "chunk pos: " << cx << ',' << cz << '\n';
        }
    }

    // Second pass: generate meshes (now neighbours exist)
    for (int cx = 0; cx < Chunk::CHUNK_COLS; ++cx) {
        for (int cz = 0; cz < Chunk::CHUNK_ROWS; ++cz) {
            auto& chunk = m_chunks[{cx, cz}];
            std::array<Chunk*, 4> neighbours = {
                (cx + 1 < Chunk::CHUNK_COLS) ? m_chunks[{cx + 1, cz}].get() : nullptr,
                (cx - 1 >= 0) ? m_chunks[{cx - 1, cz}].get() : nullptr,
                (cz + 1 < Chunk::CHUNK_ROWS) ? m_chunks[{cx, cz + 1}].get() : nullptr,
                (cz - 1 >= 0) ? m_chunks[{cx, cz - 1}].get() : nullptr};
            chunk->generateMesh(neighbours);
        }
    }
    std::cout << "chunk count: " << m_chunks.size() << '\n';
}
