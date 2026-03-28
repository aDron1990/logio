#pragma once

#include "element_data.hpp"
#include "world.hpp"

#include <SFML/Graphics.hpp>

class Element
{
public:
    virtual sf::IntRect getSprite(World& world, const ElementData& element) const noexcept = 0;
    virtual sf::IntRect getDefaultSprite() const noexcept = 0;
    virtual ~Element() = default;
    virtual void onUpdate(World& world, ElementData& element) noexcept = 0;
    virtual void onUpdateEnd(World& world, ElementData& element) noexcept
    {
        if (element.currentSignal >= 0)
            element.currentSignal = element.nextSignal;
        else
            element.currentSignal = 0;

        element.nextSignal = 0;
    }
};