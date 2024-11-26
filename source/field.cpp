#include "field.hpp"

#include <ranges>
#include <print>

Field::Field() : m_grid{4, 4}
{

}

void Field::addTo(size_t x, size_t y)
{
    m_grid.get(x, y).data.reset(new int{1});
}

void Field::draw(sf::RenderTarget& renderTarget, sf::Sprite sprite)
{
    auto cells = m_grid | std::views::filter([](const auto& cell) { return cell.data.data != nullptr; });
    for (const auto& cell : cells)
    {
        sprite.setPosition({(float)cell.x * 8, (float)cell.y * 8});
        renderTarget.draw(sprite);
    }
}