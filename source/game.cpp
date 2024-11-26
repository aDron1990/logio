#include "game.hpp"

Game::Game() : 
    m_window{{1280, 720}, "logio"}, 
    m_resources{cmrc::res::get_filesystem()}
{
    m_window.setVerticalSyncEnabled(true);
    IMGUI_CHECKVERSION();
    ImGui::SFML::Init(m_window);
}

void Game::run() 
{
    m_gameThread = std::jthread{[this]() { gameProc(); }};
    windowProc();
}

void Game::windowProc() noexcept 
{
    while (m_running)
    {
        updateWindow();
        render();
    }
    ImGui::SFML::Shutdown();
}

void Game::updateWindow() noexcept
{
    auto event = sf::Event{};
    while (m_window.pollEvent(event))
    {
        ImGui::SFML::ProcessEvent(m_window, event);
        switch (event.type)
        {
            case sf::Event::Closed: m_running = false;
        }
    }
    ImGui::SFML::Update(m_window, m_frameDeltaClock.restart());
}

void Game::render() noexcept
{
    m_window.clear();



    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::gameProc() noexcept
{
    while (m_running)
    {
    }
}