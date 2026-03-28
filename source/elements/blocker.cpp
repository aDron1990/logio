#include "elements/blocker.hpp"
#include "element_data.hpp"

Blocker::Blocker(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Blocker::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.blockSignal(element.x + x, element.y + y);
}

sf::IntRect Blocker::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Blocker::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}