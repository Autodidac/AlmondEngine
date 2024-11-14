#include "Exports_DLL.h"

// Here we would include the actual headers for the classes or functions to export
#include "EntryPoint.h"
#include "EntryPoint_Crossplatform.h"

// Define any exported functions or classes, even if they don't have additional functionality.
/*
// If you're using a static entry point function or some kind of initialization, it might look like:
extern "C" ENTRYPOINTLIBRARY_API void createEntryPoint() {
    // Initialize or run entry point logic
    // For example, call a function that initializes some part of your library
    almond::EntryPoint();
}
*/

//class ENTRYPOINTLIBRARY_API AlmondCore {};