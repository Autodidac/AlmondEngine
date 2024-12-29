#pragma once

#include "aleRobustTime.h"

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <stdexcept>
#include <format>
#include <chrono>
#include <filesystem>

namespace almond {

    enum class LogLevel {
        INFO,
        WARN,
        ALMOND_ERROR
    };

    class Logger {
    public:
        Logger(const std::string& filename, almond::RobustTime& timeSystem, LogLevel level = LogLevel::INFO)
            : logFile(filename, std::ios::app), logFileName(filename), timeSystem(timeSystem), logLevel(level) {

            std::cout << "Attempting to open log file: " << filename << std::endl;

            // Ensure the directory exists
            std::filesystem::path logPath(filename);
            if (!std::filesystem::exists(logPath.parent_path())) {
                throw std::runtime_error("Directory for log file does not exist: " + logPath.parent_path().string());
            }

            if (!logFile.is_open()) {
                std::cerr << "Failed to open log file: " << filename << std::endl;
                throw std::runtime_error("Could not open log file: " + filename);
            }
        }

        ~Logger() {
            if (logFile.is_open()) {
                logFile.close();
            }
        }

        inline static Logger& GetInstance(const std::string& logFileName, RobustTime& timeSystem) {
            static Logger instance(logFileName, timeSystem);  // Ensure timeSystem is dereferenced
            return instance;
        }

        void log(const std::string& message, LogLevel level = LogLevel::INFO) {
            std::lock_guard<std::mutex> lock(mutex);

            // Only log messages that meet or exceed the current log level
            if (level >= logLevel) {
                logFile << timeSystem.getCurrentTimeString() << " [" << logLevelToString(level) << "] - " << message << std::endl;
            }
        }

        std::string getLogFileName() const {
            return logFileName;
        }

    private:
        std::ofstream logFile;
        std::mutex mutex;
        std::string logFileName;
        almond::RobustTime& timeSystem;
        almond::LogLevel logLevel;

        // Helper to convert log level enum to string
        std::string logLevelToString(almond::LogLevel level) const {
            switch (level) {
            case almond::LogLevel::INFO: return "INFO";
            case almond::LogLevel::WARN: return "WARN";
            case almond::LogLevel::ALMOND_ERROR: return "ERROR";
            default: return "UNKNOWN";
            }
        }
    };

} // namespace almond

/*
#include "Logger.h"
#include "alsRobustTime.h"
#include <iostream>

int main() {
    almond::RobustTime timeSystem;
    try {
        // Create a Logger object
        almond::Logger logger("logfile.txt", timeSystem, almond::LogLevel::INFO);

        // Log some messages
        logger.log("Application started", almond::LogLevel::INFO);
        logger.log("Performing some operations", almond::LogLevel::INFO);

        // Log a warning message
        logger.log("Low memory warning", almond::LogLevel::WARN);

        // Log an error message
        logger.log("Failed to load resource", almond::LogLevel::ERROR);

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}

*/