#pragma once

#include "element_data.hpp"
#include "rotation.hpp"
#include "buffer.hpp"
#include "chunk.hpp"

#include <SFML/System/Vector3.hpp>
#include <cstddef>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

#include <functional>
#include <optional>
#include <unordered_map>
#include <cstdint>
#include <filesystem>

struct CoordHash
{
    size_t operator()(const Coord& c) const { return std::hash<ptrdiff_t>()(c.x) ^ (std::hash<ptrdiff_t>()(c.y) << 1); }
};

class World
{
public:
    size_t count() const noexcept;
    auto getElementsView() noexcept { return m_registry.view<ElementData>(); }
    void copy(Buffer& buffer, sf::IntRect segment) noexcept;
    void paste(const Buffer& buffer, sf::Vector2i place) noexcept;
    void clear(sf::IntRect segment) noexcept;

    template <typename T>
    bool hasComponent(ptrdiff_t x, ptrdiff_t y) const noexcept
    {
        auto [cx, cy] = Chunk::toChunkCoords(x, y);
        auto chunkIt = m_chunks.find(Coord{cx, cy});
        if (chunkIt == m_chunks.end()) return false;

        auto chunkElement = chunkIt->second.getElement(x, y);
        if (!chunkElement.element) return false;
        return m_registry.all_of<T>(chunkElement.element.value());
    }

    template <typename T>
    void addComponent(ptrdiff_t x, ptrdiff_t y, T&& component) noexcept
    {
        auto [cx, cy] = Chunk::toChunkCoords(x, y);
        auto chunkIt = m_chunks.find(Coord{cx, cy});
        if (chunkIt == m_chunks.end()) return;

        auto chunkElement = chunkIt->second.getElement(x, y);
        if (!chunkElement.element) return;
        m_registry.emplace_or_replace<T>(chunkElement.element.value());
    }

    template <typename T>
    std::optional<std::reference_wrapper<T>> getComponent(ptrdiff_t x, ptrdiff_t y) noexcept
    {
        auto [cx, cy] = Chunk::toChunkCoords(x, y);
        auto chunkIt = m_chunks.find(Coord{cx, cy});
        if (chunkIt == m_chunks.end()) return std::nullopt;

        auto chunkElement = chunkIt->second.getElement(x, y);
        if (!chunkElement.element) return std::nullopt;

        return m_registry.get<T>(chunkElement.element.value());
    }

    std::optional<ElementData> getElement(ptrdiff_t x, ptrdiff_t y) noexcept;
    void addElement(ptrdiff_t x, ptrdiff_t y, uint8_t id, Rotation rotation) noexcept;
    bool removeElement(ptrdiff_t x, ptrdiff_t y) noexcept;
    void sendSignal(ptrdiff_t x, ptrdiff_t y) noexcept;
    void blockSignal(ptrdiff_t x, ptrdiff_t y) noexcept;

    sf::Vector2i mapCoordsToGrid(sf::Vector2f worldPos) const noexcept;

    void clear() noexcept;
    void save(std::filesystem::path path) noexcept;
    bool load(std::filesystem::path path) noexcept;

private:
    entt::registry m_registry;
    // std::unordered_map<Coord, entt::entity, CoordHash> m_grid;
    std::unordered_map<Coord, Chunk, CoordHash> m_chunks;
    size_t m_count = 0;
};