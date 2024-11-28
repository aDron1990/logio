#pragma once

#include "field.hpp"
#include "element.hpp"
#include "ui.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>
#include <cmrc/cmrc.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <thread>
#include <atomic>
#include <vector>

CMRC_DECLARE(res);

class Game
{
public:
    Game();
    void run();

private:
    void windowProc() noexcept;
    void gameProc() noexcept;
    void updateField() noexcept;
    void updateWindow() noexcept;
    void updateCamera() noexcept;
    void render() noexcept;

private:
    std::atomic_bool m_running{true};
    std::jthread m_gameThread;
    sf::RenderWindow m_window;
    cmrc::embedded_filesystem m_resources;
    sf::Clock m_frameDeltaClock;
    sf::Time m_frameDeltaTime;
    float m_frameDelta{};
    sf::Clock m_updateDeltaClock;
    sf::Time m_updateDeltaTime;
    float m_updateDelta{};
    sf::Texture m_atlas;
    std::vector<std::unique_ptr<Element>> m_elementTypes;
    sf::Sprite m_signalSprite;
    std::atomic_int m_currentId{};
    Rotation m_currentRotation{Rotation::Up};
    const std::vector<int> m_updateTimes = {10, 25, 50, 100, 250, 500, 1000};
    std::atomic_int m_currentUpdateTimeId{0};

    Field m_field;
    UI m_ui;
};
