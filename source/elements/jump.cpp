#include "jump.hpp"

Jump::Jump(sf::Sprite disableSprite, sf::Sprite activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Jump::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    if (elementCell.data.data->currentSignal == 0) return;
    auto [x, y] = rotationToVector(elementCell.data.data->rotation);
    field.sendSignal(x * 2 + elementCell.x, y * 2 + elementCell.y);
}

sf::Sprite Jump::getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal == 0) return m_disableSprite;
    return m_activeSprite;
}

sf::Sprite Jump::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}