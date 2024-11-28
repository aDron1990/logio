#pragma once

#include "element.hpp"

class Jump : public Element
{
public:
    Jump(sf::Sprite sprite);
    void onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept override;
    sf::Sprite getSprite() const noexcept override;

private:
    sf::Sprite m_sprite;
};