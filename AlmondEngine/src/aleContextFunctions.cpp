#include "aleContextFunctions.h"

#include "aleGLFWContext.h"
#include "aleSDL3Context.h"

    // Include other context headers as needed
    ContextFuncs createContextFuncs(const std::string& type) {
#ifdef ALMOND_USING_GLFW
        if (type == "GLFW" || type == "glfw") {
            return { almond::initGLFW, almond::processGLFW, almond::cleanupGLFW };
        }
#endif
#ifdef ALMOND_USING_SDL
        if (type == "SDL3" || type == "sdl3") {
            return { almond::initSDL3, almond::processSDL3, almond::cleanupSDL3 };
        }
#endif
        // Add cases for other contexts
        else {
            throw std::invalid_argument("Unsupported context type");
        }
    }
