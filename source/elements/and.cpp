#include "elements/and.hpp"

And::And(sf::IntRect disableSprite, sf::IntRect activeSprite, sf::IntRect semiActiveSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite}, m_semiActiveSprite{semiActiveSprite} {}

void And::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal < 2) return;
    auto [x, y] = rotationToVector(elementCell.data.data->rotation);
    field.sendSignal(elementCell.x + x, elementCell.y + y);
}

sf::IntRect And::getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept
{
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal == 0) 
        return m_disableSprite;
    else if (elementCell.data.data->currentSignal == 1)
        return m_semiActiveSprite;
    return m_activeSprite;
}

sf::IntRect And::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}
