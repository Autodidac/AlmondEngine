#pragma once

#include "aleMovementEvent.h"

#include <map>
#include <string>
#include <vector>
#include <functional>
#include <mutex>
#include <iostream>
#include <stdexcept>

#ifdef _MSC_VER
//#pragma warning(disable : 26495) // suppress uninitialized event, it's intialized throughout the engine
#endif
namespace almond {

#undef KeyPress
enum class EventType {
    MouseButtonClick,
    MouseMove,
    KeyPress,
    TextInput,
    Unknown // Added for error handling
};

struct Event {
    almond::EventType type{};
    std::map<std::string, std::string> data;
    float x = 0.0f, y = 0.0f;  // Mouse position
    int key = 0;               // Key press code
    char text[2] = { '\0', '\0' };  // Text input (character)
};

class EventSystem {
public:
    void PollEvents();
    void RegisterCallback(const std::function<void(const almond::Event&)>& callback);
/*
    // Add a movement event as a unique pointer
    void addMovementEvent(std::unique_ptr<almond::MovementEvent> event) {
        std::lock_guard<std::mutex> lock(movementMutex);
        movementEvents.push_back(std::move(event));
    }

    // Return a vector of movement events as unique pointers and clear the internal storage
    std::vector<std::unique_ptr<almond::MovementEvent>> getMovementEvents() {
        std::lock_guard<std::mutex> lock(movementMutex);
        std::vector<std::unique_ptr<almond::MovementEvent>> eventsCopy;
        eventsCopy.swap(movementEvents); // Efficiently swap the contents
        return eventsCopy; // Return the unique pointers
    }

    // Clear the movement events (if needed, although it's already cleared after getMovementEvents)
    void clearMovementEvents() {
        std::lock_guard<std::mutex> lock(movementMutex);
        movementEvents.clear();
    }
*/
private:
   // std::vector<std::unique_ptr<almond::MovementEvent>> movementEvents; // Store movement events as unique pointers
    std::vector<std::function<void(const Event&)>> callbacks;
    std::mutex callbackMutex; // Protect callback registrations
    std::mutex movementMutex; // Protect movement events
};

// Utility functions for EventType conversion
std::string EventTypeToString(almond::EventType type);
almond::EventType StringToEventType(const std::string& str);
}