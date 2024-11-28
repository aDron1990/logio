#include "elements/and.hpp"

And::And(sf::Sprite disableSprite, sf::Sprite activeSprite, sf::Sprite semiActiveSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite}, m_semiActiveSprite{semiActiveSprite} {}

void And::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal < 2) return;
    auto [x, y] = rotationToVector(elementCell.data.data->rotation);
    field.sendSignal(elementCell.x + x, elementCell.y + y);
}

sf::Sprite And::getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal == 0) 
        return m_disableSprite;
    else if (elementCell.data.data->currentSignal == 1)
        return m_semiActiveSprite;
    return m_activeSprite;
}

sf::Sprite And::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}
