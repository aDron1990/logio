#pragma once

#include <type_traits>
#include <cassert>

template <typename Cell> requires std::is_default_constructible_v<Cell>
class Grid
{
public:
    Grid(size_t sizeX, size_t sizeY) : 
        m_sizeX{sizeX}, 
        m_sizeY{sizeY}, 
        m_size{sizeX * sizeY}
    { 
        m_data = new Cell[m_size];
    }

    ~Grid()
    { 
        if (m_data != nullptr) delete [] m_data;
    }

    Grid(Grid&& rhs) noexcept
    {
        std::swap(m_data, rhs.m_data);
        m_sizeX = rhs.m_sizeX;
        m_sizeY = rhs.m_sizeY;
        m_size = rhs.m_size;
    }

    Grid& operator=(Grid&& rhs) noexcept
    {
        if (m_data != nullptr) delete[] m_data;
        std::swap(m_data, rhs.m_data);
        m_sizeX = rhs.m_sizeX;
        m_sizeY = rhs.m_sizeY;
        m_size = rhs.m_size;
        return *this;
    }

    Grid(const Grid&) = delete;
    Grid& operator=(const Grid&) = delete;

    const Cell& get(size_t x, size_t y) const
    {
        auto index = x + y * m_sizeY;
        assert(index < m_size);
        return m_data[index];
    }

    Cell& get(size_t x, size_t y)
    {
        auto index = x + y * m_sizeY;
        assert(index < m_size);
        return m_data[index];
    }

private:
    Cell* m_data{nullptr};
    size_t m_sizeX;
    size_t m_sizeY;
    size_t m_size;
};