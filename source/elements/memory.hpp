#pragma once

#include "element.hpp"
#include <SFML/Graphics/Rect.hpp>

class Memory : public Element
{
public:
    Memory(sf::IntRect disableSprite, sf::IntRect activeSprite);
    void onUpdate(World& world, ElementData& element) noexcept override;
    void onUpdateEnd(World& world, ElementData& element) noexcept override;
    sf::IntRect getSprite(World& world, const ElementData& element) const noexcept override;
    sf::IntRect getDefaultSprite() const noexcept override;

private:
    sf::IntRect m_disableSprite;
    sf::IntRect m_activeSprite;
};