#pragma once
#include <SunnyLog/SunnyLog.h>
#include <glad/glad.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Window.hpp>

class Sfml {

public:
    sf::Window window;
    float aspectRatio;

    Sfml(uint winWidth, uint winHeight)
        : aspectRatio(static_cast<float>(winWidth) / winHeight) {
        m_settings.antiAliasingLevel = 8;
        m_settings.majorVersion = 4;
        m_settings.minorVersion = 3;
        m_settings.attributeFlags = sf::ContextSettings::Core;

        window = sf::Window(sf::VideoMode({winWidth, winHeight}), "raytracing", sf::State::Windowed, m_settings);

        window.setVerticalSyncEnabled(true);
        window.setSize(sf::Vector2u(winWidth, winHeight));
        window.setPosition(sf::Vector2i(winWidth / 2, 0));
        window.setMinimumSize(sf::Vector2u(400, 400));
        bool isWindowActive = window.setActive(true);
        window.setFramerateLimit(120); // Call this once after creating your window

        // glad must be loaded before opengl
        if (!gladLoadGL()) {
            SunnyLog::log("Error during gladLoadGl");
        }
    }

    void onMouseClick();

private:
    sf::ContextSettings m_settings;
};
