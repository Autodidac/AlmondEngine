#pragma once

#include "aleLogger.h"
#include "aleRobustTime.h"

#include <deque>
#include <iostream>
#include <memory>
#include <mutex>
#include <utility>
#include <string>
#include <chrono>
#define _CRT_SECURE_NO_WARNINGS
#include <ctime>

namespace almond {

    // HistoryManager class for managing the entity's state history
    class HistoryManager {
    public:
        // Constructor takes RobustTime reference for timestamping
        HistoryManager(const std::string logFile, RobustTime& timeSystem)
            : logger(logFile, timeSystem, almond::LogLevel::INFO), timeSystem(timeSystem) {
        }

        void saveState(float x, float y) {
            std::lock_guard<std::mutex> lock(mutex);
            if (history.size() >= MAX_HISTORY_SIZE) {
                history.pop_front(); // Remove the oldest state
            }
            // Get the current time from the time system and save state with the timestamp
            auto currentTime = timeSystem.getCurrentTimeString();
            history.emplace_back(x, y, currentTime); // Store the current state and timestamp
            logger.log("State saved: (" + std::to_string(x) + ", " + std::to_string(y) + ") at " + currentTime);
        }

        bool rewind(float& x, float& y) {
            std::lock_guard<std::mutex> lock(mutex);
            if (!history.empty()) {
                auto lastState = history.back();
                x = lastState.x;
                y = lastState.y;
                history.pop_back();
                logger.log("Rewind successful to: (" + std::to_string(x) + ", " + std::to_string(y) + ") at " + lastState.timeStamp);
                return true;
            }
            logger.log("No history to rewind to.");
            return false;
        }

    private:
        static constexpr size_t MAX_HISTORY_SIZE = 100;
        struct State {
            float x, y;
            std::string timeStamp;
            State(float x, float y, const std::string& timeStamp) : x(x), y(y), timeStamp(timeStamp) {}
        };
        std::deque<State> history; // Stores position history with timestamps
        mutable std::mutex mutex; // Mutex to protect history access
        almond::Logger logger; // Logger instance for history management
        almond::RobustTime& timeSystem; // Reference to the time system for timestamps
    };

    class Entity {
    public:
        // Default constructor
        Entity(const std::string& logFile, RobustTime& m_timeSystem)
            : id(0), posX(0.0f), posY(0.0f), historyManager(logFile, m_timeSystem), logger(logFile, m_timeSystem, almond::LogLevel::INFO), m_timeSystem(m_timeSystem) {
            if (!&m_timeSystem) {
                throw std::invalid_argument("timeSystem cannot be nullptr");
            }
            logger.log("Entity created with ID: " + std::to_string(id) + " at " + currentTime());
        }

        Entity(int id, float x, float y, const std::string& logFileName, RobustTime& m_timeSystem)
            : id(id), posX(x), posY(y), historyManager(logFileName, m_timeSystem), logger(logFileName, m_timeSystem, almond::LogLevel::INFO), m_timeSystem(m_timeSystem) {
            if (!&m_timeSystem) {
                throw std::invalid_argument("timeSystem cannot be nullptr");
            }
            logger.log("Entity created with ID: " + std::to_string(id) + " at " + currentTime());
        }


        // Print entity's position
        void printPosition() const {
            std::cout << "Entity " << id << " Position: (" << posX << ", " << posY << ")\n";
        }

        int getId() const { return id; }

        // Rewind to the last state

        // Move entity and log the state
        void move(float deltaX, float deltaY) {
            saveState(); // Save the current state before moving
            posX += deltaX;
            posY += deltaY;
            logger.log("Entity " + std::to_string(id) + " moved to: (" + std::to_string(posX) + ", " + std::to_string(posY) + ") at " + currentTime());
        }

        void rewind() {
            if (historyManager.rewind(posX, posY)) {
                logger.log("Entity " + std::to_string(id) + " rewound to: (" + std::to_string(posX) + ", " + std::to_string(posY) + ") at " + currentTime());
            }
            else {
                logger.log("No previous state to rewind to for Entity " + std::to_string(id) + " at " + currentTime());
            }
        }

        // Clone method: creates a new instance with the same ID and position
        std::unique_ptr<almond::Entity> clone() const {
            return std::make_unique<almond::Entity>(id, posX, posY, logger.getLogFileName(), m_timeSystem );
        }

    private:
        int id;
        float posX;
        float posY;
        HistoryManager historyManager; // Manages the history of states
        almond::Logger logger;         // Logger for the entity
        almond::RobustTime& m_timeSystem;        // Time system reference
        std::tm tm;

        void saveState() {
            historyManager.saveState(posX, posY); // Save the current position before changing it
        }

        std::string currentTime() const {
            auto now = std::chrono::system_clock::now();
            auto timeT = std::chrono::system_clock::to_time_t(now);
            std::tm tm;
            if (localtime_s(&tm, &timeT) != 0) {
                throw std::runtime_error("Failed to convert time to local time");
            }
            char buffer[80];
            std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm);
            return std::string(buffer);
        }

    };
} // namespace almond
