#include "elements/memory.hpp"
#include "element_data.hpp"

struct MemoryData
{
    int8_t lastSignal{};
};

Memory::Memory(sf::IntRect disableSprite, sf::IntRect activeSprite) : m_disableSprite{disableSprite}, m_activeSprite{activeSprite} {}

void Memory::onUpdate(World& world, ElementData& element) noexcept
{
    if (element.currentSignal <= 0) return;
    auto [x, y] = rotationToVector(element.rotation);
    world.sendSignal(element.x + x, element.y + y);
}

void Memory::onUpdateEnd(World& world, ElementData& element) noexcept
{
    if (!world.hasComponent<MemoryData>(element.x, element.y))
    {
        world.addComponent(element.x, element.y, MemoryData{});
    }
    auto memoryData = world.getComponent<MemoryData>(element.x, element.y);
    if (!memoryData) return;
    auto& memory = memoryData->get();

    if (memory.lastSignal < element.nextSignal) 
    {
        element.currentSignal = !element.currentSignal;
    }

    memory.lastSignal = element.nextSignal;
    element.nextSignal = 0;
}

sf::IntRect Memory::getSprite(World& world, const ElementData& element) const noexcept
{
    if (!world.hasComponent<MemoryData>(element.x, element.y))
    {
        world.addComponent(element.x, element.y, MemoryData{});
    }
    auto Memory = world.getComponent<MemoryData>(element.x, element.y);
    if (!Memory) getDefaultSprite();

    if (element.currentSignal <= 0) return m_disableSprite;
    return m_activeSprite;
}

sf::IntRect Memory::getDefaultSprite() const noexcept
{
    return m_disableSprite;
}
