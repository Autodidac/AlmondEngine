/*#include <SDL3/SDL.h>
#include <vector>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

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

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
        return -1;
    }

    // Create window and renderer
    SDL_Window* window = SDL_CreateWindow("Cellular Automaton", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!window) {
        SDL_Log("Failed to create SDL window: %s", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        SDL_Log("Failed to create SDL renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));

    // Create the cellular automaton and sand simulation
    CellularAutomaton automaton(80, 60); // Grid size for Game of Life
    SandSimulation sandSim(160, 120); // Grid size for sand simulation
    automaton.randomize();
    sandSim.randomize();

    bool running = true;
    SDL_Event event;
    bool isGameOfLife = true; // Toggle between Game of Life and sand simulation

    // Variables for mouse input
    float mouseX = 0, mouseY = 0;
    bool isMousePressed = false;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false; // Exit when the window is closed
            }

            // Toggle simulation type on key press
            if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_SPACE) {
                    isGameOfLife = !isGameOfLife; // Switch between simulations
                }
            }

            // Handle mouse events
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                SDL_GetMouseState(&mouseX, &mouseY);
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isMousePressed = true;
                }
            }
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isMousePressed = false;
                }
            }
        }

        // Update the simulation
        if (isMousePressed && !isGameOfLife) {
            int gridX = mouseX / 5; // Calculate grid X position
            int gridY = mouseY / 5; // Calculate grid Y position
            sandSim.addSandAt(gridX, gridY);
        }

        if (isGameOfLife) {
            automaton.update();
        }
        else {
            sandSim.update();
        }

        // Clear the renderer and render the chosen simulation
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        if (isGameOfLife) {
            automaton.render(renderer, 10); // Cell size
        }
        else {
            sandSim.render(renderer, 5); // Cell size
        }

        SDL_RenderPresent(renderer);

        // Delay for a bit to control update speed
        SDL_Delay(10);
    }

    // Clean up resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
*/


#include <SDL3/SDL.h>
#include <array>
#include <iostream>
#include <string>
#include <vector>

namespace SnakeGame {

    // Constants
    constexpr int SCREEN_WIDTH = 800;
    constexpr int SCREEN_HEIGHT = 600;
    constexpr int TILE_SIZE = 20;
    constexpr int NUM_TILES_X = SCREEN_WIDTH / TILE_SIZE;
    constexpr int NUM_TILES_Y = SCREEN_HEIGHT / TILE_SIZE;

    // Directions for the snake
    enum class Direction {
        Up,
        Down,
        Left,
        Right
    };

    // Position structure
    struct Position {
        int x;
        int y;

        bool operator==(const Position& other) const {
            return x == other.x && y == other.y;
        }
    };

    // Snake class
    class Snake {
    public:
        Snake(Position startPos)
            : body{ startPos }, direction(Direction::Right) {
        }

        void move(int screenWidth, int screenHeight) {
            Position newHead = body.front();

            switch (direction) {
            case Direction::Up:
                newHead.y -= TILE_SIZE;
                break;
            case Direction::Down:
                newHead.y += TILE_SIZE;
                break;
            case Direction::Left:
                newHead.x -= TILE_SIZE;
                break;
            case Direction::Right:
                newHead.x += TILE_SIZE;
                break;
            }

            // Wrap around logic: check if the head has moved outside the screen bounds
            if (newHead.x < 0) {
                newHead.x = screenWidth - TILE_SIZE;
            }
            else if (newHead.x >= screenWidth) {
                newHead.x = 0;
            }

            if (newHead.y < 0) {
                newHead.y = screenHeight - TILE_SIZE;
            }
            else if (newHead.y >= screenHeight) {
                newHead.y = 0;
            }

            // Insert the new head at the front of the body
            body.insert(body.begin(), newHead);
            body.pop_back();
        }


        void grow() {
            // Create a new segment in the direction of the last segment
            Position newSegment = body.back();

            // Extend the new segment in the direction opposite to the current head's direction
            switch (direction) {
            case Direction::Up:
                newSegment.y += TILE_SIZE;
                break;
            case Direction::Down:
                newSegment.y -= TILE_SIZE;
                break;
            case Direction::Left:
                newSegment.x += TILE_SIZE;
                break;
            case Direction::Right:
                newSegment.x -= TILE_SIZE;
                break;
            }

            body.push_back(newSegment);
        }

        void setDirection(Direction newDirection) {
            if ((direction == Direction::Up && newDirection != Direction::Down) ||
                (direction == Direction::Down && newDirection != Direction::Up) ||
                (direction == Direction::Left && newDirection != Direction::Right) ||
                (direction == Direction::Right && newDirection != Direction::Left)) {
                direction = newDirection;
            }
        }

        const std::vector<Position>& getBody() const {
            return body;
        }

    private:
        std::vector<Position> body;
        Direction direction;
    };

    // Food class
    class Food {
    public:
        Food(Position pos)
            : position(pos) {
        }

        const Position& getPosition() const {
            return position;
        }

        void respawn(const std::vector<Position>& occupiedPositions) {
            do {
                position.x = (rand() % NUM_TILES_X) * TILE_SIZE;
                position.y = (rand() % NUM_TILES_Y) * TILE_SIZE;
            } while (std::find(occupiedPositions.begin(), occupiedPositions.end(), position) != occupiedPositions.end());
        }

    private:
        Position position;
    };

    // Main Game class
    class Game {
    public:
        Game()
            : snake({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 }),
            food({ TILE_SIZE * 5, TILE_SIZE * 5 }),
            window(nullptr),
            renderer(nullptr),
            running(true) {
        }

        ~Game() {
            if (renderer) SDL_DestroyRenderer(renderer);
            if (window) SDL_DestroyWindow(window);
            SDL_Quit();
        }

        bool initialize() {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << '\n';
                return false;
            }

            window = SDL_CreateWindow("Snake Game", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_INPUT_FOCUS);
            if (!window) {
                std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << '\n';
                return false;
            }

            renderer = SDL_CreateRenderer(window, nullptr);
            if (!renderer) {
                std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << '\n';
                return false;
            }

            return true;
        }

        void run() {
            while (running) {
                handleEvents();
                update();
                render();
                SDL_Delay(100);
            }
        }

    private:
        void handleEvents() {
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_EVENT_QUIT) {
                    running = false;
                }
                else if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                    case SDLK_UP:
                        snake.setDirection(Direction::Up);
                        break;
                    case SDLK_DOWN:
                        snake.setDirection(Direction::Down);
                        break;
                    case SDLK_LEFT:
                        snake.setDirection(Direction::Left);
                        break;
                    case SDLK_RIGHT:
                        snake.setDirection(Direction::Right);
                        break;
                    }
                }
            }
        }

        void update() {
            snake.move(SCREEN_WIDTH, SCREEN_HEIGHT);

            // Check collision with food
            if (snake.getBody().front() == food.getPosition()) {
                snake.grow();
                food.respawn(snake.getBody());
            }

            // Check collision with walls and wrap around
            auto head = snake.getBody().front();
            if (head.x < 0 || head.x >= SCREEN_WIDTH || head.y < 0 || head.y >= SCREEN_HEIGHT) {
                // Wrap around the screen
                if (head.x < 0) head.x = SCREEN_WIDTH - TILE_SIZE;
                else if (head.x >= SCREEN_WIDTH) head.x = 0;
                if (head.y < 0) head.y = SCREEN_HEIGHT - TILE_SIZE;
                else if (head.y >= SCREEN_HEIGHT) head.y = 0;
            }

            // Check collision with itself
            auto& body = snake.getBody();
            if (std::find(body.begin() + 1, body.end(), head) != body.end()) {
                std::cout << "Self-collision detected!\n";
                resetGame();
            }
        }

        void render() {
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Render snake
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            for (const auto& segment : snake.getBody()) {
                SDL_FRect rect{ static_cast<float>(segment.x), static_cast<float>(segment.y), static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE) };
                SDL_RenderFillRect(renderer, &rect);
            }

            // Render food
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_FRect foodRect{ static_cast<float>(food.getPosition().x), static_cast<float>(food.getPosition().y), static_cast<float>(TILE_SIZE), static_cast<float>(TILE_SIZE) };
            SDL_RenderFillRect(renderer, &foodRect);

            SDL_RenderPresent(renderer);
        }

        void resetGame() {
            std::cout << "Game reset!\n";
            snake = Snake({ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 });
            food.respawn(snake.getBody());
        }

        Snake snake;
        Food food;
        SDL_Window* window;
        SDL_Renderer* renderer;
        bool running;
    };

} // namespace SnakeGame

int main() {
    SnakeGame::Game game;
    if (game.initialize()) {
        game.run();
    }
    return 0;
}

