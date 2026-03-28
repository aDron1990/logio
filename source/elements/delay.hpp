#pragma once

#include "element.hpp"
#include <SFML/Graphics/Rect.hpp>

class Delay : public Element
{
public:
    Delay(sf::IntRect disableSprite, sf::IntRect activeSprite, sf::IntRect delaySprite);
    void onUpdate(World& world, ElementData& element) noexcept override;
    void onUpdateEnd(World& world, ElementData& element) noexcept override;
    sf::IntRect getSprite(World& world, const ElementData& element) const noexcept override;
    sf::IntRect getDefaultSprite() const noexcept override;

private:
    sf::IntRect m_disableSprite;
    sf::IntRect m_activeSprite;
    sf::IntRect m_delaySprite;
};