#pragma once

#include <type_traits>
#include <cassert>

template <typename T> requires std::is_default_constructible_v<T>
class Grid
{
public:
    struct Cell
    {
        size_t x{};
        size_t y{};
        T data{};
    };

    inline size_t coordsToIndex(size_t x, size_t y)
    { 
        return x + y * m_sizeX;
    }

public:
    Grid(size_t sizeX, size_t sizeY) : 
        m_sizeX{sizeX}, 
        m_sizeY{sizeY}, 
        m_size{sizeX * sizeY}
    { 
        m_data = new Cell[m_size];
        for (auto y = 0; y < m_sizeY; y++)
        {
            for (auto x = 0; x < m_sizeX; x++)
            {
                auto index = coordsToIndex(x, y);
                auto& cell = m_data[index];
                cell.x = x;
                cell.y = y;
            }
        }
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

    const T& get(size_t x, size_t y) const
    {
        assert(x < m_sizeX && y < m_sizeY);
        auto index = coordsToIndex(x, y);
        return m_data[index].data;
    }

    T& get(size_t x, size_t y)
    {
        assert(x < m_sizeX && y < m_sizeY);
        auto index = coordsToIndex(x, y);
        return m_data[index].data;
    }

    Cell* begin() noexcept
    { 
        return m_data;
    }

    Cell* end() noexcept 
    { 
        return m_data + m_size; 
    }

private:
    Cell* m_data{nullptr};
    size_t m_sizeX;
    size_t m_sizeY;
    size_t m_size;
};