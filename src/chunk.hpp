#pragma once
#include <array>
#include <cstdint>
#include <glm/glm.hpp>
#include <vector>

#include "block-registry.hpp"
#include "mesh.hpp"

// Order: right, left, top, bottom, front, back
static const std::array<glm::ivec3, 6> faceDirections = {
    glm::ivec3(1, 0, 0),  // right
    glm::ivec3(-1, 0, 0), // left
    glm::ivec3(0, 1, 0),  // top
    glm::ivec3(0, -1, 0), // bottom
    glm::ivec3(0, 0, 1),  // front (assuming +Z is forward)
    glm::ivec3(0, 0, -1)  // back
};

class Chunk {
public:
    inline static constexpr int WIDTH = 16;
    // inline static constexpr int HEIGHT = 256;
    inline static constexpr int HEIGHT = 32;
    inline static constexpr int DEPTH = 16;
    inline static constexpr int VOLUME = WIDTH * HEIGHT * DEPTH;
    inline static constexpr int CHUNK_ROWS = 16;
    inline static constexpr int CHUNK_COLS = 16;

    Chunk(const glm::ivec2& coord)
        : m_coord(coord) {}

    void setBlock(int x, int y, int z, uint8_t blockID);
    // for easy block generation during debuging
    void setBlock(size_t index, uint8_t blockID);
    uint8_t getBlock(int x, int y, int z) const;
    const BlockType getBlockTypeAtPosition(const glm::vec3& pos) const;

    void generateMesh(const std::array<Chunk*, 4>& neighbours); // transfers generated mesh to GPU (Mesh object)

    void render() const; // binds VAO and draws

    inline bool isDirty() const { return m_isDirty; }

    inline void makeDirty() { m_isDirty = true; }

    inline const glm::ivec2& getCoord() const { return m_coord; }

private:
    // used to upload vertices to gpu inside generateMesh() function
    void uploadMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    glm::ivec2 m_coord;
    // each uint8_t corresponds to a blocktype
    std::array<uint8_t, VOLUME> m_blocks;
    Mesh m_mesh;
    bool m_isDirty = true;
};
