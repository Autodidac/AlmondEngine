// Crossplatform Utilities For Everyone

#pragma once

// Crossplatform Console Check 
#ifdef _WIN32

#include "framework.h"

#include <consoleapi3.h>
#else
// other platform includes
#endif

namespace almond
{
    inline bool isConsoleApplication() {
#ifdef _WIN32
    return GetConsoleWindow() != nullptr;
#else
    return isatty(fileno(stdout))
#endif

    }
}