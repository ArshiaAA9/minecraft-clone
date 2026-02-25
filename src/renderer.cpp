#include "renderer.hpp"

bool Renderer::createShader(const std::string& id, const std::string& vertexPath, const std::string& fragPath) {
    auto [it, inserted] = m_Shaders.try_emplace(id, vertexPath, fragPath);
    if (inserted == false) {
        std::cerr << "Duplicate ID: " << id << "\n";
        return false;
    }

    return true;
}

void Renderer::useProgram(const std::string& id) const { m_Shaders.at(id).useProgram(); }
