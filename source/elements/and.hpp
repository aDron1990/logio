#pragma once

#include "element.hpp"

class And : public Element
{
public:
    And(sf::IntRect disableSprite, sf::IntRect activeSprite, sf::IntRect semiActiveSprite);
    void onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept override;
    sf::IntRect getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept override;
    sf::IntRect getDefaultSprite() const noexcept override;

private:
    sf::IntRect m_disableSprite;
    sf::IntRect m_activeSprite;
    sf::IntRect m_semiActiveSprite;
};