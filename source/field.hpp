#pragma once

#include "grid.hpp"
#include "element_data.hpp"

#include <SFML/Graphics.hpp>
#include <cmrc/cmrc.hpp>

#include <optional>
#include <shared_mutex>
#include <memory>

CMRC_DECLARE(res);

struct Cell
{
    std::shared_mutex mutex;
    std::unique_ptr<ElementData> data;
};

class Field
{
public:
    Field();

    size_t sizeX();
    size_t sizeY();
    void addTo(size_t x, size_t y, uint8_t id, Rotation rotation);
    void removeFrom(size_t x, size_t y);
    void sendSignal(size_t x, size_t y);
    std::optional<sf::Vector2i> mapCoordsTpGrid(sf::Vector2f worldPos);

    Grid<Cell>::Cell* begin();
    Grid<Cell>::Cell* end();

private:
    Grid<Cell> m_grid;

};