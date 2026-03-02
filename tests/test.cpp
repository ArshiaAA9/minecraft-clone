#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <glm/glm.hpp>

// Your engine headers
#include "block-registry.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "world.hpp"

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
        cam.processKeyboard(FORWARD, 1.0f);             // deltaTime = 1s → speed = 10.5
        REQUIRE(cam.getPosition().z == Approx(-10.5f)); // default front = (0,0,-1)
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
        glm::vec3 front = cam.getViewMatrix()[2]; // third column is -front in lookAt
        REQUIRE(front.x == Approx(-0.1736f).margin(0.01f));
    }

    SECTION("Pitch is clamped") {
        cam.processMouseMovement(0.0f, 1000.0f); // huge up movement
        glm::mat4 view = cam.getViewMatrix();
        REQUIRE(view[1][2] == Approx(-0.9998f).margin(0.001f)); // almost straight up
    }
}

TEST_CASE("Camera - Mouse scroll zoom", "[camera]") {
    Camera cam;
    cam.processMouseScroll(1.0f); // zoom in
    REQUIRE_NOTHROW(cam.processMouseScroll(-10.0f));
}

// ====================== NEW TESTS BELOW ======================

TEST_CASE("BlockRegistry - Initialization and properties", "[blockregistry]") {
    BlockRegistry::init(); // Must be called before any queries

    SECTION("Air block (ID 0)") {
        REQUIRE_FALSE(BlockRegistry::isSolid(0));
        REQUIRE(BlockRegistry::isTransparent(0));
        REQUIRE_FALSE(BlockRegistry::canFall(0)); // zero-initialized static storage

        const auto& air = BlockRegistry::get(0);
        REQUIRE_FALSE(air.isSolid);
        REQUIRE(air.isTransparent);
        REQUIRE_FALSE(air.canFall);
    }

    SECTION("Grass block (ID 1)") {
        REQUIRE(BlockRegistry::isSolid(1));
        REQUIRE_FALSE(BlockRegistry::isTransparent(1));
        REQUIRE_FALSE(BlockRegistry::canFall(1));

        const auto& grass = BlockRegistry::get(1);
        REQUIRE(grass.texCoordTopLeft[0] == glm::vec2(0.0f, 0.5f)); // right
        REQUIRE(grass.texCoordTopLeft[1] == glm::vec2(0.0f, 0.5f)); // left
        REQUIRE(grass.texCoordTopLeft[2] == glm::vec2(0.5f, 0.5f)); // top
        REQUIRE(grass.texCoordTopLeft[3] == glm::vec2(0.0f, 0.0f)); // bottom
        REQUIRE(grass.texCoordTopLeft[4] == glm::vec2(0.0f, 0.5f)); // front
        REQUIRE(grass.texCoordTopLeft[5] == glm::vec2(0.0f, 0.5f)); // back
    }

    SECTION("Dirt block (ID 2)") {
        REQUIRE(BlockRegistry::isSolid(2));
        REQUIRE_FALSE(BlockRegistry::isTransparent(2));
        REQUIRE_FALSE(BlockRegistry::canFall(2));

        const auto& dirt = BlockRegistry::get(2);
        for (int i = 0; i < 6; ++i) {
            REQUIRE(dirt.texCoordTopLeft[i] == glm::vec2(0.5f, 0.0f));
        }
    }

    SECTION("Stone block (ID 3)") {
        REQUIRE(BlockRegistry::isSolid(3));
        REQUIRE_FALSE(BlockRegistry::isTransparent(3));
        REQUIRE_FALSE(BlockRegistry::canFall(3));

        const auto& stone = BlockRegistry::get(3);
        for (int i = 0; i < 6; ++i) {
            REQUIRE(stone.texCoordTopLeft[i] == glm::vec2(0.5f, 0.0f));
        }
    }
}

TEST_CASE("Entity - Basic functionality", "[entity]") {
    SECTION("Construction and getters/setters") {
        Entity e(glm::vec3(10.0f, 20.0f, 30.0f), 5.0f);

        REQUIRE(e.getPosition() == glm::vec3(10.0f, 20.0f, 30.0f));

        // velocity and state are default-constructed (zero for glm::vec3 in practice)
        e.setVelocity(glm::vec3(1.0f, -2.0f, 3.0f));
        REQUIRE(e.getVelocity() == glm::vec3(1.0f, -2.0f, 3.0f));

        const auto& state = e.getState();
        // state members are uninitialized in ctor; we don't assert them (they are not used yet)
    }

    SECTION("Movement methods") {
        Entity e(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);

        e.move(glm::vec3(5.0f, 0.0f, 0.0f));
        REQUIRE(e.getPosition() == glm::vec3(5.0f, 0.0f, 0.0f));

        e.moveTo(glm::vec3(100.0f, 200.0f, 300.0f));
        REQUIRE(e.getPosition() == glm::vec3(100.0f, 200.0f, 300.0f));

        // isValidPosition always returns true in current implementation
        e.moveTo(glm::vec3(-999.0f, -999.0f, -999.0f));
        REQUIRE(e.getPosition() == glm::vec3(-999.0f, -999.0f, -999.0f));
    }
}

TEST_CASE("World - Basic operations and physics", "[world]") {
    World w;

    SECTION("getBlockAt returns air (0) when no chunks are loaded") {
        REQUIRE(w.getBlockAt(0, 0, 0) == 0);
        REQUIRE(w.getBlockAt(glm::vec3(123.4f, 56.7f, 89.0f)) == 0);
        REQUIRE(w.getBlockAt(-10, -5, -20) == 0);
        REQUIRE(w.getBlockAt(glm::vec3(-10.0f, -5.0f, -20.0f)) == 0);
    }

    SECTION("Entity management") {
        auto ent = std::make_unique<Entity>(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f);
        w.addEntity(std::move(ent));
        REQUIRE(w.getEntities().size() == 1);
    }

    SECTION("Update applies gravity when block underneath is air (transparent)") {
        auto entPtr = std::make_unique<Entity>(glm::vec3(0.0f, 10.0f, 0.0f), 1.0f);
        entPtr->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f)); // start stationary
        w.addEntity(std::move(entPtr));

        // First update (dt = 1.0s)
        // 1. move with current vel (0) → position unchanged
        // 2. block under (y=9) is air (transparent) → velocity.y += GRAVITY * dt
        w.update(1.0f);

        const auto& entities = w.getEntities();
        REQUIRE(entities.size() == 1);
        const Entity& e = *entities[0];

        REQUIRE(e.getPosition().y == Approx(10.0f)); // position did not move yet
        REQUIRE(e.getVelocity().y == Approx(-9.8f)); // GRAVITY = -9.8
        REQUIRE(e.getVelocity().x == 0.0f);
        REQUIRE(e.getVelocity().z == 0.0f);
    }

    SECTION("Update with multiple entities") {
        w.addEntity(std::make_unique<Entity>(glm::vec3(0.0f, 5.0f, 0.0f), 1.0f));
        w.addEntity(std::make_unique<Entity>(glm::vec3(10.0f, 15.0f, 0.0f), 1.0f));

        w.update(0.5f);

        REQUIRE(w.getEntities().size() == 2);
        // Both should have negative y-velocity after update (no solid ground)
        for (const auto& ent : w.getEntities()) {
            REQUIRE(ent->getVelocity().y == Approx(-4.9f)); // -9.8 * 0.5
        }
    }
}
