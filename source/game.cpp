#include "game.hpp"
#include "defines.hpp"

#include "element_data.hpp"
#include "elements/wire.hpp"
#include "elements/jump.hpp"
#include "elements/not.hpp"
#include "elements/and.hpp"
#include "elements/tree.hpp"
#include "elements/forward_splitter.hpp"
#include "elements/fr_splitter.hpp"
#include "elements/side_splitter.hpp"
#include "elements/extractor.hpp"
#include "elements/blocker.hpp"

#include "selection.hpp"
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/PrimitiveType.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/VertexArray.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/Clipboard.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

#include <entt/entity/fwd.hpp>
#include <fstream>
#include <sstream>
#include <mutex>
#include <vector>
#include <iostream>

Game::Game()
    : m_window{sf::VideoMode::getDesktopMode(), "logio", 0, sf::ContextSettings{}}, m_resources{cmrc::res::get_filesystem()}, m_ui{m_window, m_atlas, m_resources.open("resources/fonts/ubuntu.ttf")}
{
    m_window.setVerticalSyncEnabled(true);
    auto view = m_window.getView();
    view.setCenter({0, 0});
    m_window.setView(view);

    auto atlasFile = m_resources.open("resources/images/atlas.png");
    m_atlas.loadFromMemory(atlasFile.begin(), atlasFile.size());
    // m_atlas.generateMipmap();

    m_elementTypes.emplace_back(std::make_unique<Wire>(sf::IntRect{0, 0, 256, 256}, sf::IntRect{0, 384, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<Jump>(sf::IntRect{384, 0, 256, 256}, sf::IntRect{384, 384, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<Not>(sf::IntRect{0, 1664, 256, 256}, sf::IntRect{0, 2048, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<And>(sf::IntRect{384, 1664, 256, 256}, sf::IntRect{384, 2048, 256, 256}, sf::IntRect{768, 1664, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<Tree>(sf::IntRect{384, 896, 256, 256}, sf::IntRect{384, 1280, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<FRSplitter>(sf::IntRect{1152, 896, 256, 256}, sf::IntRect{1152, 1280, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<SideSplitter>(sf::IntRect{768, 896, 256, 256}, sf::IntRect{768, 1280, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<ForwardSplitter>(sf::IntRect{0, 896, 256, 256}, sf::IntRect{0, 1280, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<Extractor>(sf::IntRect{768, 0, 256, 256}, sf::IntRect{768, 384, 256, 256}));
    m_elementTypes.emplace_back(std::make_unique<Blocker>(sf::IntRect{1152, 0, 256, 256}, sf::IntRect{1152, 384, 256, 256}));
}

void Game::run()
{
    m_gameThread = std::jthread{[this]() { gameProc(); }};
    windowProc();
}

bool Game::loadBufferFromFile(const std::filesystem::path& path) noexcept
{
    std::ifstream file{path};
    std::stringstream ss;
    ss << file.rdbuf();
    auto fileData = ss.str();
    if (m_buffer.deserialize(fileData))
    {
        m_buffer.recalcBounds();
        m_selectType = SelectType::Copy;
        m_selection.complite();
        return true;
    }
    return false;
}

void Game::windowProc() noexcept
{
    while (m_running)
    {
        m_frameDeltaTime = m_frameDeltaClock.restart();
        m_frameDelta = ((float)m_frameDeltaTime.asMicroseconds() / 1000.0f);
        updateWindow();
        updateCamera();
        m_ui.beginDraw(m_window, m_frameDeltaTime);
        m_window.clear(sf::Color(200, 200, 200, 255));
        m_ui.drawMenu(
            m_running, [this](const std::filesystem::path& path) { m_world.save(path); }, [this](const std::filesystem::path& path) { return m_world.load(path); },
            [this](const std::filesystem::path& path) { return loadBufferFromFile(path); }, [this]() { m_world.clear(); });
        if (!m_ui.isInMenu()) m_ui.drawSidebar(m_elementTypes, m_currentId);
        render();
        m_ui.endDraw(m_window);
        m_window.display();
    }
}

void Game::updateWindow() noexcept
{
    auto& io = ImGui::GetIO();
    auto event = sf::Event{};

    auto pos = sf::Mouse::getPosition();
    auto worldPos = m_window.mapPixelToCoords(pos);
    auto gridPos = m_world.mapCoordsToGrid(worldPos);

    while (m_window.pollEvent(event))
    {
        m_ui.processEvent(m_window, event);
        switch (event.type)
        {
            case sf::Event::Closed: m_running = false; break;
            case sf::Event::KeyPressed: {
                if (event.key.scancode == sf::Keyboard::Scancode::Escape) m_ui.commandMenu();
                if (event.key.scancode == sf::Keyboard::Scancode::Q)
                {
                    if (m_selection.isComplited())
                        m_buffer.rotateCCW();
                    else
                        m_currentRotation = rotateCCW(m_currentRotation);
                }
                if (event.key.scancode == sf::Keyboard::Scancode::E)
                {
                    if (m_selection.isComplited())
                        m_buffer.rotateCW();
                    else
                        m_currentRotation = rotateCW(m_currentRotation);
                }
                if (event.key.scancode == sf::Keyboard::Scancode::Comma) m_currentUpdateTimeId = std::clamp(m_currentUpdateTimeId + 1, 0, (int)m_updateTimes.size() - 1);
                if (event.key.scancode == sf::Keyboard::Scancode::Period) m_currentUpdateTimeId = std::clamp(m_currentUpdateTimeId - 1, 0, (int)m_updateTimes.size() - 1);
                if (event.key.scancode == sf::Keyboard::Scancode::Tab)
                {
                    auto view = m_window.getView();
                    view.setCenter({0, 0});
                    m_window.setView(view);
                }
                if (event.key.scancode >= sf::Keyboard::Scancode::Num1 && event.key.scancode <= sf::Keyboard::Scancode::Num9)
                {
                    auto number = event.key.scancode - sf::Keyboard::Scancode::Num1;
                    if (number >= m_elementTypes.size()) break;
                    m_currentId = number;
                }
                if (event.key.scancode == sf::Keyboard::Scancode::C && event.key.control)
                {
                    m_selectType = SelectType::Copy;
                    m_selection.activate();
                }
                if (event.key.scancode == sf::Keyboard::Scancode::V && event.key.control)
                {
                    if (m_buffer.deserialize(sf::Clipboard::getString()))
                    {
                        m_selectType = SelectType::Copy;
                        m_selection.complite();
                    }
                    else
                    {
                        std::cout << "Failed to deser clipboard" << std::endl;
                    }
                }
                if (event.key.scancode == sf::Keyboard::Scancode::Delete || event.key.scancode == sf::Keyboard::Scancode::Backspace)
                {
                    m_selectType = SelectType::Delete;
                    m_selection.activate();
                }
                break;
            }
            case sf::Event::MouseButtonPressed: {
                if (!m_selection.isActive()) break;
                if (event.mouseButton.button == sf::Mouse::Left && !m_selection.isComplited())
                {
                    m_selection.click(gridPos);
                    if (m_selection.isComplited())
                    {
                        auto selectionRect = m_selection.getSelectionRect();
                        if (m_selectType == SelectType::Copy)
                        {
                            m_world.copy(m_buffer, selectionRect);
                            sf::Clipboard::setString(m_buffer.serialize());
                        }
                        else if (m_selectType == SelectType::Delete)
                        {
                            m_world.clear(selectionRect);
                            m_selection.deactivate();
                        }
                    }
                    m_blockRMB = true;
                }
                else if (event.mouseButton.button == sf::Mouse::Left && m_selection.isComplited())
                {
                    if (m_selectType == SelectType::Copy) m_world.paste(m_buffer, gridPos);
                    m_blockRMB = true;
                }
                break;
            }
            case sf::Event::MouseButtonReleased: {
                if (event.mouseButton.button == sf::Mouse::Left) m_blockRMB = false;
                break;
            }
            case sf::Event::MouseWheelScrolled: {
                if (io.WantCaptureMouse) break;
                if (!(event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)) break;
                auto view = m_window.getView();
                auto zoom = std::abs(event.mouseWheelScroll.delta - 0.1f);
                view.zoom(zoom);
                m_window.setView(view);
                break;
            }
            default: break;
        }
    }

    if (!m_window.hasFocus()) return;
    std::unique_lock lock{m_mutex};
    if (!io.WantCaptureMouse && sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
    {
        m_world.sendSignal(gridPos.x, gridPos.y);
    }
    if (!io.WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && !m_selection.isActive() && !m_blockRMB)
    {
        m_world.addElement(gridPos.x, gridPos.y, m_currentId, m_currentRotation);
    }
    if (!io.WantCaptureMouse && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right))
    {
        m_selection.deactivate();
        if (!m_blockRMB) m_world.removeElement(gridPos.x, gridPos.y);
    }
}

void Game::updateCamera() noexcept
{
    if (m_ui.isInMenu()) return;
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

void Game::render() noexcept
{

    sf::VertexArray quads(sf::PrimitiveType::Quads);
    {
        std::unique_lock lock{m_mutex};
        quads.resize(4 * m_world.count());
        size_t quadCount{};
        auto view = m_world.getElementsView();
        for (auto [_, element] : view.each())
        {
            auto rect = m_elementTypes[element.typeId]->getSprite(element);

            quads[quadCount * 4 + 0].position = sf::Vector2f(0.f + element.x * SPRITE_SIZE, 0.f + element.y * SPRITE_SIZE);
            quads[quadCount * 4 + 1].position = sf::Vector2f(SPRITE_SIZE + element.x * SPRITE_SIZE, 0.f + element.y * SPRITE_SIZE);
            quads[quadCount * 4 + 2].position = sf::Vector2f(SPRITE_SIZE + element.x * SPRITE_SIZE, SPRITE_SIZE + element.y * SPRITE_SIZE);
            quads[quadCount * 4 + 3].position = sf::Vector2f(0.f + element.x * SPRITE_SIZE, SPRITE_SIZE + element.y * SPRITE_SIZE);

            auto [v0, v1, v2, v3] = rotationToTexCoords(element.rotation, rect);

            quads[quadCount * 4 + 0].texCoords = v0;
            quads[quadCount * 4 + 1].texCoords = v1;
            quads[quadCount * 4 + 2].texCoords = v2;
            quads[quadCount * 4 + 3].texCoords = v3;

            quadCount++;
        }
    }

    sf::RenderStates states{&m_atlas};
    m_window.draw(quads, states);

    if (m_selection.isSecondClickWait())
    {
        auto pos = sf::Mouse::getPosition();
        auto worldPos = m_window.mapPixelToCoords(pos);
        auto gridPos = m_world.mapCoordsToGrid(worldPos);

        auto selectionRect = m_selection.getSeceltionForDraw(gridPos);

        sf::VertexArray selection{sf::PrimitiveType::Lines, 8};
        selection[0].position = sf::Vector2f{selectionRect.left, selectionRect.top};
        selection[1].position = sf::Vector2f{selectionRect.left + selectionRect.width, selectionRect.top};

        selection[2].position = sf::Vector2f{selectionRect.left + selectionRect.width, selectionRect.top};
        selection[3].position = sf::Vector2f{selectionRect.left + selectionRect.width, selectionRect.top + selectionRect.height};

        selection[4].position = sf::Vector2f{selectionRect.left + selectionRect.width, selectionRect.top + selectionRect.height};
        selection[5].position = sf::Vector2f{selectionRect.left, selectionRect.top + selectionRect.height};

        selection[6].position = sf::Vector2f{selectionRect.left, selectionRect.top + selectionRect.height};
        selection[7].position = sf::Vector2f{selectionRect.left, selectionRect.top};

        selection[0].color = sf::Color::Green;
        selection[1].color = sf::Color::Green;
        selection[2].color = sf::Color::Green;
        selection[3].color = sf::Color::Green;
        selection[4].color = sf::Color::Green;
        selection[5].color = sf::Color::Green;
        selection[6].color = sf::Color::Green;
        selection[7].color = sf::Color::Green;

        m_window.draw(selection);
    }
    else if (m_selection.isComplited())
    {
        auto pos = sf::Mouse::getPosition();
        auto worldPos = m_window.mapPixelToCoords(pos);
        auto gridPos = m_world.mapCoordsToGrid(worldPos);

        if (!ImGui::GetIO().WantCaptureMouse && m_window.hasFocus())
        {
            for (auto ghostElement : m_buffer.getData())
            {
                auto ghostRect = m_elementTypes[ghostElement.typeId]->getDefaultSprite();
                auto ghost = sf::Sprite{m_atlas, ghostRect};
                ghost.setColor({255, 255, 255, 150});
                ghost.setOrigin({static_cast<float>(SPRITE_SIZE) / 2.0f, static_cast<float>(SPRITE_SIZE) / 2.0f});
                ghost.setPosition({(float)(gridPos.x + ghostElement.x) * SPRITE_SIZE + (float)SPRITE_SIZE / 2, (float)(gridPos.y + ghostElement.y) * SPRITE_SIZE + (float)SPRITE_SIZE / 2});
                ghost.setRotation(rotationToAngle(ghostElement.rotation));
                m_window.draw(ghost);
            }
        }
    }
    else if (!m_selection.isFirstClickWait())
    {
        auto pos = sf::Mouse::getPosition();
        auto worldPos = m_window.mapPixelToCoords(pos);
        auto gridPos = m_world.mapCoordsToGrid(worldPos);

        if (!ImGui::GetIO().WantCaptureMouse && m_window.hasFocus())
        {
            auto ghostRect = m_elementTypes[m_currentId]->getDefaultSprite();
            auto ghost = sf::Sprite{m_atlas, ghostRect};
            ghost.setColor({255, 255, 255, 150});
            ghost.setOrigin({static_cast<float>(SPRITE_SIZE) / 2.0f, static_cast<float>(SPRITE_SIZE) / 2.0f});
            ghost.setPosition({(float)gridPos.x * SPRITE_SIZE + (float)SPRITE_SIZE / 2, (float)gridPos.y * SPRITE_SIZE + (float)SPRITE_SIZE / 2});
            ghost.setRotation(rotationToAngle(m_currentRotation));
            m_window.draw(ghost);
        }
    }
}

void Game::gameProc() noexcept
{
    using namespace std::literals;
    while (m_running)
    {
        m_updateDeltaTime = m_updateDeltaClock.restart();
        sf::Clock clock;
        clock.restart();
        if (!m_ui.isInMenu()) updateField();
        m_updateDelta = ((float)clock.getElapsedTime().asMicroseconds() / 1000.0f);
        auto sleepTime = std::chrono::milliseconds(m_updateTimes[m_currentUpdateTimeId] - (int)m_updateDelta);
        std::this_thread::sleep_for(sleepTime);
    }
}

void Game::updateField() noexcept
{
    std::unique_lock lock{m_mutex};
    auto elements = m_world.getElementsView();
    for (auto [_, elementData] : elements.each())
    {
        auto& element = m_elementTypes[elementData.typeId];
        element->onUpdate(m_world, elementData);
    }

    for (auto [_, elementData] : elements.each())
    {
        if (elementData.currentSignal >= 0)
            elementData.currentSignal = elementData.nextSignal;
        else
            elementData.currentSignal = 0;

        elementData.nextSignal = 0;
    }
}
