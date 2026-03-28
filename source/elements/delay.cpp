#include "elements/delay.hpp"
#include "element_data.hpp"

struct DelayData
{
    int8_t delaySignal{};
};

Delay::Delay(sf::IntRect disableSprite, sf::IntRect activeSprite, sf::IntRect delaySprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite}, m_delaySprite(delaySprite) {}

void Delay::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(element.x + x, element.y + y);
}

void Delay::onUpdateEnd(World& world, ElementData& element) noexcept
{
    if (!world.hasComponent<DelayData>(element.x, element.y))
    {
        world.addComponent(element.x, element.y, DelayData{});
    }
    auto delay = world.getComponent<DelayData>(element.x, element.y);
    if (!delay) return;

    if (element.currentSignal >= 0)
        element.currentSignal = delay->get().delaySignal;
    else
        element.currentSignal = 0;

    delay->get().delaySignal = element.nextSignal;
    element.nextSignal = 0;
}

sf::IntRect Delay::getSprite(World& world, const ElementData& element) const noexcept
{
    if (!world.hasComponent<DelayData>(element.x, element.y))
    {
        world.addComponent(element.x, element.y, DelayData{});
    }
    auto delay = world.getComponent<DelayData>(element.x, element.y);
    if (!delay) getDefaultSprite();

    if (delay->get().delaySignal > 0) return m_delaySprite;
    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Delay::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}
