#include "world.hpp"
#include "buffer.hpp"
#include "element_data.hpp"
#include "defines.hpp"

#include <cstddef>
#include <nlohmann/json.hpp>
#include <optional>
#include <zlib.h>

#include <fstream>
#include <cmath>

std::optional<ElementData> World::getElement(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto it = m_grid.find(Coord{x, y});
    if (it == m_grid.end()) return std::nullopt;

    return m_registry.get<ElementData>(it->second);
}

void World::addElement(ptrdiff_t x, ptrdiff_t y, uint8_t id, Rotation rotation) noexcept
{
    removeElement(x, y);
    auto entity = m_registry.create();
    m_registry.emplace_or_replace<ElementData>(entity, ElementData{.rotation = rotation, .typeId = id, .currentSignal = 0, .nextSignal = 0, .x = x, .y = y});
    if (m_grid.insert_or_assign(Coord{x, y}, entity).second) m_count++;
}

void World::removeElement(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto it = m_grid.find(Coord{x, y});
    if (it == m_grid.end()) return;

    auto entity = it->second;
    m_registry.destroy(entity);
    m_grid.erase(it);
    m_count--;
}

void World::sendSignal(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto it = m_grid.find(Coord{x, y});
    if (it == m_grid.end()) return;

    auto entity = it->second;
    auto& elementData = m_registry.get<ElementData>(entity);
    if (elementData.currentSignal >= 0) 
        elementData.nextSignal++;
}

void World::blockSignal(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto it = m_grid.find(Coord{x, y});
    if (it == m_grid.end()) return;

    auto entity = it->second;
    auto& elementData = m_registry.get<ElementData>(entity);
    elementData.nextSignal = -1;
}

sf::Vector2i World::mapCoordsToGrid(sf::Vector2f worldPos) const noexcept
{
    int gx = static_cast<int>(std::floor(worldPos.x / SPRITE_SIZE));
    int gy = static_cast<int>(std::floor(worldPos.y / SPRITE_SIZE));
    return {gx, gy};
}

size_t World::count() const noexcept
{
    return m_count;
}

void World::clear() noexcept
{
    m_registry.clear();
    m_grid.clear();
}

void World::save(std::filesystem::path path) noexcept
{
    using namespace std::literals;
    auto json = nlohmann::json{};
    for (auto [_, element] : getElementsView().each())
    {
        auto cellJson = nlohmann::json{};
        cellJson["type_id"] = element.typeId;
        cellJson["current_signal"] = element.currentSignal;
        cellJson["next_signal"] = element.nextSignal;
        cellJson["rotation"] = rotationToAngle(element.rotation);
        cellJson["coords"] = nlohmann::json::array({element.x, element.y});
        json.push_back(cellJson);
    }
    auto file = std::ofstream{path};
    file << nlohmann::to_string(json);
}

bool World::load(std::filesystem::path path) noexcept
{
    try
    {
        if (!std::filesystem::exists(path)) return false;
        auto file = std::ifstream{path};
        auto json = nlohmann::json::parse(file);
        if (json.empty()) return false;

        entt::registry new_registry;
        std::unordered_map<Coord, entt::entity, CoordHash> new_grid;

        for (auto& cellJson : json)
        {
            auto x = cellJson["coords"][0].get<ptrdiff_t>();
            auto y = cellJson["coords"][1].get<ptrdiff_t>();
            auto typeId = cellJson["type_id"].get<uint8_t>();
            auto currentSignal = cellJson["current_signal"].get<int8_t>();
            auto nextSignal = cellJson["next_signal"].get<int8_t>();
            auto rotation = static_cast<Rotation>(cellJson["rotation"].get<float>());

            auto entity = new_registry.create();
            new_registry.emplace_or_replace<ElementData>(entity, ElementData{.rotation = rotation, .typeId = typeId, .currentSignal = currentSignal, .nextSignal = nextSignal, .x = x, .y = y});
            new_grid.insert_or_assign(Coord{x, y}, entity);

            m_count++;
        }

        std::swap(new_registry, m_registry);
        std::swap(new_grid, m_grid);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

void World::copy(Buffer& buffer, sf::IntRect segment) const noexcept
{
    buffer.clear();
    for (ptrdiff_t x = segment.left; x < segment.left + segment.width; x++)
    {
        for (ptrdiff_t y = segment.top; y < segment.top + segment.height; y++)
        {
            auto it = m_grid.find({x, y});
            if (it == m_grid.end()) continue;

            auto data = m_registry.get<ElementData>(it->second);
            data.x -= segment.left;
            data.y -= segment.top;

            buffer.push(data);
        }
    }
    buffer.recalcBounds();
}

void World::paste(const Buffer& buffer, sf::Vector2i place) noexcept
{
    auto& elements = buffer.getData();
    for (auto& element : elements)
    {
        addElement(element.x + place.x, element.y + place.y, element.typeId, element.rotation);
    }
}

void World::clear(sf::IntRect segment) noexcept
{
    for (ptrdiff_t x = segment.left; x < segment.left + segment.width; x++)
    {
        for (ptrdiff_t y = segment.top; y < segment.top + segment.height; y++)
        {
            removeElement(x, y);
        }
    }
}