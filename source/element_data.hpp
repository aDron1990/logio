#pragma once

#include <cstdint>

#include "rotation.hpp"

struct ElementData
{
    Rotation rotation;
    uint8_t typeId;
    uint8_t currentSignal{};
    uint8_t nextSignal{};
};
