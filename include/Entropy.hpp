#ifndef ENTROPY_HPP
#define ENTROPY_HPP

#include <vector>
#include <cmath>
#include "Body.hpp"
#include <cstddef> 

template <typename T>
void updateEntropyGrid(const std::vector<Body<T, 2>> &bodies, std::vector<std::vector<int>> &grid,
                       T minX, T maxX, T minY, T maxY, T invDx, T invDy)
{
    int size = grid.size();
    for (const auto &body : bodies)
    {
        auto pos = body.getPosition();
        int i = static_cast<int>((pos[0] - minX) * invDx);
        int j = static_cast<int>((pos[1] - minY) * invDy);
        if (i >= 0 && i < size && j >= 0 && j < size)
        {
            grid[i][j]++;
        }
    }
}

inline double computeNormalizedEntropy(const std::vector<std::vector<int>> &grid, std::size_t totalSamples, double k_B = 1.0)
{
    double entropy = 0.0;
    int numBins = grid.size() * grid[0].size(); // Total number of grid cells

    for (const auto &row : grid)
    {
        for (int count : row)
        {
            if (count > 0)
            {
                double p = static_cast<double>(count) / static_cast<double>(totalSamples);
                entropy -= p * std::log(p);
            }
        }
    }

    double maxEntropy = std::log(static_cast<double>(numBins));
    return (maxEntropy > 0) ? (entropy / maxEntropy) * k_B : 0.0;
}

#endif // ENTROPY_HPP
