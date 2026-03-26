#include "selection.hpp"
#include "defines.hpp"

void Selection::activate() noexcept
{
    m_state = State::FirstClickWait;
}

void Selection::deactivate() noexcept
{
    m_state = State::None;
}

void Selection::complite() noexcept
{
    m_state = State::Complited;
}

bool Selection::isActive() const noexcept
{
    return m_state != State::None;
}

bool Selection::isFirstClickWait() const noexcept
{
    return m_state == State::FirstClickWait;
}

bool Selection::isSecondClickWait() const noexcept
{
    return m_state == State::SecondClickWait;
}

bool Selection::isComplited() const noexcept
{
    return m_state == State::Complited;
}

void Selection::click(sf::Vector2i gridPos) noexcept
{
    switch (m_state)
    {
        case State::FirstClickWait: {
            m_firstClick = gridPos;
            m_state = State::SecondClickWait;
            break;
        }
        case State::SecondClickWait: {
            m_secondClick = gridPos;
            m_state = State::Complited;
            break;
        }
        default: break;
    }
}

sf::FloatRect Selection::getSeceltionForDraw(sf::Vector2i mousePos) const noexcept
{
    auto secondPos = m_state == State::Complited ? m_secondClick : mousePos;

    sf::FloatRect selectionRect{};
    if (secondPos.x < m_firstClick.x)
    {
        selectionRect.left = secondPos.x;
        selectionRect.width = 1.0f + (m_firstClick.x - secondPos.x);
    }
    else if (secondPos.x == m_firstClick.x)
    {
        selectionRect.left = m_firstClick.x;
        selectionRect.width = 1.0f;
    }
    else if (secondPos.x > m_firstClick.x)
    {
        selectionRect.left = m_firstClick.x;
        selectionRect.width = 1.0f + (secondPos.x - m_firstClick.x);
    }

    if (secondPos.y < m_firstClick.y)
    {
        selectionRect.top = secondPos.y;
        selectionRect.height = 1.0f + (m_firstClick.y - secondPos.y);
    }
    else if (secondPos.y == m_firstClick.y)
    {
        selectionRect.top = m_firstClick.y;
        selectionRect.height = 1.0f;
    }
    else if (secondPos.y > m_firstClick.y)
    {
        selectionRect.top = m_firstClick.y;
        selectionRect.height = 1.0f + (secondPos.y - m_firstClick.y);
    }

    selectionRect.left *= SPRITE_SIZE;
    selectionRect.top *= SPRITE_SIZE;
    selectionRect.width *= SPRITE_SIZE;
    selectionRect.height *= SPRITE_SIZE;
    return selectionRect;
}

sf::IntRect Selection::getSelectionRect() const noexcept
{
    auto minX = std::min(m_firstClick.x, m_secondClick.x);
    auto maxX = std::max(m_firstClick.x, m_secondClick.x);
    auto minY = std::min(m_firstClick.y, m_secondClick.y);
    auto maxY = std::max(m_firstClick.y, m_secondClick.y);
    return {minX, minY, maxX - minX + 1, maxY - minY + 1};
}