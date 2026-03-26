#pragma once

#include "element_data.hpp"
#include "rotation.hpp"
#include "buffer.hpp"

#include <SFML/System/Vector3.hpp>
#include <cstddef>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <SFML/Graphics.hpp>

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
    void copy(Buffer& buffer, sf::IntRect segment) const noexcept;
    void paste(const Buffer& buffer, sf::Vector2i place) noexcept;

    void addElement(ptrdiff_t x, ptrdiff_t y, uint8_t id, Rotation rotation) noexcept;
    void removeElement(ptrdiff_t x, ptrdiff_t y) noexcept;
    void sendSignal(ptrdiff_t x, ptrdiff_t y) noexcept;

    sf::Vector2i mapCoordsToGrid(sf::Vector2f worldPos) const noexcept;

    void clear() noexcept;
    void save(std::filesystem::path path) noexcept;
    bool load(std::filesystem::path path) noexcept;

private:
    entt::registry m_registry;
    std::unordered_map<Coord, entt::entity, CoordHash> m_grid;
    size_t m_count = 0;
};