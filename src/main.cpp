#include <SFML/Window/Keyboard.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "sfml.hpp"

#define WIN_WIDTH 1260
#define WIN_HEIGHT 720

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastTime = 0.0f;

int main() {
    // Ground plane vertices
    std::vector<float> groundVertices = {
        // positions
        -10.0f, -1.0f, -10.0f, 0.15f, 0.55f, 0.15f, //
        10.0f,  -1.0f, -10.0f, 0.15f, 0.55f, 0.15f, //
        10.0f,  -1.0f, 10.0f,  0.15f, 0.55f, 0.15f, //
        -10.0f, -1.0f, 10.0f,  0.15f, 0.55f, 0.15f  //
    };

    std::vector<unsigned int> groundIndices = {
        0, 1, 2, // first triangle
        2, 3, 0  // second triangle
    };

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
    std::vector<Mesh> meshes;

    Mesh cube{};
    cube.setVertices(vertices);
    cube.setIndexData(indices);
    cube.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    cube.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    meshes.push_back(std::move(cube));

    Mesh ground{};
    ground.setVertices(groundVertices);
    ground.setIndexData(groundIndices);
    ground.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    ground.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    meshes.push_back(std::move(ground));

    const Shader& shader = renderer.getShader("base");
    shader.useProgram();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    shader.setMat4("model", model);

    Camera camera{};

    glEnable(GL_DEPTH_TEST);
    while (sf.window.isOpen()) {
        float currentTime = sf.clock.getElapsedTime().asSeconds();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        sf.handleEvents(deltaTime, camera);

        glClearColor(0.f, 1.0f, 1.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.useProgram();

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(45.0f), sf.aspectRatio, 0.1f, 100.0f);
        shader.setMat4("projection", projection);

        glm::mat4 view = camera.getViewMatrix();
        shader.setMat4("view", view);

        for (const auto& mesh : meshes) {
            mesh.draw();
        }

        sf.window.display();
    }
    return 0;
}
