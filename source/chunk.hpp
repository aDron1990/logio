#pragma once

#include "defines.hpp"

#include <entt/entity/fwd.hpp>
#include <entt/entity/entity.hpp>

#include <optional>
#include <array>
#include <cstddef>

struct ChunkElement
{
    std::optional<entt::entity> element;
};

struct Chunk
{
    std::array<ChunkElement, CHUNK_SIZE * CHUNK_SIZE> elements;

    static std::pair<ptrdiff_t, ptrdiff_t> toChunkCoords(ptrdiff_t x, ptrdiff_t y) noexcept
    {
        auto chunkX = floorDiv(x, CHUNK_SIZE);
        auto chunkY = floorDiv(y, CHUNK_SIZE);
        return {chunkX, chunkY};
    }

    std::pair<ptrdiff_t, ptrdiff_t> toLocalCoords(ptrdiff_t x, ptrdiff_t y) const noexcept
    {
        ptrdiff_t localX = mod(x, CHUNK_SIZE);
        ptrdiff_t localY = mod(y, CHUNK_SIZE);
        return {localX, localY};
    }

    ChunkElement getElement(ptrdiff_t x, ptrdiff_t y) const noexcept
    {
        ptrdiff_t localX = mod(x, CHUNK_SIZE);
        ptrdiff_t localY = mod(y, CHUNK_SIZE);

        ptrdiff_t index = localY * CHUNK_SIZE + localX;
        return elements[index];
    }

    ChunkElement& getElement(ptrdiff_t x, ptrdiff_t y) noexcept
    {
        ptrdiff_t localX = mod(x, CHUNK_SIZE);
        ptrdiff_t localY = mod(y, CHUNK_SIZE);

        ptrdiff_t index = localY * CHUNK_SIZE + localX;
        return elements[index];
    }
};