#include "jump.hpp"

Jump::Jump(sf::Sprite sprite) : m_sprite{sprite} {}

void Jump::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    if (elementCell.data.data->currentSignal == 0) return;
    auto [x, y] = rotationToVector(elementCell.data.data->rotation);
    field.sendSignal(x * 2 + elementCell.x, y * 2 + elementCell.y);
}

sf::Sprite Jump::getSprite() const noexcept
{
    return m_sprite;
}