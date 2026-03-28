#include "elements/fr_splitter.hpp"

FRSplitter::FRSplitter(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void FRSplitter::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    {
        auto [x, y] = rotationToVector(element.rotation);
        world.sendSignal(element.x + x, element.y + y);
    }
    {
        auto [x, y] = rotationToVector(rotateCW(element.rotation));
        world.sendSignal(element.x + x, element.y + y);
    }
}

sf::IntRect FRSplitter::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect FRSplitter::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}