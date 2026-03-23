#pragma once

#include "element.hpp"

class Not : public Element
{
public:
    Not(sf::IntRect disableSprite, sf::IntRect activeSprite);
    void onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept override;
    sf::IntRect getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept override;
    sf::IntRect getDefaultSprite() const noexcept override;

private:
    sf::IntRect m_disableSprite;
    sf::IntRect m_activeSprite;
};