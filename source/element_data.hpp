#pragma once

#include "rotation.hpp"

#include <atomic>
#include <cstdint>

struct ElementData
{
    std::atomic<Rotation> rotation;
    std::atomic<uint8_t> typeId;
    std::atomic<uint8_t> currentSignal{};
    std::atomic<uint8_t> nextSignal{};
};
