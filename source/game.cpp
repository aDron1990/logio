#include "game.hpp"
#include "imgui_theme.hpp"
#include "defines.hpp"

#include "elements/wire.hpp"
#include "elements/jump.hpp"
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

    m_elementTypes.emplace_back(std::make_unique<Wire>(sf::Sprite{m_atlas, {16 * 1 + 1, 0, 16, 16}}, sf::Sprite{m_atlas, {16 * 1 + 1, 16 * 1 + 1, 16, 16}}));
    m_elementTypes.emplace_back(std::make_unique<Jump>(sf::Sprite{m_atlas, {16 * 2 + 2, 0, 16, 16}}, sf::Sprite{m_atlas, {16 * 2 + 2, 16 * 1 + 1, 16, 16}}));
    m_elementTypes.emplace_back(std::make_unique<Not>(sf::Sprite{m_atlas, {16 * 3 + 3, 0, 16, 16}}, sf::Sprite{m_atlas, {16 * 3 + 3, 16 * 1 + 1, 16, 16}}));
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
                if (event.key.scancode == sf::Keyboard::Scancode::Comma) m_currentUpdateTimeId = std::clamp(m_currentUpdateTimeId + 1, 0, (int)m_updateTimes.size() - 1);
                if (event.key.scancode == sf::Keyboard::Scancode::Period) m_currentUpdateTimeId = std::clamp(m_currentUpdateTimeId - 1, 0, (int)m_updateTimes.size() - 1);
                if (event.key.scancode >= sf::Keyboard::Scancode::Num1 && event.key.scancode <= sf::Keyboard::Scancode::Num9)
                {
                    auto number = event.key.scancode - sf::Keyboard::Scancode::Num1;
                    if (number >= m_elementTypes.size()) break;
                    m_currentId = number;
                }
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
    const float BUTTON_SIZE = 75.0f;
    const sf::Color ACTIVE_ELEMENT_BG{0xA0A0A0FF};

    static auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoTitleBar;

    auto size = ImVec2{};
    size.y = sf::VideoMode::getDesktopMode().height;
    size.x = BUTTON_SIZE + 16;
    ImGui::SetNextWindowSize(size);
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowBgAlpha(0.8f);
    ImGui::Begin("sprites", nullptr, flags);

    for (auto element : m_elementTypes | std::views::enumerate)
    {
        auto cursor = ImGui::GetCursorPos();
        ImGui::PushID(std::get<0>(element));
        if (ImGui::InvisibleButton("spriteButton", {BUTTON_SIZE, BUTTON_SIZE}))
        {
            m_currentId = std::get<0>(element);
        }
        ImGui::PopID();
        ImGui::SetCursorPos(cursor);
        if (m_currentId == std::get<0>(element)) ImGui::DrawRectFilled({0, 0, BUTTON_SIZE, BUTTON_SIZE}, ACTIVE_ELEMENT_BG);
        auto sprite = std::get<1>(element)->getDefaultSprite();
        auto scale = sprite.getScale();
        scale = {(float)scale.x / SPRITE_SIZE, (float)scale.y / SPRITE_SIZE};
        sprite.setScale(scale * BUTTON_SIZE);
        ImGui::Image(sprite);
        ImGui::SetCursorPos({cursor.x, cursor.y + BUTTON_SIZE + 8});
    }

    ImGui::End();

    #if 0
    ImGui::Begin("Debug");
    static float frametimes[1000] = {};
    for (auto i = 0; i < std::size(frametimes) - 1; i++)
    {
        frametimes[i] = frametimes[i + 1];
    }
    frametimes[std::size(frametimes) - 1] = m_frameDelta;

    static float updatetimes[1000] = {};
    for (auto i = 0; i < std::size(updatetimes) - 1; i++)
    {
        updatetimes[i] = updatetimes[i + 1];
    }
    updatetimes[std::size(updatetimes) - 1] = m_updateDelta;
    ImGui::PlotLines("frametime", frametimes, std::size(frametimes));
    ImGui::PlotLines("updatetime", updatetimes, std::size(updatetimes));
    ImGui::End();

    ImGui::ShowDemoWindow();
    #endif
}

void Game::render() noexcept
{
    m_window.clear({100, 100, 100, 255});

    auto width = static_cast<float>(m_field.sizeX() * SPRITE_SIZE);
    auto height = static_cast<float>(m_field.sizeY() * SPRITE_SIZE);
    auto fieldBackground = sf::RectangleShape{{width, height}};
    fieldBackground.setFillColor(sf::Color{200, 200, 200, 255});
    m_window.draw(fieldBackground);

    for (auto& cell : m_field)
    {
        std::shared_lock lock{cell.data.mutex};
        if (cell.data.data == nullptr) continue;
        auto sprite = m_elementTypes[cell.data.data->typeId]->getSprite(m_field, cell);
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
        auto ghost = m_elementTypes[m_currentId]->getDefaultSprite();
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
    using namespace std::literals;
    while (m_running)
    {
        m_updateDeltaTime = m_updateDeltaClock.restart();
        sf::Clock clock;
        clock.restart();
        updateField();
        m_updateDelta = ((float)clock.getElapsedTime().asMicroseconds() / 1000.0f);
        auto sleepTime = std::chrono::milliseconds(m_updateTimes[m_currentUpdateTimeId] - (int)m_updateDelta);
        std::this_thread::sleep_for(sleepTime);
    }
}

void Game::updateField() noexcept
{
    std::for_each(std::execution::par, m_field.begin(), m_field.end(), [this](auto& elementData)
    {
        std::shared_lock lock{elementData.data.mutex};
        if (elementData.data.data == nullptr) return;
        auto& element = m_elementTypes[elementData.data.data->typeId];
        element->onUpdate(m_field, elementData);
    });
    std::for_each(std::execution::par, m_field.begin(), m_field.end(),
    [this](auto& elementData)
    {
        std::shared_lock lock{elementData.data.mutex};
        if (elementData.data.data == nullptr) return;
        elementData.data.data->currentSignal = elementData.data.data->nextSignal.load();
        if (elementData.data.data->nextSignal > 1)
        {
            elementData.data.data->nextSignal--;
            elementData.data.data->currentSignal = 1;
        }
        else
            elementData.data.data->nextSignal = 0;
    });
}