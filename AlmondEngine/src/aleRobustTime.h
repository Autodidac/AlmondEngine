#pragma once

#include <chrono>
#include <concepts>
#include <format>
#include <functional>
#include <map>
#include <ranges>
#include <string>
#include <vector>
#include <ctime>
#include <iomanip>  // For std::put_time

namespace almond {

    template<typename T>
    concept Clockable = requires(T a) {
        { T::now() } -> std::convertible_to<typename T::time_point>;
        typename T::duration;
        typename T::time_point;
    };

    class RobustTime {
    public:
        template<Clockable Clock = std::chrono::system_clock>
        struct TimePoint {
            typename Clock::time_point time;

            TimePoint() : time(Clock::now()) {}
            explicit TimePoint(const typename Clock::time_point& t) : time(t) {}
            // Define a comparison operator
            auto operator<=>(const TimePoint&) const = default;
        };

        using SystemTimePoint = TimePoint<std::chrono::system_clock>;
        using SteadyTimePoint = TimePoint<std::chrono::steady_clock>;

        RobustTime() : currentSystemTime(SystemTimePoint()), currentSteadyTime(SteadyTimePoint()), gameTimeScale(1.0) {}

        void setCurrentTime(const SystemTimePoint& time) {
            currentSystemTime = time;
            currentSteadyTime = SteadyTimePoint();
        }

        template<typename Rep, typename Period>
        void advanceTime(const std::chrono::duration<Rep, Period>& duration) {
            currentSystemTime.time += std::chrono::duration_cast<std::chrono::system_clock::duration>(duration);
            currentSteadyTime.time += std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration);
        }

        template<typename Rep, typename Period>
        void rewindTime(const std::chrono::duration<Rep, Period>& duration) {
            currentSystemTime.time -= std::chrono::duration_cast<std::chrono::system_clock::duration>(duration);
            currentSteadyTime.time -= std::chrono::duration_cast<std::chrono::steady_clock::duration>(duration);
        }

        void setGameTimeScale(double scale) { gameTimeScale = scale; }
        [[nodiscard]] constexpr double getGameTimeScale() const { return gameTimeScale; }

        [[nodiscard]] SystemTimePoint getCurrentSystemTime() const { return currentSystemTime; }
        [[nodiscard]] SteadyTimePoint getCurrentSteadyTime() const { return currentSteadyTime; }


        [[nodiscard]] std::string getCurrentTimeString(const std::string format = "%Y-%m-%d %H:%M:%S") const {
            // Get the current system time
            auto timeT = std::chrono::system_clock::to_time_t(static_cast<std::chrono::system_clock::time_point>(currentSystemTime.time));

            // Convert to tm structure
            std::tm tm_time;
            // Thread-safe time handling
#if defined(_WIN32) || defined(_WIN64)
            localtime_s(&tm_time, &timeT);  // Windows-specific thread-safe localtime
#else
            // For non-Windows, ensure thread safety
            static std::mutex mtx;
            std::lock_guard<std::mutex> lock(mtx);
            tm_time = *std::localtime(&timeT); // For other platforms, using standard localtime
#endif

            // Use std::ostringstream to format the time string
            std::ostringstream oss;
            oss << std::put_time(&tm_time, format.c_str());  // Format the time according to the specified format

            return oss.str();  // Return the formatted string
        }

        class Timer {
        public:
            explicit Timer(RobustTime& system) : timeSystem(system), running(false) {}

            void start() {
                startTime = timeSystem.getCurrentSteadyTime();
                running = true;
            }

            void stop() {
                endTime = timeSystem.getCurrentSteadyTime();
                running = false;
            }

            [[nodiscard]] double elapsed() const {
                auto end = running ? timeSystem.getCurrentSteadyTime() : endTime;
                return std::chrono::duration<double>(end.time - startTime.time).count();
            }

        private:
            RobustTime& timeSystem;
            SteadyTimePoint startTime;
            SteadyTimePoint endTime;
            bool running;
        };

        [[nodiscard]] Timer createTimer() { return Timer(*this); }

/* [[nodiscard]] std::string getTimeInTimeZone(const std::string& timeZone) {
            auto timeT = std::chrono::system_clock::to_time_t(currentSystemTime.time);
            std::tm tm_time = *std::localtime(&timeT);
            return std::format("Time in {}: {:%Y-%m-%d %H:%M:%S}", timeZone, tm_time);
        }
*/
        using AlarmCallback = std::function<void()>;
        void setAlarm(const SystemTimePoint& alarmTime, AlarmCallback callback) {
            alarms[alarmTime] = std::move(callback);
        }

        void checkAndTriggerAlarms() {
            auto now = getCurrentSystemTime();
            std::erase_if(alarms, [&](const auto& item) {
                auto const& [time, callback] = item;
                if (time.time <= now.time) {
                    callback();
                    return true;
                }
                return false;
                });
        }

    private:
        SystemTimePoint currentSystemTime;
        SteadyTimePoint currentSteadyTime;
        double gameTimeScale;
        std::map<SystemTimePoint, AlarmCallback> alarms;
    };
}
// namespace almond
/*
// ------ Usage Example ---------
#include "RobustTime.h"
#include <iostream>
#include <thread>

int main() {
    almond::RobustTime timeSystem;

    // Create and use a timer
    auto timer = timeSystem.createTimer();
    timer.start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    timer.stop();
    std::cout << std::format("Elapsed time: {:.2f} seconds", timer.elapsed()) << std::endl;

    // Set and manipulate game time scale
    timeSystem.setGameTimeScale(2.0); // Double speed
    timeSystem.advanceTime(std::chrono::seconds(5));
    std::cout << "Current system time after advancing (scaled): " << timeSystem.getCurrentTimeString() << std::endl;

    // Set an alarm to trigger in 3 seconds
    timeSystem.setAlarm(
        almond::RobustTime::SystemTimePoint(std::chrono::system_clock::now() + std::chrono::seconds(3)),
        []() { std::cout << "Alarm triggered!" << std::endl; }
    );

    // Run the alarm check loop (for simulation)
    for (int i = 0; i < 10; ++i) {
        timeSystem.checkAndTriggerAlarms();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Get the current time in a specific time zone (still a placeholder)
    std::cout << timeSystem.getTimeInTimeZone("UTC") << std::endl;

    return 0;
}

*/