#pragma once

#include "rotation.hpp"

#include <atomic>
#include <cstdint>

struct ElementData
{
    Rotation rotation;
    uint8_t typeId;
    uint8_t currentSignal{};
    uint8_t nextSignal{};
};
