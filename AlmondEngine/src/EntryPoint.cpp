#include "EntryPoint.h"
#include <iostream>

namespace almond {

    // Constructor for EntryPoint
    EntryPoint::EntryPoint(int width, int height, const std::wstring& title)
        : width(width), height(height), title(title) {
        // Initialize platform-specific resources (e.g., create window)
        std::wcout << L"EntryPoint created: " << title << L" (" << width << L"x" << height << L")" << std::endl;
    }

    // Destructor
    EntryPoint::~EntryPoint() noexcept {
        // Cleanup platform-specific resources (e.g., destroy window)
        std::wcout << L"EntryPoint destroyed: " << title << std::endl;
    }

    // The show() method
    void EntryPoint::show() const {
        // Platform-specific code to show the window
        std::wcout << L"Showing window: " << title << std::endl;
    }

    // The pollEvents() method
    bool EntryPoint::pollEvents() const {
        // Platform-specific code to poll events (this could involve an event loop)
        std::wcout << L"Polling events for window: " << title << std::endl;
        return true; // Simulate events being polled
    }
/*
    // Static function to create and access the cross-platform entry point
    std::optional<std::unique_ptr<EntryPoint_Crossplatform>>& EntryPoint::getEntryPoint() {
        static std::optional<std::unique_ptr<EntryPoint_Crossplatform>> entryPoint;
        return entryPoint;
    }*/

    // Exported function to create an EntryPoint instance (for use by external code)
    EntryPoint* CreateEntryPoint(int width, int height, const wchar_t* title) {
        return new EntryPoint(width, height, title);
    }

} // namespace almond
