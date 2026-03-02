
#include <SFML/Window/Keyboard.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "block-registry.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "renderer.hpp"
#include "sfml.hpp"
#include "world.hpp"

#define WIN_WIDTH 1260
#define WIN_HEIGHT 720
#define FOV 80.f

int main() {
    BlockRegistry::init();

    Sfml sf{WIN_WIDTH, WIN_HEIGHT};

    Renderer renderer{};
    renderer.createShader("base", "../src/vertShader.glsl", "../src/fragShader.glsl");

    sf::Image img;
    if (!img.loadFromFile("../assets/textures/test-block3.png")) {
        // error
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGBA, img.getSize().x, img.getSize().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, img.getPixelsPtr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Then bind it for use
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    const Shader& shader = renderer.getShader("base");
    shader.useProgram();
    shader.setInt("textureAtlas", 0); // tell shader to use texture unit 0

    World world{};
    std::unique_ptr<Camera> pCamera = std::make_unique<Camera>();
    Camera& camera = *pCamera;
    // turns Camera into Entity unique_ptr implicitly
    world.addEntity(std::move(pCamera));
    camera.moveTo(glm::vec3(50.0f, 100.0f, 50.0f));

    world.genChunks();

    glEnable(GL_DEPTH_TEST);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    while (sf.window.isOpen()) {
        sf::Time deltaTime = sf.clock.restart();

        // physics:
        world.update(deltaTime.asSeconds());

        sf.handleEvents(deltaTime.asSeconds(), camera);

        glClearColor(0.f, 1.0f, 1.0f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = camera.getViewMatrix();
        shader.setView(view);

        glm::mat4 projection;
        projection = glm::perspective(glm::radians(FOV), sf.aspectRatio, 0.1f, 300.0f);
        shader.setProjection(projection);

        world.renderVisibleChunks(camera, shader);

        sf.window.display();
    }
    return 0;
}
