
#include "EntryPoint_Crossplatform.h"
#include "EntryPoint_Headless.h"
#include "Utilities.h"

#include <memory>
#include <stdexcept>

#ifdef _WIN32
#ifndef _CONSOLE
    #include "EntryPoint_Win32.h"
#endif
#elif defined(__linux__)
    #include "EntryPoint_X11.h"
#elif defined(__APPLE__)
    #if TARGET_OS_IOS
        #include "EntryPoint_iOS.mm"
    #else
        #include "EntryPoint_Cocoa.mm"
    #endif
#elif defined(__ANDROID__)
    #include "EntryPoint_Android.cpp"
#elif defined(__EMSCRIPTEN__)
    #include "EntryPoint_Emscripten.cpp"
#elif defined(_XBOX)
    #include "EntryPoint_XboxSeriesXS.h"
#elif defined(_PS5)
    #include "EntryPoint_PS5.h"
#endif

// fixed em :)
//I'll remove this soon
//#ifdef _MSC_VER
// Only for MSVC compilers
//#pragma warning(disable : 4251)
//#pragma warning(disable : 4273)
//#endif

namespace almond {
   /// @brief  Crossplatform Factory Function
   /// @return Returns Contextual Entry Point
   [[nodiscard]] std::unique_ptr<EntryPoint_Crossplatform> create() {
        // Check for console application
        if (isConsoleApplication()) {
#ifdef _CONSOLE
            return std::make_unique<HeadlessEntryPoint>();
#endif
        }
        // Platform-specific instantiation
#ifdef _WIN32
#ifndef _CONSOLE
        return std::make_unique<Win32EntryPoint>();
#endif
#elif defined(__linux__)
        return std::make_unique<X11EntryPoint>();
#elif defined(__APPLE__)
        #if TARGET_OS_IOS
                return std::make_unique<IOSEntryPoint>();
        #else
                return std::make_unique<CocoaEntryPoint>();
        #endif
#elif defined(__ANDROID__)
        return std::make_unique<AndroidEntryPoint>();
#elif defined(__EMSCRIPTEN__)
        return std::make_unique<WasmEntryPoint>();
#elif defined(_XBOX)
        return std::make_unique<XboxEntryPoint>();
#elif defined(_PS5)
        return std::make_unique<PS5EntryPoint>();
#else
        throw std::runtime_error("Platform not supported");
#endif
        // Ideally, this return statement should never be reached
        return nullptr; // Add a return statement to satisfy all paths.
    }
} // namespace almond
