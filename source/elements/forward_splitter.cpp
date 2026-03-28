#include "elements/forward_splitter.hpp"

ForwardSplitter::ForwardSplitter(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void ForwardSplitter::onUpdate(World& world, const ElementData& element) noexcept
{
    if (element.currentSignal == 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(element.x + x, element.y + y);
    world.sendSignal(x * 2 + element.x, y * 2 + element.y);
}

sf::IntRect ForwardSplitter::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal == 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect ForwardSplitter::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}