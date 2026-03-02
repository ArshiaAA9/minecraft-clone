#pragma once
#include <cstdint>
#include <glm/glm.hpp>

struct BlockType {
    bool isSolid;
    bool isTransparent;
    bool canFall;
    glm::vec2 texCoordTopLeft[6]; // order: right, left, top, bottom, front, back
};

class BlockRegistry {
public:
    // Access a block type by ID (const)
    static inline const BlockType& get(uint8_t id) { return s_blocks[id]; }

    static inline bool isTransparent(uint8_t id) { return s_blocks[id].isTransparent; }

    static inline bool isSolid(uint8_t id) { return s_blocks[id].isSolid; }

    static inline bool canFall(uint8_t id) { return s_blocks[id].canFall; }

    // Initialize all block types – call once at program start
    static inline void init() {
        // Air (ID 0)
        s_blocks[0].isSolid = false;
        s_blocks[0].isTransparent = true;

        // Grass (ID 1)
        s_blocks[1].isSolid = true;
        s_blocks[1].isTransparent = false;
        s_blocks[1].texCoordTopLeft[0] = glm::vec2(0.0f, 0.5f); // right
        s_blocks[1].texCoordTopLeft[1] = glm::vec2(0.0f, 0.5f); // left
        s_blocks[1].texCoordTopLeft[2] = glm::vec2(0.5f, 0.5f); // top
        s_blocks[1].texCoordTopLeft[3] = glm::vec2(0.0f, 0.0f); // bottom
        s_blocks[1].texCoordTopLeft[4] = glm::vec2(0.0f, 0.5f); // front
        s_blocks[1].texCoordTopLeft[5] = glm::vec2(0.0f, 0.5f); // back

        // Dirt (ID 2)
        s_blocks[2].isSolid = true;
        s_blocks[2].isTransparent = false;
        for (int i = 0; i < 6; ++i) {
            s_blocks[2].texCoordTopLeft[i] = glm::vec2(0.5f, 0.0f); // all faces use dirt texture
        }

        // Stone (ID 3)
        s_blocks[3].isSolid = true;
        s_blocks[3].isTransparent = false;
        for (int i = 0; i < 6; ++i) {
            s_blocks[3].texCoordTopLeft[i] = glm::vec2(0.5f, 0.0f); // all faces use dirt texture
        }
    }

private:
    static inline std::array<BlockType, 256> s_blocks; // up to 256 types
};
