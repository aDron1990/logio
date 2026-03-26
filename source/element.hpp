#pragma once

#include "element_data.hpp"
#include "world.hpp"

#include <SFML/Graphics.hpp>

class Element
{
public:
    virtual void onUpdate(World& world, const ElementData& element) noexcept = 0;
    virtual sf::IntRect getSprite(const ElementData& element) const noexcept = 0;
    virtual sf::IntRect getDefaultSprite() const noexcept = 0;
    virtual ~Element() = default;
};