#include "elements/side_splitter.hpp"

SideSplitter::SideSplitter(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void SideSplitter::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    {
        auto [x, y] = rotationToVector(rotateCCW(element.rotation));
        world.sendSignal(element.x + x, element.y + y);
    }
    {
        auto [x, y] = rotationToVector(rotateCW(element.rotation));
        world.sendSignal(element.x + x, element.y + y);
    }
}

sf::IntRect SideSplitter::getSprite(const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect SideSplitter::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}