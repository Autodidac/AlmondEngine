#pragma once

#include <chrono>
#include <concepts>
#include <format>
#include <functional>
#include <map>
#include <ranges>
#include <string>
#include <vector>

namespace almond 
{
    template<typename T>
    concept Clockable = requires(T a) {
        { T::now() } -> std::convertible_to<typename T::time_point>;
        typename T::duration;
        typename T::time_point;
    };

    class RobustTimeSystem {
    public:
        template<Clockable Clock = std::chrono::system_clock>
        struct TimePoint {
            typename Clock::time_point time;
            
            TimePoint() : time(Clock::now()) {}
            explicit TimePoint(const typename Clock::time_point& t) : time(t) {}
        };

        using SystemTimePoint = TimePoint<std::chrono::system_clock>;
        using SteadyTimePoint = TimePoint<std::chrono::steady_clock>;

        RobustTimeSystem() : currentSystemTime(SystemTimePoint()), currentSteadyTime(SteadyTimePoint()), gameTimeScale(1.0) {}

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

        [[nodiscard]] std::string getCurrentTimeString(const std::string& format = "%Y-%m-%d %H:%M:%S") const {
            auto timeT = std::chrono::system_clock::to_time_t(currentSystemTime.time);
            std::tm tm_time = *std::localtime(&timeT);
            return std::vformat(format, std::make_format_args(tm_time));
        }

        class Timer {
        public:
            explicit Timer(RobustTimeSystem& system) : timeSystem(system), running(false) {}

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
            RobustTimeSystem& timeSystem;
            SteadyTimePoint startTime;
            SteadyTimePoint endTime;
            bool running;
        };

        [[nodiscard]] Timer createTimer() { return Timer(*this); }

        [[nodiscard]] std::string getTimeInTimeZone(const std::string& timeZone) const {
            // Note: This is still a placeholder. Proper time zone handling would require additional libraries.
            auto timeT = std::chrono::system_clock::to_time_t(currentSystemTime.time);
            std::tm tm_time = *std::localtime(&timeT);
            return std::format("Time in {}: {:%Y-%m-%d %H:%M:%S}", timeZone, tm_time);
        }

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
} // namespace almond

/* ------ Usage Example ---------
#include "RobustTimeSystem.hpp"
#include <iostream>

int main() {
    almond::RobustTimeSystem timeSystem;

    // Create and use a timer
    auto timer = timeSystem.createTimer();
    timer.start();
    // ... do some work ...
    timer.stop();
    std::cout << std::format("Elapsed time: {:.2f} seconds", timer.elapsed()) << std::endl;

    // Advance time
    timeSystem.advanceTime(std::chrono::hours(1));

    // Get current time
    std::cout << std::format("Current time: {}", timeSystem.getCurrentTimeString()) << std::endl;

    // Set an alarm
    timeSystem.setAlarm(
        almond::RobustTimeSystem::SystemTimePoint(std::chrono::system_clock::now() + std::chrono::minutes(5)),
        []() { std::cout << "Alarm triggered!" << std::endl; }
    );

    // Check for alarms (you'd typically do this in your main loop)
    timeSystem.checkAndTriggerAlarms();

    return 0;}*/