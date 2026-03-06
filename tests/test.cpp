#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <glm/glm.hpp>

// Your engine headers
#include <glad/glad.h> // For gladLoadGL()

#include <SFML/Window/Context.hpp>

#include "block-registry.hpp"
#include "camera.hpp"
#include "entity.hpp"
#include "world.hpp"

struct GLContextFixture {
    sf::Context context;

    GLContextFixture() {
        context.setActive(true);
        if (!gladLoadGL()) {
            FAIL("Failed to load GLAD in test fixture");
        }
    }
};

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

TEST_CASE("Camera - rayCast and setTargetedBlock", "[camera]") {
    GLContextFixture glFix;
    BlockRegistry::init();
    World w;
    w.genChunks();

    Camera cam;

    SECTION("Looking down at grass block from above, within reach") {
        cam.moveTo(glm::vec3(8.5f, 32.5f, 8.5f));
        cam.processMouseMovement(0.0f, -900.0f, false); // straight down
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::BLOCK);
        REQUIRE(hit.blockPos == glm::ivec3(8, 31, 8));
        REQUIRE(hit.distance == Approx(0.5f));
        REQUIRE(hit.faceNormal == glm::vec3(0.0f, 1.0f, 0.0f));
        REQUIRE(hit.hitPoint.x == Approx(8.5f));
        REQUIRE(hit.hitPoint.y == Approx(32.0f));
        REQUIRE(hit.hitPoint.z == Approx(8.5f));
    }

    SECTION("Looking down at grass block from above, beyond reach") {
        cam.moveTo(glm::vec3(8.5f, 36.5f, 8.5f));
        cam.processMouseMovement(0.0f, -900.0f, false);
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::MISS);
    }

    SECTION("Looking up from below ground through air column, should miss") {
        // Air column at x=5, y=10.5 (y=10 is all air). Next solid above is at y=16 (distance 5.5 > 4)
        cam.moveTo(glm::vec3(5.5f, 10.5f, 8.5f));
        cam.processMouseMovement(0.0f, 900.0f, false); // straight up
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::MISS);
    }

    SECTION("Horizontal ray hitting side of block, within reach") {
        // Air at x=5.5, y=15.5 (x=5, y=15 is air). Looking +X, first solid at x=6 (stone)
        cam.moveTo(glm::vec3(5.5f, 15.5f, 8.5f));
        cam.processMouseMovement(900.0f, 0.0f); // yaw = 0° (looking +X)
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::BLOCK);
        REQUIRE(hit.blockPos == glm::ivec3(6, 15, 8));
        REQUIRE(hit.distance == Approx(0.5f));
        REQUIRE(hit.faceNormal == glm::vec3(-1.0f, 0.0f, 0.0f));
        REQUIRE(hit.hitPoint.x == Approx(6.0f));
        REQUIRE(hit.hitPoint.y == Approx(15.5f));
        REQUIRE(hit.hitPoint.z == Approx(8.5f));
    }

    SECTION("Horizontal ray, beyond reach") {
        // At y=10 all blocks are air, so no solid within 4 blocks
        cam.moveTo(glm::vec3(5.5f, 10.5f, 8.5f));
        cam.processMouseMovement(900.0f, 0.0f);
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::MISS);
    }

    SECTION("Ray through vertical air gap, hitting block above") {
        cam.moveTo(glm::vec3(5.5f, 12.5f, 8.5f));
        cam.processMouseMovement(0.0f, 900.0f, false);
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::BLOCK);
        REQUIRE(hit.blockPos == glm::ivec3(5, 16, 8));
        REQUIRE(hit.distance == Approx(3.5f));                   // unchanged
        REQUIRE(hit.faceNormal == glm::vec3(0.0f, -1.0f, 0.0f)); // was (0,1,0)
        REQUIRE(hit.hitPoint.x == Approx(5.5f));
        REQUIRE(hit.hitPoint.y == Approx(16.0f));
        REQUIRE(hit.hitPoint.z == Approx(8.5f));
    }

    SECTION("Starting near boundary from below, hit block above") {
        cam.moveTo(glm::vec3(5.5f, 15.999f, 8.5f));
        cam.processMouseMovement(0.0f, 900.0f, false);
        cam.setTargetedBlock(w);
        auto hit = cam.getCrosshairTarget();
        REQUIRE(hit.type == HitTarget::BLOCK);
        REQUIRE(hit.blockPos == glm::ivec3(5, 16, 8));
        // Add a small margin for floating‑point inaccuracy
        REQUIRE(hit.distance == Approx(0.001f).margin(0.0001f));
        REQUIRE(hit.faceNormal == glm::vec3(0.0f, -1.0f, 0.0f));
        REQUIRE(hit.hitPoint.x == Approx(5.5f));
        REQUIRE(hit.hitPoint.y == Approx(16.0f));
        REQUIRE(hit.hitPoint.z == Approx(8.5f));
    }
}

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
        auto entPtr = std::make_unique<Entity>(glm::vec3(0.0f, 11.5f, 0.0f), 1.0f); // was 10.5f
        entPtr->setVelocity(glm::vec3(0.0f, 0.0f, 0.0f));
        w.addEntity(std::move(entPtr));

        w.update(1.0f);

        const auto& entities = w.getEntities();
        REQUIRE(entities.size() == 1);
        const Entity& e = *entities[0];
        REQUIRE(e.getPosition().y == Approx(11.5f));
        REQUIRE(e.getVelocity().y == Approx(-9.8f));
        REQUIRE(e.getVelocity().x == 0.0f);
        REQUIRE(e.getVelocity().z == 0.0f);
    }

    SECTION("Update with multiple entities") {
        // Both entities placed where the integer‑floor block underneath is air (y=11.5 → floor 11 → block at y=10 is
        // air)
        w.addEntity(std::make_unique<Entity>(glm::vec3(0.0f, 11.5f, 0.0f), 1.0f));
        w.addEntity(std::make_unique<Entity>(glm::vec3(10.0f, 11.5f, 0.0f), 1.0f));

        w.update(0.5f);

        REQUIRE(w.getEntities().size() == 2);
        for (const auto& ent : w.getEntities()) {
            REQUIRE(ent->getVelocity().y == Approx(-4.9f)); // -9.8 * 0.5
        }
    }
}
