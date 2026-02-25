#pragma once
#include <glad/glad.h>

#include <unordered_map>

#include "shader.hpp"

class Renderer {
public:
    Renderer() {}

    ~Renderer() = default;

    bool createShader(const std::string& id, const std::string& vertexPath, const std::string& fragPath);
    void useProgram(const std::string& id) const;

    inline const std::unordered_map<std::string, Shader>& getShaders() const { return m_Shaders; }

    inline const Shader& getShader(const std::string& id) const { return m_Shaders.at(id); }

private:
    std::unordered_map<std::string, Shader> m_Shaders;
};
