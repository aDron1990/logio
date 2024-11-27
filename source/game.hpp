#pragma once

#include "field.hpp"

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
    void updateWindow() noexcept;
    void updateCamera() noexcept;
    void renderUI() noexcept;
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
    ImTextureID m_atlasUI;
    std::vector<sf::Sprite> m_elementSprites;
    sf::Sprite m_signalSprite;
    std::atomic_int m_currentId{};

    Field m_field;
};
