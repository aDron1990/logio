#include "jump.hpp"

Jump::Jump(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Jump::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(x * 2 + element.x, y * 2 + element.y);
}

sf::IntRect Jump::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Jump::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}