#pragma once

#include "Logger.h"

#include <deque>
#include <iostream>
#include <memory>
#include <utility>

namespace almond {
class Entity {
public:
    // Default constructor
    Entity()
        : id(0), posX(0.0f), posY(0.0f), logger("entity.log") {
        logger.log("Default Entity created with ID: " + std::to_string(id));
    }

    // Parameterized constructor
    Entity(int id, float x, float y, const std::string& logFileName = "entity.log")
        : id(id), posX(x), posY(y), logger(logFileName) {
        logger.log("Entity created with ID: " + std::to_string(id));
    }

    void move(float deltaX, float deltaY) {
        saveState(); // Save the current state before moving
        posX += deltaX;
        posY += deltaY;
        logger.log("Entity " + std::to_string(id) + " moved to: (" + std::to_string(posX) + ", " + std::to_string(posY) + ")");
    }

    void printPosition() {
        std::cout << "Entity " << id << " Position: (" << posX << ", " << posY << ")\n";
    }

    int getId() const { return id; }

    void rewind() {
        if (!history.empty()) {
            auto lastState = history.back();
            posX = lastState.first;
            posY = lastState.second;
            history.pop_back();
            logger.log("Entity " + std::to_string(id) + " rewound to: (" + std::to_string(posX) + ", " + std::to_string(posY) + ")");
        }
        else {
            logger.log("No previous state to rewind to for Entity " + std::to_string(id) + ".");
        }
    }

    // Clone method
    std::unique_ptr<Entity> clone() const {
        return std::make_unique<Entity>(id, posX, posY, logger.getLogFileName());
    }

private:
    int id;
    float posX;
    float posY;
    std::deque<std::pair<float, float>> history;

    void saveState() {
        if (history.size() >= MAX_HISTORY_SIZE) {
            history.pop_front(); // Remove the oldest state if max history size is reached
        }
        history.emplace_back(posX, posY); // Store the current state
    }

    static constexpr size_t MAX_HISTORY_SIZE = 100;
    almond::Logger logger; // Assuming Logger is properly defined and thread-safe if needed
};
} // namespace almond
