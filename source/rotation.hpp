#pragma once

#include <cstdint>
#include <utility>

enum class Rotation : uint16_t
{
    Up = 0,
    Right = 90,
    Down = 180,
    Left = 270
};

constexpr float rotationToAngle(Rotation rotation) noexcept
{
    return static_cast<float>(rotation);
}

constexpr Rotation rotateCW(Rotation rotation) noexcept
{
    switch (rotation)
    {
        case Rotation::Up: return Rotation::Right;
        case Rotation::Right: return Rotation::Down;
        case Rotation::Down: return Rotation::Left;
        case Rotation::Left: return Rotation::Up;
        default: return Rotation::Up;
    }
}

constexpr Rotation rotateCCW(Rotation rotation) noexcept
{
    switch (rotation)
    {
        case Rotation::Up: return Rotation::Left;
        case Rotation::Right: return Rotation::Up;
        case Rotation::Down: return Rotation::Right;
        case Rotation::Left: return Rotation::Down;
        default: return Rotation::Up;
    }
}

constexpr std::pair<int, int> rotationToVector(Rotation rotation) noexcept 
{
    switch (rotation)
    {
        case Rotation::Up: return std::make_pair(0, -1);
        case Rotation::Right: return std::make_pair(1, 0);
        case Rotation::Down: return std::make_pair(0, 1);
        case Rotation::Left: return std::make_pair(-1, 0);
        default: return std::make_pair(0, 0);
    }
}
