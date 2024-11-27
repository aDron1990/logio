#include "game.hpp"
#include "imgui_theme.hpp"
#include "defines.hpp"

#include "elements/wire.hpp"
#include "elements/not.hpp"

#include <print>
#include <ranges>
#include <execution>

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

    m_elementTypes.emplace_back(std::make_unique<Wire>(sf::Sprite{m_atlas, {17, 0, 16, 16}}));
    m_elementTypes.emplace_back(std::make_unique<Not>(sf::Sprite{m_atlas, {34, 0, 16, 16}}));
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
                if (event.key.scancode == sf::Keyboard::Scancode::Q) m_currentRotation = rotateCCW(m_currentRotation);
                if (event.key.scancode == sf::Keyboard::Scancode::E) m_currentRotation = rotateCW(m_currentRotation);
            }
            break;
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

    if (!io.WantCaptureMouse && sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && gridPos)
    {
        m_field.sendSignal(gridPos->x, gridPos->y);
    }
    if (!io.WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && gridPos)
    {
        m_field.addTo(gridPos->x, gridPos->y, m_currentId, m_currentRotation);
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

    auto size = ImVec2{};
    size.y = sf::VideoMode::getDesktopMode().height;
    size.x = 90 + 16;
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("sprites", nullptr, flags);

    for (auto element : m_elementTypes | std::views::enumerate)
    {
        auto cursor = ImGui::GetCursorPos();
        ImGui::PushID(std::get<0>(element));
        if (ImGui::InvisibleButton("spriteButton", {90, 90}))
        {
            m_currentId = std::get<0>(element);
        }
        ImGui::PopID();
        ImGui::SetCursorPos(cursor);
        if (m_currentId == std::get<0>(element))
            ImGui::DrawRectFilled({0, 0, 90, 90}, sf::Color{0xF0F0F0FF});
        ImGui::SetCursorPos({cursor.x + 5, cursor.y + 5});
        auto sprite = std::get<1>(element)->getSprite();
        sprite.setScale(sprite.getScale() * 5.0f);
        ImGui::Image(sprite);
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
        if (cell.data.data->currentSignal > 0)
        {
            auto sprite = m_signalSprite;
            sprite.setPosition({(float)cell.x * SPRITE_SIZE, (float)cell.y * SPRITE_SIZE});
            m_window.draw(sprite);
        }
        auto sprite = m_elementTypes[cell.data.data->typeId]->getSprite();
        sprite.setOrigin({SPRITE_SIZE / 2, SPRITE_SIZE / 2});
        sprite.setPosition({(float)cell.x * SPRITE_SIZE + SPRITE_SIZE / 2, (float)cell.y * SPRITE_SIZE + SPRITE_SIZE / 2});
        sprite.setRotation(rotationToAngle(cell.data.data->rotation));
        m_window.draw(sprite);
    }

    auto pos = sf::Mouse::getPosition();
    auto worldPos = m_window.mapPixelToCoords(pos);
    auto gridPos = m_field.mapCoordsTpGrid(worldPos);

    if (!ImGui::GetIO().WantCaptureMouse && gridPos)
    {
        auto ghost = m_elementTypes[m_currentId]->getSprite();
        ghost.setColor({255, 255, 255, 150});
        ghost.setOrigin({SPRITE_SIZE / 2, SPRITE_SIZE / 2});
        ghost.setPosition({(float)gridPos->x * SPRITE_SIZE + SPRITE_SIZE / 2, (float)gridPos->y * SPRITE_SIZE + SPRITE_SIZE / 2});
        ghost.setRotation(rotationToAngle(m_currentRotation));
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
        updateField();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Game::updateField() noexcept
{
    std::for_each(m_field.begin(), m_field.end(), [this](auto& elementData)
    {
        std::shared_lock lock{elementData.data.mutex};
        if (elementData.data.data == nullptr) return;
        auto& element = m_elementTypes[elementData.data.data->typeId];
        element->onUpdate(m_field, elementData);
    });
    std::for_each(m_field.begin(), m_field.end(),
    [this](auto& elementData)
    {
        std::unique_lock lock{elementData.data.mutex};
        if (elementData.data.data == nullptr) return;
        elementData.data.data->currentSignal = elementData.data.data->nextSignal;
        elementData.data.data->nextSignal = 0;
    });
}