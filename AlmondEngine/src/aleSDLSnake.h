#pragma once
#include "aleEngineConfig.h"

#ifdef ALMOND_USING_SDL

// SnakeGame.h

#include <vector>
#include <random>
//#include <SDL3/SDL.h>

inline bool operator==(const SDL_Point& a, const SDL_Point& b) {
    return a.x == b.x && a.y == b.y;
}

class SnakeGame {
public:
    enum class Direction { Up, Down, Left, Right };

    SnakeGame(int width, int height)
        : width(width), height(height), direction(Direction::Right), lastDirection(Direction::Right), snakeLength(1) {
        snake.push_back({ width / 2, height / 2 }); // Initial position
        std::cout << "Initial snake position: (" << width / 2 << ", " << height / 2 << ")" << std::endl;
        placeFood();
    }

    int update(float deltaTime) {
        static float accumulatedTime = 0.0f;
        const float moveInterval = 0.1f;

        accumulatedTime += deltaTime;

        if (accumulatedTime < moveInterval) {
            return 0;
        }

        accumulatedTime -= moveInterval;

        auto& head = snake.front();
        SDL_Point newHead = head;

        switch (direction) {
        case Direction::Up:    newHead.y -= 1; break;
        case Direction::Down:  newHead.y += 1; break;
        case Direction::Left:  newHead.x -= 1; break;
        case Direction::Right: newHead.x += 1; break;
        }

        // Check for collision with the wall and wrap around
        if (newHead.x < 0) newHead.x = width - 1;
        else if (newHead.x >= width) newHead.x = 0;

        if (newHead.y < 0) newHead.y = height - 1;
        else if (newHead.y >= height) newHead.y = 0;

        // Check for collision with itself
        for (size_t i = 0; i < snake.size(); ++i) {
            if (newHead == snake[i]) {
                resetGame();
                return 0;
            }
        }

        // Move the snake by updating the head and removing the last segment
        snake.insert(snake.begin(), newHead);
        if (snake.size() > snakeLength) {
            snake.pop_back();
        }

        // Check if the snake's head collides with the food
        if (newHead == food) {
            snakeLength++;
            placeFood();
        }

        // Update the last direction
        lastDirection = direction;

        return 0;
    }

    void render(SDL_Renderer* renderer, float cellSize) {
        // Draw each part of the snake
        for (const auto& segment : snake) {
            rect = { segment.x * cellSize, segment.y * cellSize, cellSize, cellSize };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Green for the snake
            SDL_RenderFillRect(renderer, &rect);
        }

        // Draw food

        foodRect = { food.x * cellSize, food.y * cellSize, cellSize, cellSize };


        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Red for the food
        SDL_RenderFillRect(renderer, &foodRect);
    }

    void updateDirection(Direction newDirection) {
        // Prevent reversing direction into the tail when there are only two segments
        if (snake.size() == 2) {
            auto& head = snake.front();
            auto& tail = snake.back();
            SDL_Point predictedHead = head;

            switch (newDirection) {
            case Direction::Up:    predictedHead.y -= 1; break;
            case Direction::Down:  predictedHead.y += 1; break;
            case Direction::Left:  predictedHead.x -= 1; break;
            case Direction::Right: predictedHead.x += 1; break;
            }

            if (predictedHead == tail) {
                return; // Avoid reversing into the tail
            }
        }

        // Prevent reversing direction for longer snakes
        if ((lastDirection == Direction::Up && newDirection == Direction::Down) ||
            (lastDirection == Direction::Down && newDirection == Direction::Up) ||
            (lastDirection == Direction::Left && newDirection == Direction::Right) ||
            (lastDirection == Direction::Right && newDirection == Direction::Left)) {
            return;
        }

        direction = newDirection;
    }

private:
    int width, height;
    Direction direction;
    SDL_FRect foodRect;
    SDL_FRect rect;
    int snakeLength;
    std::vector<SDL_Point> snake;
    SDL_Point food;
    float deltaTime;
    Direction lastDirection; // Tracks the last valid direction

    void placeFood() {
        static std::random_device rd;
        static std::mt19937 rng(rd());
        std::uniform_int_distribution<int> distX(0, width - 1);
        std::uniform_int_distribution<int> distY(0, height - 1);

        food.x = distX(rng);
        food.y = distY(rng);

        // Find a valid position where the food does not appear on the snake's body
        while (std::find_if(snake.begin(), snake.end(), [this](const SDL_Point& point) {
            return point.x == this->food.x && point.y == this->food.y;
            }) != snake.end()) {
            food.x = distX(rng);
            food.y = distY(rng);
        }

        // Debug output to verify food placement
        std::cout << "Food placed at: (" << food.x << ", " << food.y << ")" << std::endl;
    }

    void resetGame() {
        snake.clear();
        snake.push_back({ width / 2, height / 2 });
        direction = Direction::Right;
        snakeLength = 1;
        placeFood();
    }
};
#endif