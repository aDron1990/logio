#include "elements/wire.hpp"
#include "element_data.hpp"

Wire::Wire(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Wire::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(element.x + x, element.y + y);
}

sf::IntRect Wire::getSprite(World& world, const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Wire::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}