#pragma once

#include "element.hpp"

#include <SFML/Graphics.hpp>
#include <cmrc/cmrc.hpp>

#include <vector>
#include <memory>
#include <atomic>

class UI
{
public:
	UI(sf::RenderWindow& window, cmrc::file fontBin);
    ~UI();
    
    void processEvent(sf::RenderWindow& window, sf::Event& event);
    void beginDraw(sf::RenderWindow& window, sf::Time deltaTime);
    void endDraw(sf::RenderWindow& window);

    void drawSidebar(std::vector<std::unique_ptr<Element>>& elementTypes, std::atomic_int& currentId);

    void drawMenu(std::atomic_bool& running);
    void commandMenu();
    bool isInMenu();

    UI(const UI&) = delete;
    UI(UI&&) = delete;
    UI& operator=(const UI&) = delete;
    UI& operator=(UI&&) = delete;

private:
    std::atomic_bool m_inMenu{false};
};