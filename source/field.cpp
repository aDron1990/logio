#include "field.hpp"
#include "defines.hpp"

#include <ranges>
#include <print>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <zlib.h>

Field::Field() : m_grid{250, 250} {}

void Field::addTo(size_t x, size_t y, uint8_t id, Rotation rotation)
{
    auto& cell = m_grid.get(x, y);
    std::unique_lock lock{cell.data.mutex};
    m_grid.get(x, y).data.data.reset(new ElementData{.rotation = rotation, .typeId = id});
}

void Field::removeFrom(size_t x, size_t y)
{
    auto& cell = m_grid.get(x, y);
    std::unique_lock lock{cell.data.mutex};
    m_grid.get(x, y).data.data.reset();
}

void Field::sendSignal(size_t x, size_t y) 
{
    auto& cell = m_grid.get(x, y);
    std::shared_lock lock{cell.data.mutex};
    if (m_grid.get(x, y).data.data == nullptr) return;
    m_grid.get(x, y).data.data->nextSignal++;
}

std::optional<sf::Vector2i> Field::mapCoordsTpGrid(sf::Vector2f worldPos)
{
    auto in_range = [](auto val, auto min, auto max) { return min <= val && val <= max; };
    if (!in_range(worldPos.x, 0, m_grid.sizeX() * SPRITE_SIZE) || !in_range(worldPos.y, 0, m_grid.sizeY() * SPRITE_SIZE)) return std::nullopt;
    return sf::Vector2i
    {
        static_cast<int>(worldPos.x / SPRITE_SIZE), 
        static_cast<int>(worldPos.y / SPRITE_SIZE)
    };
}

void Field::clear()
{
    m_grid = Grid<Cell>{m_grid.sizeX(), m_grid.sizeY()};
}

void Field::save(std::filesystem::path path)
{
    using namespace std::literals;
    auto json = nlohmann::json{};
    for (auto& cell : m_grid)
    {
        std::shared_lock lock{cell.data.mutex};
        if (cell.data.data == nullptr) continue;
        auto cellJson = nlohmann::json{};
        cellJson["type_id"] = cell.data.data->typeId.load();
        cellJson["current_signal"] = cell.data.data->currentSignal.load();
        cellJson["next_signal"] = cell.data.data->nextSignal.load();
        cellJson["rotation"] = rotationToAngle(cell.data.data->rotation);
        cellJson["coords"] = nlohmann::json::array({cell.x, cell.y});
        json.push_back(cellJson);
    }
    auto file = std::ofstream{path};
    file << nlohmann::to_string(json);
}

bool Field::load(std::filesystem::path path)
{
    if (!std::filesystem::exists(path)) return false;
    auto file = std::ifstream{path};
    auto json = nlohmann::json::parse(file);
    if (json.empty()) return false;
    m_grid = Grid<Cell>{m_grid.sizeX(), m_grid.sizeY()};
    for (auto& cellJson : json)
    {
        auto x = cellJson["coords"][0].get<size_t>();
        auto y = cellJson["coords"][1].get<size_t>();
        auto typeId = cellJson["type_id"].get<uint8_t>();
        auto currentSignal = cellJson["current_signal"].get<uint8_t>();
        auto nextSignal = cellJson["next_signal"].get<uint8_t>();
        auto rotation = static_cast<Rotation>(cellJson["rotation"].get<float>());
        auto& cell = m_grid.get(x, y);
        std::unique_lock lock{cell.data.mutex};
        cell.data.data.reset(new ElementData
        {
            .rotation = rotation, 
            .typeId = typeId,
            .currentSignal = currentSignal,
            .nextSignal = nextSignal
        });
    }
    return true;
}

Grid<Cell>::Cell* Field::begin()
{
    return m_grid.begin();
}

Grid<Cell>::Cell* Field::end() 
{
    return m_grid.end();
}

size_t Field::sizeX()
{
    return m_grid.sizeX();
}

size_t Field::sizeY()
{
    return m_grid.sizeY();
}