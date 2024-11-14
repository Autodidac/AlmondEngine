#pragma once

#include <fstream>
#include <string>
#include <mutex>
#include <stdexcept>

namespace almond 
{
    class Logger {
    public:
        Logger(const std::string& filename) : logFile(filename, std::ios::app), logFileName(filename) {
            if (!logFile.is_open()) {
                throw std::runtime_error("Could not open log file: " + filename);
            }
        }

        ~Logger() {
            if (logFile.is_open()) {
                logFile.close();
            }
        }

        void log(const std::string& message) {
            std::lock_guard<std::mutex> lock(mutex);
            if (logFile.is_open()) {
                logFile << message << std::endl;
            }
        }

        // New method to get the log file name
        std::string getLogFileName() const {
            return logFileName;
        }

    private:
        std::ofstream logFile; // File stream for logging
        std::mutex mutex; // Mutex for thread-safe logging
        std::string logFileName; // Variable to store the log file name
    };
} // namespace almond
