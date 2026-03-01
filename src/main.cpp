#include <SFML/Window/Keyboard.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.hpp"
#include "entity.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "sfml.hpp"
#include "world.hpp"

#define WIN_WIDTH 1260
#define WIN_HEIGHT 720
#define FOV 60.f

// timing
float deltaTime = 0.0f; // time between current frame and last frame
float lastTime = 0.0f;

int main() {
    std::vector<float> cubeVertices{
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
    std::vector<unsigned int> cubeIndices = {

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
    const Shader& shader = renderer.getShader("base");
    shader.useProgram();

    std::vector<Mesh> meshes;

    Mesh cube{};
    cube.setVertices(cubeVertices, 6);
    cube.setIndexData(cubeIndices);
    cube.setAttribute(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    cube.setAttribute(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    meshes.push_back(std::move(cube));

    std::vector<glm::mat4> cubeModels;
    cubeModels.reserve(100 * 100 * 100); // pre-allocate for 1M
    for (int x = 0; x < 100; x++) {
        for (int y = 0; y < 100; y++) {
            for (int z = 0; z < 100; z++) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z));
                cubeModels.push_back(model);
            }
        }
    }

    meshes[0].setVertices(cubeModels, 0);
    meshes[0].bind();
    for (GLuint i = 0; i < 4; i++) {
        meshes[0].setAttribute(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glVertexAttribDivisor(2 + i, 1); // this makes it instanced
    }

    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << std::endl;
    }

    World world{};
    std::unique_ptr<Camera> pCamera = std::make_unique<Camera>();
    Camera& camera = *pCamera;
    // turns Camera into Entity unique_ptr implicitly
    world.addEntity(std::move(pCamera));
    camera.setVelocity(glm::vec3(0.0f, -1.f, 0.f));
    camera.moveTo(glm::vec3(-70.0f, 50.0f, 50.0f));

    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (sf.window.isOpen()) {
        float currentTime = sf.clock.getElapsedTime().asSeconds();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // physics:
        // world.simulate(deltaTime);

        sf.handleEvents(deltaTime, camera);

        glClearColor(0.f, 1.0f, 1.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        shader.setView(view);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(FOV), sf.aspectRatio, 0.1f, 300.0f);
        shader.setProjection(projection);

        meshes[0].drawInstanced(static_cast<GLsizei>(cubeModels.size()));

        sf.window.display();
    }
    return 0;
}
