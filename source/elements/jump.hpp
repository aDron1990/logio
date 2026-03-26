#pragma once

#include "element.hpp"

class Jump : public Element
{
public:
    Jump(sf::IntRect disableSprite, sf::IntRect activeSprite);
    void onUpdate(World& world, const ElementData& element) noexcept override;
    sf::IntRect getSprite(const ElementData& element) const noexcept override;
    sf::IntRect getDefaultSprite() const noexcept override;

private:
    sf::IntRect m_disableSprite;
    sf::IntRect m_activeSprite;
};