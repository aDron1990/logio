#include "buffer.hpp"
#include "element_data.hpp"
#include "rotation.hpp"

#include <nlohmann/json.hpp>
#include <vector>

void Buffer::push(ElementData element) noexcept
{
    m_elements.push_back(element);
}

void Buffer::push(std::vector<ElementData> elements) noexcept
{
    std::swap(elements, m_elements);
}

void Buffer::recalcBounds() noexcept
{
    for (const auto& element : m_elements)
    {
        if (!m_minX || !m_maxX)
        {
            m_minX = element.x;
            m_maxX = element.x;
        }
        else
        {
            m_minX = std::min(m_minX.value(), element.x);
            m_maxX = std::max(m_maxX.value(), element.x);
        }
        if (!m_minY || !m_maxY)
        {
            m_minY = element.y;
            m_maxY = element.y;
        }
        else
        {
            m_minY = std::min(m_minY.value(), element.y);
            m_maxY = std::max(m_maxY.value(), element.y);
        }
    }
    for (auto& element : m_elements)
    {
        element.x -= m_minX.value();
        element.y -= m_minY.value();
    }
}

void Buffer::clear() noexcept
{
    m_elements.clear();
    m_maxX.reset();
    m_maxY.reset();
    m_minX.reset();
    m_minY.reset();
}

void Buffer::rotateCCW() noexcept
{
    for (auto& element : m_elements)
    {
        auto x = element.x;
        auto y = element.y;
        element.x = y;
        element.y = getSize().x - 1 - x;
        element.rotation = ::rotateCCW(element.rotation);
    }
}
void Buffer::rotateCW() noexcept
{
    for (auto& element : m_elements)
    {
        auto x = element.x;
        auto y = element.y;
        element.x = getSize().y - 1 - y;
        element.y = x;
        element.rotation = ::rotateCW(element.rotation);
    }
}

sf::Vector2i Buffer::getSize() const noexcept
{
    return {static_cast<int>(m_maxX.value_or(0) - m_minX.value_or(0)), static_cast<int>(m_maxY.value_or(0) - m_minY.value_or(0))};
}

const std::vector<ElementData>& Buffer::getData() const noexcept
{
    return m_elements;
}

std::string Buffer::serialize() const noexcept
{
    using namespace std::literals;
    auto json = nlohmann::json{};
    for (const auto& element : m_elements)
    {
        auto cellJson = nlohmann::json{};
        cellJson["type_id"] = element.typeId;
        cellJson["current_signal"] = element.currentSignal;
        cellJson["next_signal"] = element.nextSignal;
        cellJson["rotation"] = rotationToAngle(element.rotation);
        cellJson["coords"] = nlohmann::json::array({element.x, element.y});
        json.push_back(cellJson);
    }
    return nlohmann::to_string(json);
}

bool Buffer::deserialize(const std::string& str) noexcept
{
    try
    {
        auto json = nlohmann::json::parse(str);
        if (json.empty()) return false;

        std::vector<ElementData> elements;

        for (auto& cellJson : json)
        {
            auto x = cellJson["coords"][0].get<ptrdiff_t>();
            auto y = cellJson["coords"][1].get<ptrdiff_t>();
            auto typeId = cellJson["type_id"].get<uint8_t>();
            auto currentSignal = cellJson["current_signal"].get<uint8_t>();
            auto nextSignal = cellJson["next_signal"].get<uint8_t>();
            auto rotation = static_cast<Rotation>(cellJson["rotation"].get<float>());

            elements.push_back(ElementData{.rotation = rotation, .typeId = typeId, .currentSignal = currentSignal, .nextSignal = nextSignal, .x = x, .y = y});
        }

        clear();
        std::swap(elements, m_elements);
    }
    catch (...)
    {
        return false;
    }
    return true;
}