#pragma once

#include "field.hpp"

#include <SFML/Graphics.hpp>

class Element
{
public:
    virtual void onUpdate(Field& field, Grid<Cell>::Cell& elementCell) noexcept = 0;
    virtual sf::Sprite getSprite(Field& field, Grid<Cell>::Cell& elementCell) const noexcept = 0;
    virtual sf::Sprite getDefaultSprite() const noexcept = 0;
};