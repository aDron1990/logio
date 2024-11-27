#include "elements/not.hpp"

Not::Not(sf::Sprite sprite) : m_sprite{sprite} {}

void Not::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    if (elementCell.data.data->currentSignal != 0) return;
    auto [x, y] = rotationToVector(elementCell.data.data->rotation);
    field.sendSignal(elementCell.x + x, elementCell.y + y);
}

sf::Sprite Not::getSprite() const noexcept
{
    return m_sprite;
}