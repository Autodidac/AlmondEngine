/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
//alogger.hpp
#pragma once

#include "aplatform.hpp"      // Must always come first for platform defines

#include "arobusttime.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <stdexcept>
#include <format>
#include <chrono>
#include <filesystem>

namespace almondnamespace
{
    enum class LogLevel
    {
        INFO,
        WARN,
        ALMOND_ERROR
    };

    class Logger
    {
    public:
        Logger(const std::string& filename, LogLevel level = LogLevel::INFO)
            : logFile(filename, std::ios::app), logFileName(filename), logLevel(level)
        {
            std::cout << "Attempting to open log file: " << filename << std::endl;

            // Ensure the directory exists when a directory component is provided
            std::filesystem::path logPath(filename);
            const auto parentPath = logPath.parent_path();
            if (!parentPath.empty() && !std::filesystem::exists(parentPath))
            {
                throw std::runtime_error("Directory for log file does not exist: " + parentPath.string());
            }

            if (!logFile.is_open()) {
                std::cerr << "Failed to open log file: " << filename << std::endl;
                throw std::runtime_error("Could not open log file: " + filename);
            }
        }

        ~Logger()
        {
            if (logFile.is_open())
            {
                logFile.close();
            }
        }

        inline static Logger& GetInstance(const std::string& logFileName)
        {
            static Logger instance(logFileName);  // Ensure timeSystem is dereferenced
            return instance;
        }

        void log(const std::string& message, LogLevel level = LogLevel::INFO)
        {
            std::lock_guard<std::mutex> lock(mutex);

            // Only log messages that meet or exceed the current log level
            if (level >= logLevel) {
                logFile << time::getCurrentTimeString() << " [" << logLevelToString(level) << "] - " << message << std::endl;
            }
        }

        std::string getLogFileName() const
        {
            return logFileName;
        }

    private:
        std::ofstream logFile;
        std::mutex mutex;
        std::string logFileName;
       // almondnamespace::time::Timer& timeSystem;
        LogLevel logLevel;

        // Helper to convert log level enum to string
        std::string logLevelToString(LogLevel level) const {
            switch (level) {
            case LogLevel::INFO: return "INFO";
            case LogLevel::WARN: return "WARN";
            case LogLevel::ALMOND_ERROR: return "ERROR";
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