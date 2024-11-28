#include "elements/tree.hpp"

Tree::Tree(sf::Sprite disableSprite, sf::Sprite activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Tree::onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal == 0) return;
    {
        auto [x, y] = rotationToVector(elementCell.data.data->rotation);
        field.sendSignal(elementCell.x + x, elementCell.y + y);
    }
    {
        auto [x, y] = rotationToVector(rotateCW(elementCell.data.data->rotation));
        field.sendSignal(elementCell.x + x, elementCell.y + y);
    }
    {
        auto [x, y] = rotationToVector(rotateCCW(elementCell.data.data->rotation));
        field.sendSignal(elementCell.x + x, elementCell.y + y);
    }
}

sf::Sprite Tree::getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept
{
    std::shared_lock lock{elementCell.data.mutex};
    assert(elementCell.data.data != nullptr);
    if (elementCell.data.data->currentSignal == 0) return m_disableSprite;
    return m_activeSprite;
}

sf::Sprite Tree::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}