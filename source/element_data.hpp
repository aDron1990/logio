#pragma once

#include "rotation.hpp"

#include <cstdint>

struct ElementData
{
    Rotation rotation;
    uint8_t typeId;
    int8_t currentSignal{};
    int8_t nextSignal{};
    ptrdiff_t x;
    ptrdiff_t y;
};

struct Coord
{
    ptrdiff_t x;
    ptrdiff_t y;
    bool operator==(const Coord& other) const { return x == other.x && y == other.y; }
};
