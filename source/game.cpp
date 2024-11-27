#include "game.hpp"
#include "imgui_theme.hpp"

#include <print>

Game::Game() : 
    m_window{sf::VideoMode::getDesktopMode(), "logio", 0}, 
    m_resources{cmrc::res::get_filesystem()}
{
    m_window.setVerticalSyncEnabled(true);
    auto view = m_window.getView();
    view.setCenter({0, 0});
    view.zoom(0.1);
    m_window.setView(view);
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
        m_frameDeltaTime = m_frameDeltaClock.restart();
        m_frameDelta = ((float)m_frameDeltaTime.asMicroseconds() / 1000.0f);
        updateWindow();
        updateCamera();
        renderUI();
        render();
    }
    ImGui::SFML::Shutdown();
}

void Game::updateWindow() noexcept
{
    auto& io = ImGui::GetIO();
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
            case sf::Event::MouseWheelScrolled:
            {
                if (io.WantCaptureMouse) break;
                if (!event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) break;
                auto view = m_window.getView();
                auto zoom = std::abs(event.mouseWheelScroll.delta - 0.1f);
                view.zoom(zoom);
                m_window.setView(view);
                break;
            }
            case sf::Event::MouseButtonPressed:
            {
                if (io.WantCaptureMouse) break;
                auto pos = sf::Vector2i{event.mouseButton.x, event.mouseButton.y};
                auto worldPos = m_window.mapPixelToCoords(pos);
                auto gridPos = m_field.mapCoordsTpGrid(worldPos);
                if (!gridPos) break;
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    m_field.addTo(gridPos->x, gridPos->y);
                }
                else
                    m_field.removeFrom(gridPos->x, gridPos->y);
                break;
            }
        }
    }
    ImGui::SFML::Update(m_window, m_frameDeltaTime);
}

void Game::updateCamera() noexcept
{
    auto resolution = sf::VideoMode::getDesktopMode();
    auto viewSize = m_window.getView().getSize();

    auto zoom = (float)resolution.width / viewSize.x;
    auto speed = 2.0f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)) speed *= 1.5f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
    {
        auto view = m_window.getView();
        view.setCenter(view.getCenter() + sf::Vector2f{0.0f, -speed * m_frameDelta / zoom});
        m_window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        auto view = m_window.getView();
        view.setCenter(view.getCenter() + sf::Vector2f{0.0f, speed * m_frameDelta / zoom});
        m_window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        auto view = m_window.getView();
        view.setCenter(view.getCenter() + sf::Vector2f{-speed * m_frameDelta / zoom, 0.0f});
        m_window.setView(view);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
    {
        auto view = m_window.getView();
        view.setCenter(view.getCenter() + sf::Vector2f{speed * m_frameDelta / zoom, 0.0f});
        m_window.setView(view);
    }
}

void Game::renderUI() noexcept
{
    static auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

    auto sprite = sf::Sprite{m_atlas, {0, 0, 8, 8}};
    sprite.setScale(sprite.getScale() * 10.0f);
    auto size = ImVec2{};
    size.y = sf::VideoMode::getDesktopMode().height;
    size.x = 100;
    ImGui::SetNextWindowSize(size);

    ImGui::Begin("image", nullptr, flags);
    ImGui::Image(sprite);
    ImGui::SetNextItemWidth(sprite.getScale().x * 8);
    ImGui::SetWindowPos({0, 0});
    ImGui::End();
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
        m_updateDeltaTime = m_updateDeltaClock.restart();
        m_updateDelta = ((float)m_updateDeltaTime.asMicroseconds() / 1000.0f);
        
    }
}