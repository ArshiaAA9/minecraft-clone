#pragma once
#include <glad/glad.h>

#include <vector>

struct Mesh {
    Mesh() { glGenVertexArrays(1, &m_vao); }

    ~Mesh() {
        glDeleteVertexArrays(1, &m_vao);
        for (GLuint vbo : m_vbos) {
            glDeleteBuffers(1, &vbo);
        }
        glDeleteBuffers(1, &m_ebo);
    }

    // disable the copy constructor and copy operator to avoid double deletion
    Mesh(const Mesh&) = delete;
    Mesh& operator=(const Mesh&) = delete;

    Mesh(Mesh&& other) noexcept
        : m_vao(other.m_vao)
        , m_vbos(std::move(other.m_vbos))
        , m_ebo(other.m_ebo)
        , m_vertexCount(other.m_vertexCount)
        , m_indexCount(other.m_indexCount) {
        other.m_vao = 0;
        other.m_vbos.clear();
        other.m_ebo = 0;
    }

    Mesh& operator=(Mesh&& other) noexcept {
        if (this != &other) {
            // Clean up current resources
            glDeleteVertexArrays(1, &m_vao);
            for (GLuint vbo : m_vbos) glDeleteBuffers(1, &vbo);
            if (m_ebo) glDeleteBuffers(1, &m_ebo);

            // Transfer ownership
            m_vao = other.m_vao;
            m_vbos = std::move(other.m_vbos);
            m_ebo = other.m_ebo;
            m_vertexCount = other.m_vertexCount;
            m_indexCount = other.m_indexCount;

            other.m_vao = 0;
            other.m_vbos.clear();
            other.m_ebo = 0;
        }
        return *this;
    }

    // Bind the VAO
    inline void bind() const { glBindVertexArray(m_vao); }

    // Upload vertex data (interleaved or separate)
    template <typename T>
    inline void setVertices(const std::vector<T>& data, int bindingIndex = 0) {
        bind();
        GLuint vbo;
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
        m_vbos.push_back(vbo);
        m_vertexCount = static_cast<GLsizei>(data.size());
    }

    inline void setIndexData(const std::vector<unsigned int>& indices) {
        bind();
        if (m_ebo == 0) {
            glGenBuffers(1, &m_ebo);
        }
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
        m_indexCount = static_cast<GLsizei>(indices.size());
    }

    inline void
    setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer) {
        bind();
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalized, stride, pointer);
    }

    // Draw the mesh (using indices if available)
    inline void draw() const {
        bind();
        if (m_ebo) glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    }

private:
    GLuint m_vao;
    std::vector<GLuint> m_vbos; // each VBO can hold different attributes
    GLuint m_ebo = 0;
    GLsizei m_vertexCount = 0;
    GLsizei m_indexCount = 0;
};
