#pragma once

#include <cstdint>

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
        case Rotation::Up: return Rotation::Right; break;
        case Rotation::Right: return Rotation::Down; break;
        case Rotation::Down: return Rotation::Left; break;
        case Rotation::Left: return Rotation::Up; break;
        default: return Rotation::Up; break;
    }
}

constexpr Rotation rotateCCW(Rotation rotation) noexcept
{
    switch (rotation)
    {
        case Rotation::Up: return Rotation::Left; break;
        case Rotation::Right: return Rotation::Up; break;
        case Rotation::Down: return Rotation::Right; break;
        case Rotation::Left: return Rotation::Down; break;
        default: return Rotation::Up; break;
    }
}
