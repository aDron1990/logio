#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <atomic>

class Selection
{
public:
    enum class State
    {
        None,
        FirstClickWait,
        SecondClickWait,
        Complited
    };

public:
    void complite() noexcept;
    void activate() noexcept;
    void deactivate() noexcept;
    bool isActive() const noexcept;
    bool isFirstClickWait() const noexcept;
    bool isSecondClickWait() const noexcept;
    bool isComplited() const noexcept;
    void click(sf::Vector2i gridPos) noexcept;
    sf::FloatRect getSeceltionForDraw(sf::Vector2i mousePos) const noexcept;
    sf::IntRect getSelectionRect() const noexcept;

private:
    std::atomic<State> m_state{State::None};
    sf::Vector2i m_firstClick;
    sf::Vector2i m_secondClick;
};