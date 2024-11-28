#pragma once

#include "element.hpp"

class And : public Element
{
public:
    And(sf::Sprite disableSprite, sf::Sprite activeSprite, sf::Sprite semiActiveSprite);
    void onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept override;
    sf::Sprite getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept override;
    sf::Sprite getDefaultSprite() const noexcept override;

private:
    sf::Sprite m_disableSprite;
    sf::Sprite m_activeSprite;
    sf::Sprite m_semiActiveSprite;
};