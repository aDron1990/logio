#pragma once

#include "grid.hpp"

#include <SFML/Graphics.hpp>
#include <cmrc/cmrc.hpp>
#include <imgui.h>
#include <imgui-SFML.h>

#include <thread>
#include <atomic>

#include <shared_mutex>
#include <memory>

CMRC_DECLARE(res);

struct Cell
{
    std::shared_mutex mutex;
    std::unique_ptr<int> data;
};

class Game
{
public:
    Game();
    void run();

private:
    void windowProc() noexcept;
    void gameProc() noexcept;
    void updateWindow() noexcept;
    void renderUI() noexcept;
    void render() noexcept;

private:
    std::atomic_bool m_running{true};
    std::jthread m_gameThread;
    sf::RenderWindow m_window;
    cmrc::embedded_filesystem m_resources;
    sf::Clock m_frameDeltaClock;

    Grid<int> m_grid;
};
