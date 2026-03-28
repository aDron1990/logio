#pragma once

#include "element.hpp"

class And : public Element
{
public:
    And(sf::IntRect disableSprite, sf::IntRect activeSprite, sf::IntRect semiActiveSprite);
    void onUpdate(World& world, ElementData& element) noexcept override;
    sf::IntRect getSprite(const ElementData& element) const noexcept override;
    sf::IntRect getDefaultSprite() const noexcept override;

private:
    sf::IntRect m_disableSprite;
    sf::IntRect m_activeSprite;
    sf::IntRect m_semiActiveSprite;
};