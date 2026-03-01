#include "sfml.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <iostream>

#include "camera.hpp"

void Sfml::handleEvents(float deltaTime, Camera& camera) {
    while (const std::optional event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        } else if (const auto* resized = event->getIf<sf::Event::Resized>()) {
            glViewport(0, 0, resized->size.x, resized->size.y);
        }
    }
    handleKeyboard(deltaTime, camera);
    handleMouse(deltaTime, camera);
}

void Sfml::handleKeyboard(float deltaTime, Camera& camera) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q)) window.close();
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) camera.processKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) camera.processKeyboard(Camera_Movement::LEFT, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) camera.processKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) camera.processKeyboard(Camera_Movement::RIGHT, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) camera.processKeyboard(Camera_Movement::UP, deltaTime);
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl))
        camera.processKeyboard(Camera_Movement::DOWN, deltaTime);
}

void Sfml::handleMouse(float deltaTime, Camera& camera) {
    sf::Vector2i currentMousePos = sf::Mouse::getPosition(window);
    // std::cout << "MousePos: " << currentMousePos.x << ',' << currentMousePos.y << '\n';
    // Calculate delta from center of window
    sf::Vector2i windowCenter(window.getSize().x / 2, window.getSize().y / 2);
    float xoffset = static_cast<float>(currentMousePos.x - windowCenter.x);
    float yoffset = static_cast<float>(windowCenter.y - currentMousePos.y); // inverted Y

    // process the movement
    if (xoffset != 0 || yoffset != 0) {
        camera.processMouseMovement(xoffset, yoffset);

        // set mouse to center
        sf::Mouse::setPosition(windowCenter, window);

        // Reset mouse to center to avoid hitting screen edges
        // sf::Mouse::setPosition(windowCenter, m_window);
    }
}
