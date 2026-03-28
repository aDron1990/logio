#include "elements/not.hpp"

Not::Not(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Not::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal != 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(element.x + x, element.y + y);
}

sf::IntRect Not::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Not::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}