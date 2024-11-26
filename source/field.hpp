#pragma once

#include "grid.hpp"

#include <SFML/Graphics.hpp>
#include <cmrc/cmrc.hpp>

#include <shared_mutex>
#include <memory>

CMRC_DECLARE(res);

struct Cell
{
    std::shared_mutex mutex;
    std::unique_ptr<int> data;
};

class Field
{
public:
    Field();

    void draw(sf::RenderTarget& renderTarget, sf::Sprite sprite);
    void addTo(size_t x, size_t y);


private:
    Grid<Cell> m_grid;

};