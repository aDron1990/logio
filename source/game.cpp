#include "game.hpp"
#include "imgui_theme.hpp"

Game::Game() : 
    m_window{sf::VideoMode::getDesktopMode(), "logio", 0}, 
    m_resources{cmrc::res::get_filesystem()}
{
    m_window.setVerticalSyncEnabled(true);
    IMGUI_CHECKVERSION();
    ImGui::SFML::Init(m_window);

    auto& io = ImGui::GetIO();
    auto fontFile = m_resources.open("resources/fonts/ubuntu.ttf");
    auto fontCfg = ImFontConfig{};
    fontCfg.FontDataOwnedByAtlas = false;
    io.Fonts->Clear();
    io.Fonts->AddFontFromMemoryTTF((void*)fontFile.begin(), fontFile.size(), 16, &fontCfg);
    ImGui::SFML::UpdateFontTexture();
    setImguiTheme();

    m_field.addTo(0, 0);
    m_field.addTo(1, 0);
    m_field.addTo(2, 0);
    m_field.addTo(3, 0);

    auto atlasFile = m_resources.open("resources/images/atlas.png");
    m_atlas.loadFromMemory(atlasFile.begin(), atlasFile.size());
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
        renderUI();
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
            case sf::Event::Closed: m_running = false; break;
            case sf::Event::KeyPressed: 
            {
                if (event.key.scancode == sf::Keyboard::Scancode::Escape) m_running = false;
                break;
            }
        }
    }
    ImGui::SFML::Update(m_window, m_frameDeltaClock.restart());
}

void Game::renderUI() noexcept
{
    //ImGui::ShowDemoWindow();
}

void Game::render() noexcept
{
    m_window.clear();

    m_field.draw(m_window, sf::Sprite{m_atlas, {0, 0, 8, 8}});

    ImGui::SFML::Render(m_window);
    m_window.display();
}

void Game::gameProc() noexcept
{
    while (m_running)
    {
    }
}