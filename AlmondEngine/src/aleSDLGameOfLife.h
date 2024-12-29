#pragma once

#include "aleEngineConfig.h"

#ifdef ALMOND_USING_SDL

// Define a basic Cellular Automaton class
class CellularAutomaton {
public:
    CellularAutomaton(int width, int height)
        : width(width), height(height), grid(width* height, false) {
    }

    void randomize() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                grid[y * width + x] = (rand() % 2 == 0);
            }
        }
    }

    void update() {
        std::vector<bool> newGrid(width * height, false);

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                int liveNeighbors = countLiveNeighbors(x, y);
                if (grid[y * width + x]) {
                    newGrid[y * width + x] = (liveNeighbors == 2 || liveNeighbors == 3);
                }
                else {
                    newGrid[y * width + x] = (liveNeighbors == 3);
                }
            }
        }

        grid = newGrid;
    }

    void render(SDL_Renderer* renderer, int cellSize) const {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y * width + x]) {
                    SDL_FRect rect = { x * cellSize, y * cellSize, cellSize, cellSize };
                    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White for live cells
                    SDL_RenderFillRect(renderer, &rect);
                }
            }
        }
    }

private:
    int width, height;
    std::vector<bool> grid;

    int countLiveNeighbors(int x, int y) const {
        int count = 0;
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue; // Skip the cell itself
                int nx = x + dx, ny = y + dy;
                if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
                    count += grid[ny * width + nx] ? 1 : 0;
                }
            }
        }
        return count;
    }
};
#endif