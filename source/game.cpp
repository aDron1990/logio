#include "game.hpp"
#include "imgui_theme.hpp"
#include "defines.hpp"

#include <print>
#include <ranges>

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

    auto atlasFile = m_resources.open("resources/images/atlas.png");
    m_atlas.loadFromMemory(atlasFile.begin(), atlasFile.size());
    m_signalSprite = sf::Sprite{m_atlas, {0, 0, 16, 16}};
    m_elementSprites.push_back(sf::Sprite{m_atlas, {17, 0, 16, 16}});
    m_elementSprites.push_back(sf::Sprite{m_atlas, {34, 0, 16, 16}});
    

    m_field.addTo(0, 0, 0);
    m_field.addTo(1, 0, 1);
    m_field.addTo(2, 0, 0);
    m_field.addTo(3, 0, 0);
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

    auto pos = sf::Mouse::getPosition();
    auto worldPos = m_window.mapPixelToCoords(pos);
    auto gridPos = m_field.mapCoordsTpGrid(worldPos);

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
        }
    }

    if (!io.WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && gridPos)
    {
        m_field.addTo(gridPos->x, gridPos->y, m_currentId);
    }
    if (!io.WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right) && gridPos)
    {
        m_field.removeFrom(gridPos->x, gridPos->y);
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

    auto sprites = m_elementSprites;
    std::transform(sprites.begin(), sprites.end(), sprites.begin(),
        [](auto& sprite)
        {
            sprite.setScale(sprite.getScale() * 5.0f);
            return sprite;
        });
    auto size = ImVec2{};
    size.y = sf::VideoMode::getDesktopMode().height;
    size.x = 90 + 16;
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("sprites", nullptr, flags);

    for (auto sprite : sprites | std::views::enumerate)
    {
        auto cursor = ImGui::GetCursorPos();
        ImGui::PushID(std::get<0>(sprite));
        if (ImGui::InvisibleButton("spriteButton", {90, 90}))
        {
            m_currentId = std::get<0>(sprite);
        }
        ImGui::PopID();
        ImGui::SetCursorPos(cursor);
        if (m_currentId == std::get<0>(sprite))
            ImGui::DrawRectFilled({0, 0, 90, 90}, sf::Color{0xF0F0F0FF});
        ImGui::SetCursorPos({cursor.x + 5, cursor.y + 5});
        ImGui::Image(std::get<1>(sprite));
        ImGui::SetCursorPos({cursor.x, cursor.y * 2 + 90});
    }

    ImGui::End();
}

void Game::render() noexcept
{
    m_window.clear();

    auto width = static_cast<float>(m_field.sizeX() * SPRITE_SIZE);
    auto height = static_cast<float>(m_field.sizeY() * SPRITE_SIZE);
    auto fieldBackground = sf::RectangleShape{{width, height}};
    fieldBackground.setFillColor({150, 150, 150, 255});
    m_window.draw(fieldBackground);

    for (auto& cell : m_field)
    {
        std::shared_lock lock{cell.data.mutex};
        if (cell.data.data == nullptr) continue;
        auto sprite = m_elementSprites[*cell.data.data];
        sprite.setPosition({(float)cell.x * SPRITE_SIZE, (float)cell.y * SPRITE_SIZE});
        m_window.draw(sprite);
    }

    auto pos = sf::Mouse::getPosition();
    auto worldPos = m_window.mapPixelToCoords(pos);
    auto gridPos = m_field.mapCoordsTpGrid(worldPos);

    if (gridPos)
    {
        auto ghost = m_elementSprites[m_currentId];
        ghost.setPosition({(float)gridPos->x * SPRITE_SIZE, (float)gridPos->y * SPRITE_SIZE});
        ghost.setColor({255, 255, 255, 150});
        m_window.draw(ghost);
    }

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