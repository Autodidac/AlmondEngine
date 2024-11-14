#pragma once

#include "EntryPoint_Crossplatform.h"

#include <memory>
#include <optional>
#include <string>

#ifdef _MSC_VER
// Only for MSVC compilers
#pragma warning(disable : 4251)
#pragma warning(disable : 4273)
#endif

namespace almond {

    // Forward declaration of EntryPoint
   // class EntryPoint;
/*
    // Static member definition (should be in the corresponding cpp file)
    inline std::optional<std::unique_ptr<EntryPoint_Crossplatform>>& EntryPoint::getEntryPoint() {
        static std::optional<std::unique_ptr<EntryPoint_Crossplatform>> entryPoint;
        return entryPoint;
    }
*/
    // Exported function to create the EntryPoint instance

    // The EntryPoint class encapsulates the platform-specific details of the windowing and event system
    class ENTRYPOINTLIBRARY_API EntryPoint {
    public:
        // Constructor and Destructor
        EntryPoint(int width, int height, const std::wstring& title);
        ~EntryPoint() noexcept; // noexcept ensures exception safety

        // Move constructor and move assignment operator
        EntryPoint(EntryPoint&&) noexcept = default;
        EntryPoint& operator=(EntryPoint&&) noexcept = default;

        // Show the window
        void show() const;

        // Poll events for the window
        bool pollEvents() const;

        // Static method to create and access the cross-platform entry point
       // static std::optional<std::unique_ptr<EntryPoint_Crossplatform>>& getEntryPoint();

    private:
        // Class now handles all logic directly (no more Impl class)
        int width;
        int height;
        std::wstring title;

        // You can add platform-specific resources directly here (e.g., window handle)
    };

    extern "C" EntryPoint* CreateEntryPoint(int width, int height, const wchar_t* title);

} // namespace almond
