#include "chunk.hpp"

#include <array>
#include <cstdint>

#include "block-registry.hpp"
#include "glm/fwd.hpp"

void Chunk::setBlock(size_t index, uint8_t blockID) { m_blocks.at(index) = blockID; }

void Chunk::setBlock(int x, int y, int z, uint8_t blockID) { m_blocks.at((y * DEPTH + z) * WIDTH + x) = blockID; }

uint8_t Chunk::getBlock(int x, int y, int z) const { return m_blocks.at((y * DEPTH + z) * WIDTH + x); }

const BlockType Chunk::getBlockTypeAtPosition(const glm::vec3& pos) const {
    uint8_t block = m_blocks.at((pos.y * DEPTH + pos.z) * WIDTH + pos.x);
    return BlockRegistry::get(block);
}

/*
 * find the vertices that are not blocked
 * and send them to the gpu using uploadMesh() method
 *
 * check if block is on the edge of the chunk
 * if yes then use those if not then only
 * check using the adjactent blocks
 * in the chunk
 * NOTE: IMPLEMENTED WITH AI GOODLUCK DEBUGGING
 * */
void Chunk::generateMesh(const std::array<Chunk*, 4>& neighbours) {
    // preallocate the memory using reserve()
    // i cant bother to check the math for the maximum
    // possible vertices count
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    auto getBlockAt = [&](int x, int y, int z) -> uint8_t {
        // Vertical out‑of‑bounds → air
        if (y < 0 || y >= HEIGHT) return 0;

        // Horizontal checks
        if (x >= 0 && x < WIDTH && z >= 0 && z < DEPTH) {
            // Inside current chunk
            return getBlock(x, y, z);
        }

        // Outside current chunk – need to find the correct neighbour
        // neighbours order: [0] = +x, [1] = -x, [2] = +z, [3] = -z
        Chunk* neighbour = nullptr;
        int nx = x, nz = z;

        if (x < 0) {
            neighbour = neighbours[1]; // -x
            nx = x + WIDTH;            // wrap into neighbour's local coordinate
        } else if (x >= WIDTH) {
            neighbour = neighbours[0]; // +x
            nx = x - WIDTH;
        } else if (z < 0) {
            neighbour = neighbours[3]; // -z
            nz = z + DEPTH;
        } else if (z >= DEPTH) {
            neighbour = neighbours[2]; // +z
            nz = z - DEPTH;
        }

        if (neighbour) {
            return neighbour->getBlock(nx, y, nz);
        }
        return 0; // no neighbour → air
    };

    // Texture size in UV space (hard‑coded to 0.5 because the registry uses 0.5×0.5 tiles)
    constexpr float TEX_SIZE = 0.5f;
    // constexpr float TEX_SIZE = 1.0f;

    for (int x = 0; x < WIDTH; ++x) {
        for (int y = 0; y < HEIGHT; ++y) {
            for (int z = 0; z < DEPTH; ++z) {
                uint8_t blockID = getBlock(x, y, z);
                if (BlockRegistry::isTransparent(blockID)) continue;
                const BlockType& type = BlockRegistry::get(blockID);

                // check whether the block is on the edge of chunk

                // check each face for neighbouring blocks
                // check whether blocks next to the current block are transparent
                // only add the vertices next to transparent blocks to vertices
                // vector and pass to the gpu
                for (int face = 0; face < 6; face++) {
                    glm::ivec3 dir = faceDirections[face];
                    int nx = x + dir.x;
                    int ny = y + dir.y;
                    int nz = z + dir.z;

                    // if neighbouring block is transparent skip
                    uint8_t neighbourID = getBlockAt(nx, ny, nz);
                    if (!BlockRegistry::isTransparent(neighbourID)) continue;

                    // This face is visible – add its four vertices
                    glm::vec2 uvTopLeft = type.texCoordTopLeft[face];

                    // World position of the block's minimum corner
                    float wx = static_cast<float>(x);
                    float wy = static_cast<float>(y);
                    float wz = static_cast<float>(z);

                    // Define the four corners of the face.
                    // The order is: bottom‑left, bottom‑right, top‑right, top‑left
                    // (counter‑clockwise when looking from outside).
                    glm::vec3 corners[4];
                    glm::vec2 uvs[4];

                    // Set corners and UVs depending on the face
                    switch (face) {
                        case 0:                                    // right (+x)
                            corners[0] = {wx + 1, wy, wz + 1};     // bottom‑left  (far)
                            corners[1] = {wx + 1, wy, wz};         // bottom‑right (near)
                            corners[2] = {wx + 1, wy + 1, wz};     // top‑right
                            corners[3] = {wx + 1, wy + 1, wz + 1}; // top‑left
                            break;
                        case 1:                                // left (-x)
                            corners[0] = {wx, wy, wz};         // bottom‑left  (near)
                            corners[1] = {wx, wy, wz + 1};     // bottom‑right (far)
                            corners[2] = {wx, wy + 1, wz + 1}; // top‑right
                            corners[3] = {wx, wy + 1, wz};     // top‑left
                            break;
                        case 2:                                    // top (+y)
                            corners[0] = {wx, wy + 1, wz};         // bottom‑left  (near)
                            corners[1] = {wx + 1, wy + 1, wz};     // bottom‑right
                            corners[2] = {wx + 1, wy + 1, wz + 1}; // top‑right
                            corners[3] = {wx, wy + 1, wz + 1};     // top‑left
                            break;
                        case 3:                                // bottom (-y)
                            corners[0] = {wx, wy, wz + 1};     // bottom‑left  (far)
                            corners[1] = {wx + 1, wy, wz + 1}; // bottom‑right
                            corners[2] = {wx + 1, wy, wz};     // top‑right
                            corners[3] = {wx, wy, wz};         // top‑left
                            break;
                        case 4:                                    // front (+z)
                            corners[0] = {wx, wy, wz + 1};         // bottom‑left
                            corners[1] = {wx + 1, wy, wz + 1};     // bottom‑right
                            corners[2] = {wx + 1, wy + 1, wz + 1}; // top‑right
                            corners[3] = {wx, wy + 1, wz + 1};     // top‑left
                            break;
                        case 5:                                // back (-z)
                            corners[0] = {wx + 1, wy, wz};     // bottom‑left
                            corners[1] = {wx, wy, wz};         // bottom‑right
                            corners[2] = {wx, wy + 1, wz};     // top‑right
                            corners[3] = {wx + 1, wy + 1, wz}; // top‑left
                            break;
                    }

                    // UV coordinates for the four corners
                    // top‑left -> (u, v) ; top‑right -> (u+size, v) ; bottom‑right -> (u+size, v+size) ; bottom‑left ->
                    // (u, v+size)
                    uvs[0] = uvTopLeft + glm::vec2(0.0f, TEX_SIZE);     // bottom‑left
                    uvs[1] = uvTopLeft + glm::vec2(TEX_SIZE, TEX_SIZE); // bottom‑right
                    uvs[2] = uvTopLeft + glm::vec2(TEX_SIZE, 0.0f);     // top‑right
                    uvs[3] = uvTopLeft;                                 // top‑left

                    // Append vertices (each = x,y,z,u,v)
                    unsigned int startIndex = static_cast<unsigned int>(vertices.size() / 5);
                    for (int v = 0; v < 4; ++v) {
                        vertices.push_back(corners[v].x);
                        vertices.push_back(corners[v].y);
                        vertices.push_back(corners[v].z);
                        vertices.push_back(uvs[v].x);
                        vertices.push_back(uvs[v].y);
                    }

                    // Add indices for two triangles: (0,1,2) and (0,2,3)
                    indices.push_back(startIndex);
                    indices.push_back(startIndex + 1);
                    indices.push_back(startIndex + 2);
                    indices.push_back(startIndex);
                    indices.push_back(startIndex + 2);
                    indices.push_back(startIndex + 3);
                }
            }
        }
    }

    // Upload the generated mesh to the GPU
    uploadMesh(vertices, indices);
}

void Chunk::uploadMesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices) {
    Mesh newMesh{};
    newMesh.setVertices(vertices, static_cast<GLsizei>(vertices.size() / 5));
    newMesh.setIndexData(indices);
    newMesh.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    newMesh.setAttribute(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    m_mesh = std::move(newMesh);
    m_isDirty = false;
}

void Chunk::render() const { m_mesh.draw(); }
