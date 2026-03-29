#include "world.hpp"
#include "buffer.hpp"
#include "chunk.hpp"
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
    auto [cx, cy] = Chunk::toChunkCoords(x, y);
    auto chunkIt = m_chunks.find(Coord{cx, cy});
    if (chunkIt == m_chunks.end()) return std::nullopt;

    auto chunkElement = chunkIt->second.getElement(x, y);
    if (!chunkElement.element) return std::nullopt;

    return m_registry.get<ElementData>(chunkElement.element.value());
}

void World::addElement(ptrdiff_t x, ptrdiff_t y, uint8_t id, Rotation rotation) noexcept
{
    auto entity = m_registry.create();

    auto [cx, cy] = Chunk::toChunkCoords(x, y);
    auto chunkIt = m_chunks.find(Coord{cx, cy});
    if (chunkIt == m_chunks.end())
    {
        auto newChunk = m_chunks.emplace(Coord{cx, cy}, Chunk{});
        chunkIt = newChunk.first;
    }

    auto [lx, ly] = chunkIt->second.toLocalCoords(x, y);
    ptrdiff_t index = ly * CHUNK_SIZE + lx;

    removeElement(x, y);
    chunkIt->second.elements[index].element = entity;
    m_registry.emplace_or_replace<ElementData>(entity, ElementData{.rotation = rotation, .typeId = id, .currentSignal = 0, .nextSignal = 0, .x = x, .y = y});
    m_count++;
}

bool World::removeElement(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto [cx, cy] = Chunk::toChunkCoords(x, y);
    auto chunkIt = m_chunks.find(Coord{cx, cy});
    if (chunkIt == m_chunks.end()) return false;

    auto& chunkElement = chunkIt->second.getElement(x, y);
    if (!chunkElement.element) return false;

    auto entity = chunkElement.element.value();
    chunkElement.element.reset();
    m_registry.destroy(entity);
    m_count--;
    return true;
}

void World::sendSignal(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto [cx, cy] = Chunk::toChunkCoords(x, y);
    auto chunkIt = m_chunks.find(Coord{cx, cy});
    if (chunkIt == m_chunks.end()) return;

    auto chunkElement = chunkIt->second.getElement(x, y);
    if (!chunkElement.element) return;

    auto entity = chunkElement.element.value();
    auto& elementData = m_registry.get<ElementData>(entity);
    if (elementData.currentSignal >= 0) elementData.nextSignal++;
}

void World::blockSignal(ptrdiff_t x, ptrdiff_t y) noexcept
{
    auto [cx, cy] = Chunk::toChunkCoords(x, y);
    auto chunkIt = m_chunks.find(Coord{cx, cy});
    if (chunkIt == m_chunks.end()) return;

    auto chunkElement = chunkIt->second.getElement(x, y);
    if (!chunkElement.element) return;

    auto entity = chunkElement.element.value();
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
    m_chunks.clear();
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
        std::unordered_map<Coord, Chunk, CoordHash> new_chunks;

        for (auto& cellJson : json)
        {
            auto x = cellJson["coords"][0].get<ptrdiff_t>();
            auto y = cellJson["coords"][1].get<ptrdiff_t>();
            auto typeId = cellJson["type_id"].get<uint8_t>();
            auto currentSignal = cellJson["current_signal"].get<int8_t>();
            auto nextSignal = cellJson["next_signal"].get<int8_t>();
            auto rotation = static_cast<Rotation>(cellJson["rotation"].get<float>());

            auto [cx, cy] = Chunk::toChunkCoords(x, y);
            auto chunkIt = new_chunks.find({x, y});
            if (chunkIt == new_chunks.end())
            {
                auto [it, _] = new_chunks.emplace(Coord{cx, cy}, Chunk{});
                chunkIt = it;
            }

            auto entity = new_registry.create();
            new_registry.emplace_or_replace<ElementData>(entity, ElementData{.rotation = rotation, .typeId = typeId, .currentSignal = currentSignal, .nextSignal = nextSignal, .x = x, .y = y});

            auto [lx, ly] = chunkIt->second.toLocalCoords(x, y);
            ptrdiff_t index = ly * CHUNK_SIZE + lx;
            chunkIt->second.elements[index].element = entity;
            auto chunkElement = chunkIt->second.getElement(x, y);
            chunkElement.element = entity;

            m_count++;
        }

        std::swap(new_registry, m_registry);
        std::swap(new_chunks, m_chunks);
    }
    catch (...)
    {
        return false;
    }
    return true;
}

void World::copy(Buffer& buffer, sf::IntRect segment) noexcept
{
    buffer.clear();
    for (ptrdiff_t x = segment.left; x < segment.left + segment.width; x++)
    {
        for (ptrdiff_t y = segment.top; y < segment.top + segment.height; y++)
        {
            auto chunkIt = m_chunks.find({x, y});
            if (chunkIt == m_chunks.end()) continue;

            auto element = chunkIt->second.getElement(x, y);
            if (!element.element) continue;

            auto data = m_registry.get<ElementData>(element.element.value());
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