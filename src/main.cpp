#include <SFML/System.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "renderer.hpp"
#include "sfml.hpp"

#define WIN_WIDTH 1260
#define WIN_HEIGHT 720

int main() {

    std::vector<float> vertices{
        // Position:         // Colors:
        -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, // 0: bottom-left-back - red
        0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 1: bottom-right-back - green
        0.5f,  0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, // 2: top-right-back - blue
        -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f, 0.0f, // 3: top-left-back - yellow
        -0.5f, -0.5f, 0.5f,  1.0f, 0.0f, 1.0f, // 4: bottom-left-front - magenta
        0.5f,  -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, // 5: bottom-right-front - cyan
        0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.5f, // 6: top-right-front - gray
        -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, 0.0f  // 7: top-left-front - black
    };

    std::vector<unsigned int> indices = {

        // Back face
        0, 1, 2, 2, 3, 0,

        // Front face
        4, 5, 6, 6, 7, 4,

        // Top face
        3, 2, 6, 6, 7, 3,

        // Bottom face
        0, 1, 5, 5, 4, 0,

        // Right face
        1, 2, 6, 6, 5, 1,

        // Left face
        0, 3, 7, 7, 4, 0

    };

    Sfml sf{WIN_WIDTH, WIN_HEIGHT};
    Renderer renderer{};
    renderer.createShader("base", "../src/vertShader.glsl", "../src/fragShader.glsl");
    unsigned int vao;
    unsigned int vbo;
    unsigned int ebo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // unbind vao
    glBindVertexArray(0);

    glEnable(GL_DEPTH_TEST);
    while (sf.window.isOpen()) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) {
            sf.window.close();
        }

        glClearColor(0.f, 1.0f, 1.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        renderer.useProgram("base");
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        sf.window.display();
    }

    return 0;
}
