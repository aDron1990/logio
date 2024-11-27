#include "field.hpp"
#include "defines.hpp"

#include <ranges>
#include <print>

Field::Field() : m_grid{100, 100} {}

void Field::addTo(size_t x, size_t y, int id)
{
    auto& cell = m_grid.get(x, y);
    std::unique_lock lock{cell.data.mutex};
    m_grid.get(x, y).data.data.reset(new int{id});
}

void Field::removeFrom(size_t x, size_t y)
{
    auto& cell = m_grid.get(x, y);
    std::unique_lock lock{cell.data.mutex};
    m_grid.get(x, y).data.data.reset();
}

std::optional<sf::Vector2i> Field::mapCoordsTpGrid(sf::Vector2f worldPos)
{
    auto in_range = [](auto val, auto min, auto max) { return min <= val && val <= max; };
    if (!in_range(worldPos.x, 0, m_grid.sizeX() * SPRITE_SIZE) || !in_range(worldPos.y, 0, m_grid.sizeY() * SPRITE_SIZE)) return std::nullopt;
    return sf::Vector2i
    {
        static_cast<int>(worldPos.x / SPRITE_SIZE), 
        static_cast<int>(worldPos.y / SPRITE_SIZE)
    };
}

Grid<Cell>::Cell* Field::begin()
{
    return m_grid.begin();
}

Grid<Cell>::Cell* Field::end() 
{
    return m_grid.end();
}

size_t Field::sizeX()
{
    return m_grid.sizeX();
}

size_t Field::sizeY()
{
    return m_grid.sizeY();
}