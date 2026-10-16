// Row-owning 2D grid used for the game map arrays.
//
// It keeps ported `grid[x][y]` access working: operator[] returns a row, then
// std::vector's operator[] returns the element. Rows may be padded, because the
// map grids allocate mapWidth+1 / mapHeight+1 (heightMap +2) sizes.
#ifndef SUPPORT_GRID_H
#define SUPPORT_GRID_H

#include <cstddef>
#include <vector>

template <typename T>
class Grid {
public:
    Grid() = default;

    // Allocate rows x cols, all zero-initialised.
    void resize(int rows, int cols) {
        rows_.assign(static_cast<std::size_t>(rows),
                     std::vector<T>(static_cast<std::size_t>(cols)));
    }

    void clear() { rows_.clear(); }
    bool empty() const { return rows_.empty(); }
    int rows() const { return static_cast<int>(rows_.size()); }

    std::vector<T> &operator[](int x) {
        return rows_[static_cast<std::size_t>(x)];
    }
    const std::vector<T> &operator[](int x) const {
        return rows_[static_cast<std::size_t>(x)];
    }

    // Direct row access for the save codec.
    std::vector<std::vector<T>> &storage() { return rows_; }

private:
    std::vector<std::vector<T>> rows_;
};

#endif
