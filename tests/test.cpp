#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp> // for Approx

// Your engine headers - everything works because we link the library
#include "camera.hpp"

using namespace Catch;

TEST_CASE("Camera - Constructors", "[camera]") {
    SECTION("Default constructor") {
        Camera cam;
        REQUIRE(cam.getPosition() == glm::vec3(0.0f, 0.0f, 0.0f));
    }

    SECTION("Position + yaw/pitch constructor") {
        Camera cam(glm::vec3(10.0f, 20.0f, 30.0f), glm::vec3(0, 1, 0), -90.0f, 30.0f);
        REQUIRE(cam.getPosition() == glm::vec3(10.0f, 20.0f, 30.0f));
    }
}

TEST_CASE("Camera - Keyboard movement", "[camera]") {
    Camera cam;

    SECTION("Move forward") {
        cam.processKeyboard(FORWARD, 1.0f);            // deltaTime = 1s → velocity = 2.5
        REQUIRE(cam.getPosition().z == Approx(-2.5f)); // default front = (0,0,-1)
    }

    SECTION("Move right then left (should cancel)") {
        cam.processKeyboard(RIGHT, 1.0f);
        cam.processKeyboard(LEFT, 1.0f);
        REQUIRE(cam.getPosition() == glm::vec3(0.0f, 0.0f, 0.0f));
    }
}

TEST_CASE("Camera - Mouse look", "[camera]") {
    Camera cam;

    SECTION("Looking left should change yaw") {
        cam.processMouseMovement(-100.0f, 0.0f); // 100 pixels left
        // yaw was -90, now -100 (sensitivity = 0.1)
        // We can't read yaw directly, but we can check front vector
        glm::vec3 front = cam.getViewMatrix()[2];           // third column is -front in lookAt
        REQUIRE(front.x == Approx(-0.1736f).margin(0.01f)); // sin(-100° * 0.1) etc.
    }

    SECTION("Pitch is clamped") {
        cam.processMouseMovement(0.0f, 1000.0f); // huge up movement
        // pitch should be clamped to 89°
        glm::mat4 view = cam.getViewMatrix();
        REQUIRE(view[1][2] == Approx(-0.9998f).margin(0.001f)); // almost straight up
    }
}

TEST_CASE("Camera - Mouse scroll zoom", "[camera]") {
    Camera cam;
    cam.processMouseScroll(1.0f); // zoom in
    // We can't read m_zoom directly, but it's used in projection in main.cpp
    // For now just make sure it doesn't crash
    REQUIRE_NOTHROW(cam.processMouseScroll(-10.0f));
}

// ====================== ADD YOUR OWN TESTS BELOW ======================

// // Example for Mesh (pure construction, no GL calls)
// TEST_CASE("Mesh - Creation and data upload", "[mesh]") {
//     Mesh mesh;
//
//     std::vector<float> verts = {0, 0, 0, 1, 0, 0, 0, 1, 0};
//     mesh.setVertices(verts);
//
//     REQUIRE(true); // if it didn't crash, we're good for now
//     // Later you can expose getters or test with a real GL context
// }

// Example for Shader loading (will need a valid GL context)
TEST_CASE("Shader - Loads and compiles", "[shader]") {
    // Renderer renderer;
    // REQUIRE(renderer.createShader("test", "src/vertShader.glsl", "src/fragShader.glsl"));
}
