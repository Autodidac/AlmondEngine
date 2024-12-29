#pragma once

#include "aleEngineConfig.h"

#ifdef ALMOND_USING_SDL

// Define a sand simulation class
class SandSimulation {
public:
    SandSimulation(int width, int height)
        : width(width), height(height), grid(width* height, false) {
    }

    void randomize() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                // Add sand particles at random positions
                grid[y * width + x] = (rand() % 5 == 0); // Adjust density as needed
            }
        }
    }

    void update() {
        std::vector<bool> newGrid(width * height, false);

        for (int y = height - 2; y >= 0; --y) { // Start from the second-to-last row up to the first row
            for (int x = 0; x < width; ++x) {
                if (grid[y * width + x]) {
                    // Check if the cell below is empty
                    if (y + 1 < height && !grid[(y + 1) * width + x]) {
                        newGrid[(y + 1) * width + x] = true; // Drop sand down
                    }
                    else if (x > 0 && y + 1 < height && !grid[(y + 1) * width + (x - 1)]) {
                        newGrid[(y + 1) * width + (x - 1)] = true; // Diagonal left
                    }
                    else if (x + 1 < width && y + 1 < height && !grid[(y + 1) * width + (x + 1)]) {
                        newGrid[(y + 1) * width + (x + 1)] = true; // Diagonal right
                    }
                    else {
                        newGrid[y * width + x] = true; // Stay in place if no space to fall
                    }
                }
            }
        }

        // Ensure particles at the bottom row remain in place
        for (int x = 0; x < width; ++x) {
            if (grid[(height - 1) * width + x]) {
                newGrid[(height - 1) * width + x] = true;
            }
        }

        grid = newGrid;
    }

    void addSandAt(int x, int y) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            grid[y * width + x] = true; // Add a sand particle
        }
    }

    void render(SDL_Renderer* renderer, int cellSize) const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y * width + x]) {
                    SDL_FRect rect = { x * cellSize, y * cellSize, cellSize, cellSize };
                    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow for sand
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }

private:
    int width, height;
    std::vector<bool> grid;
};
#endif