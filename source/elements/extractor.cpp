#include "elements/extractor.hpp"
#include "element_data.hpp"
#include "rotation.hpp"

Extractor::Extractor(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Extractor::onUpdate(World& world, ElementData& element) noexcept
{
    {
        auto back = rotateCW(rotateCW(element.rotation));
        auto [x, y] = rotationToVector(back);
        if (auto eback = world.getElement(element.x + x, element.y + y); eback.has_value() && eback->currentSignal > 0)
        {
            world.sendSignal(element.x, element.y);
        }
    }
    {
        if (element.currentSignal <= 0) return;
        auto [x, y] = rotationToVector(element.rotation);
        world.sendSignal(element.x + x, element.y + y);
    }
}

sf::IntRect Extractor::getSprite(World& world, const ElementData& element) const noexcept
{
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Extractor::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}