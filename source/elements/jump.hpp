#pragma once

#include "element.hpp"

class Jump : public Element
{
public:
    Jump(sf::Sprite disableSprite, sf::Sprite activeSprite);
    void onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept override;
    sf::Sprite getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept override;
    sf::Sprite getDefaultSprite() const noexcept override;

private:
    sf::Sprite m_disableSprite;
    sf::Sprite m_activeSprite;
};