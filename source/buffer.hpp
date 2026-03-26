#pragma once

#include "element_data.hpp"

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>

#include <cstddef>
#include <string>
#include <vector>

class Buffer
{
public:
    void push(ElementData element) noexcept;
    void push(std::vector<ElementData> elements) noexcept;
    void recalcBounds() noexcept;

    void clear() noexcept;
    void rotateCW() noexcept;
    void rotateCCW() noexcept;

    std::string serialize() const noexcept;
    bool deserialize(const std::string& str) noexcept;

    sf::Vector2i getSize() const noexcept;
    const std::vector<ElementData>& getData() const noexcept;

private:
    std::vector<ElementData> m_elements;
    std::optional<ptrdiff_t> m_minX;
    std::optional<ptrdiff_t> m_minY;
    std::optional<ptrdiff_t> m_maxX;
    std::optional<ptrdiff_t> m_maxY;
};