#include "elements/wire.hpp"

Wire::Wire(sf::Sprite sprite) : m_sprite{sprite} {}

void Wire::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    if (elementCell.data.data->currentSignal == 0) return;
    auto [x, y] = rotationToVector(elementCell.data.data->rotation);
    field.sendSignal(elementCell.x + x, elementCell.y + y);
}

sf::Sprite Wire::getSprite() const noexcept
{
    return m_sprite;
}