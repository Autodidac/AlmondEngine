#pragma once

#include "Exports_DLL.h"

#include <memory>

namespace almond {

    class ENTRYPOINTLIBRARY_API EntryPoint_Crossplatform {
    public:
        virtual ~EntryPoint_Crossplatform() = default;

        // Creates the entry point for the application with specified width, height, and title.
        virtual void createEntryPoint(int width, int height, const wchar_t* title) = 0;

        // Displays the created entry point (window, etc.).
        virtual void show() = 0;

        // Polls for events (like input, etc.) and returns whether any were processed.
        virtual bool pollEvents() = 0;

        // Factory method to create platform-specific entry points
        [[nodiscard]] std::unique_ptr<EntryPoint_Crossplatform> create(); 
    };

} // namespace almond