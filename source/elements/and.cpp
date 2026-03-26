#include "elements/and.hpp"

And::And(sf::IntRect disableSprite, sf::IntRect activeSprite, sf::IntRect semiActiveSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite}, m_semiActiveSprite{semiActiveSprite} {}

void And::onUpdate(World& world, const ElementData& element) noexcept
{
    if (element.currentSignal < 2) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(element.x + x, element.y + y);
}

sf::IntRect And::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal == 0)
        return m_disableSprite;
    else if (element.currentSignal == 1)
        return m_semiActiveSprite;
    return m_activeSprite;
}

sf::IntRect And::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}
