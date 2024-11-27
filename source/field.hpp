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

    void draw(sf::RenderTarget& renderTarget, const std::vector<sf::Sprite>& sprites);
    void addTo(size_t x, size_t y, int id);
    void removeFrom(size_t x, size_t y);
    std::optional<sf::Vector2i> mapCoordsTpGrid(sf::Vector2f worldPos);

private:
    void drawGridLines(sf::RenderTarget& renderTarget);

private:
    Grid<Cell> m_grid;
    sf::RenderTexture m_gridBakeTexture;

};