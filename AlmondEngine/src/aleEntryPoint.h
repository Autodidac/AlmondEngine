#pragma once

#include "aleUtilities.h"
#include "aleExports_DLL.h"

#include <memory>

namespace almond {

    class EntryPoint {
    public:
        virtual ~EntryPoint() = default;

        // Creates the entry point for the application with specified width, height, and title.
        //virtual void createEntryPoint(int width, int height, const wchar_t* title) = 0;

        // Displays the created entry point (window, etc.).
       // virtual void show();

        // Polls for events (like input, etc.) and returns whether any were processed.
       // virtual bool pollEvents();

        // Factory method to create platform-specific entry points
        [[nodiscard]] std::unique_ptr<EntryPoint> create(); 
/*
    private:
        // Class now handles all logic directly (no more Impl class)
        int width;
        int height;
        std::wstring title;
*/
    };

} // namespace almond