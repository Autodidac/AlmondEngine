#pragma once

#include "aleEngineConfig.h"

#ifdef ALMOND_USING_OPENGL

#include <vector>
#include <cstdlib>
#include <iostream>
#include <tuple>
#include <algorithm> // for std::count_if

class SandSimulation {
public:
    SandSimulation(int width, int height)
        : width(width), height(height), grid(width* height, 0) {
        std::cout << "Initializing sand simulation\n";
    }

    // Randomize the grid with some initial sand particles
    void randomize() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                grid[y * width + x] = (rand() % 5 == 0); // Adjust density as needed
            }
        }
    }

    void setWidth(int width)
    {
        this->width = width;
        this->grid.resize(width * height);
    }
    void setHeight(int height)
    {
        this->height = height;
        this->grid.resize(width * height);
    }

    size_t getActiveParticles() const {
        return std::count_if(grid.begin(), grid.end(), [](uint8_t particle) { return particle > 0; });
    }


    bool isParticleAt(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return grid[y * width + x] > 0;
        }
        return false;
    }

    // Update the sand simulation logic
    void update() {
        std::vector<uint8_t> newGrid(width * height, 0);

        // Update particles starting from the bottom
        for (int y = height - 2; y >= 0; --y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y * width + x]) {
                    // Check if the cell below is empty
                    if (y + 1 < height && !grid[(y + 1) * width + x]) {
                        newGrid[(y + 1) * width + x] = 1; // Drop sand down
                    }
                    else if (x > 0 && y + 1 < height && !grid[(y + 1) * width + (x - 1)]) {
                        newGrid[(y + 1) * width + (x - 1)] = 1; // Diagonal left
                    }
                    else if (x + 1 < width && y + 1 < height && !grid[(y + 1) * width + (x + 1)]) {
                        newGrid[(y + 1) * width + (x + 1)] = 1; // Diagonal right
                    }
                    else {
                        newGrid[y * width + x] = 1; // Stay in place if no space to fall
                    }
                }
            }
        }

        // Ensure particles at the bottom row remain in place
        for (int x = 0; x < width; ++x) {
            if (grid[(height - 1) * width + x]) {
                newGrid[(height - 1) * width + x] = 1;
            }
        }

        grid = newGrid;
    }

    // Add a sand particle at a given position
    void addSandAt(int x, int y, uint8_t sandType = 1) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y * width + x] = sandType; // Add a sand particle
        }
#ifdef DEBUG_MODE
        std::cout << "adding sand at x: " << x << " y: " << y << "\n";
#endif

    }

    // Returns the grid for rendering
    const std::vector<uint8_t>& getGrid() const {
        return grid;
    }


private:
    int width;
    int height;
    std::vector<uint8_t> grid; // Store what particle type it is
};

#endif