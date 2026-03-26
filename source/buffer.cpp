#include "buffer.hpp"
#include "rotation.hpp"

void Buffer::push(ElementData element) noexcept
{
    m_elements.push_back(element);
    if (!m_minX || !m_maxX)
    {
        m_minX = element.x;
        m_maxX = element.x;
    }
    else
    {
        m_minX = std::min(m_minX.value(), element.x);
        m_maxX = std::min(m_maxX.value(), element.x);
    }
    if (!m_minY || !m_maxY)
    {
        m_minY = element.x;
        m_maxY = element.x;
    }
    else
    {
        m_minY = std::min(m_minY.value(), element.y);
        m_maxY = std::min(m_maxY.value(), element.y);
    }
}

void Buffer::clear() noexcept
{
    m_elements.clear();
}

void Buffer::rotateCW() noexcept
{
    for (auto& element : m_elements)
    {
        auto x = element.x;
        auto y = element.y;
        element.x = y;
        element.y = getSize().x - 1 - x;
        element.rotation = ::rotateCW(element.rotation);
    }
}
void Buffer::rotateCCW() noexcept {}

sf::Vector2i Buffer::getSize() const noexcept
{
    return {static_cast<int>(m_maxX.value_or(0) - m_minX.value_or(0)), static_cast<int>(m_maxY.value_or(0) - m_minY.value_or(0))};
}

const std::vector<ElementData>& Buffer::getData() const noexcept
{
    return m_elements;
}