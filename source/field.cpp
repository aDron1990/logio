#include "field.hpp"
#include "defines.hpp"

#include <ranges>
#include <print>

Field::Field() : m_grid{100, 100} 
{
    m_gridBakeTexture.create(m_grid.sizeX() * SPRITE_SIZE, m_grid.sizeY() * SPRITE_SIZE);
    m_gridBakeTexture.clear({0, 0, 0, 0});
    drawGridLines(m_gridBakeTexture);
}

void Field::addTo(size_t x, size_t y)
{
    auto& cell = m_grid.get(x, y);
    std::unique_lock lock{cell.data.mutex};
    m_grid.get(x, y).data.data.reset(new int{1});
}

void Field::removeFrom(size_t x, size_t y)
{
    auto& cell = m_grid.get(x, y);
    std::unique_lock lock{cell.data.mutex};
    m_grid.get(x, y).data.data.reset();
}

void Field::draw(sf::RenderTarget& renderTarget, sf::Sprite sprite)
{
    renderTarget.draw(sf::Sprite{m_gridBakeTexture.getTexture()});
    for (auto& cell : m_grid)
    {
        std::shared_lock lock{cell.data.mutex};
        if (cell.data.data == nullptr) continue;
        sprite.setPosition({(float)cell.x * SPRITE_SIZE, (float)cell.y * SPRITE_SIZE});
        renderTarget.draw(sprite);
    }
}

void Field::drawGridLines(sf::RenderTarget& renderTarget)
{
    auto size = renderTarget.getSize();
    auto rect = sf::RectangleShape{{(float)size.x, (float)size.y}};
    rect.setFillColor({150, 150, 150, 255});
    renderTarget.draw(rect);
}

std::optional<sf::Vector2i> Field::mapCoordsTpGrid(sf::Vector2f worldPos)
{
    auto in_range = [](auto val, auto min, auto max) { return min <= val && val <= max; };
    if (!in_range(worldPos.x, 0, m_grid.sizeX() * 8) || !in_range(worldPos.y, 0, m_grid.sizeY() * 8)) return std::nullopt;
    return sf::Vector2i
    {
        static_cast<int>(worldPos.x / SPRITE_SIZE), 
        static_cast<int>(worldPos.y / SPRITE_SIZE)
    };
}