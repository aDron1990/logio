#pragma once

#include <cstddef>

constexpr auto SPRITE_SIZE = 256;

constexpr auto CHUNK_SIZE = 256;

inline ptrdiff_t floorDiv(ptrdiff_t a, ptrdiff_t b) noexcept
{
    ptrdiff_t result = a / b;
    ptrdiff_t rem = a % b;

    if ((rem != 0) && ((rem < 0) != (b < 0))) --result;

    return result;
}

inline ptrdiff_t mod(ptrdiff_t a, ptrdiff_t b) noexcept
{
    ptrdiff_t result = a % b;
    if (result < 0) result += b;
    return result;
}