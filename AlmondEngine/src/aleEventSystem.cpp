
#include "aleEventSystem.h"

namespace almond {

    // Converts EventType to string
    std::string EventTypeToString(EventType type) {
        switch (type) {
        case EventType::MouseButtonClick: return "MouseButtonClick";
        case EventType::MouseMove: return "MouseMove";
        case EventType::KeyPress: return "KeyPress";
        case EventType::TextInput: return "TextInput";
        default: return "Unknown"; // For unexpected or invalid EventType
        }
    }

    // Converts string to EventType with error handling
    EventType StringToEventType(const std::string& str) {
        if (str == "MouseButtonClick") return EventType::MouseButtonClick;
        if (str == "MouseMove") return EventType::MouseMove;
        if (str == "KeyPress") return EventType::KeyPress;
        if (str == "TextInput") return EventType::TextInput;
        return EventType::Unknown; // Default to Unknown if not found
    }

    void EventSystem::PollEvents() {
        // Example: Call registered callbacks with a mock event
        Event mockEvent{ EventType::MouseMove, {{"action", "move"}}, 100.0f, 200.0f };

        std::lock_guard<std::mutex> lock(callbackMutex);
        for (const auto& callback : callbacks) {
            callback(mockEvent);
        }
    }

    void EventSystem::RegisterCallback(const std::function<void(const Event&)>& callback) {
        std::lock_guard<std::mutex> lock(callbackMutex);
        callbacks.push_back(callback);
    }
}