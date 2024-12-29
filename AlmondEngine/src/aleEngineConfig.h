#pragma once

#ifdef _WIN32
// include windows TODO: platform macros
#include "framework.h"
#endif

// --------------------
// Engine Context Config
// --------------------

// Rendering backend (Uncomment ONLY ONE)
//
//#define ALMOND_USING_VULKAN  // Currently Not Supported In AlmondShell, See AlmondEngine...
//#define ALMOND_USING_DIRECTX  // Currently Not Supported In AlmondShell, See AlmondEngine...

// Windowing backend (Uncomment ONLY ONE)
#define ALMOND_USING_GLFW
//#define ALMOND_USING_SDL

// --------------------
// Include Libraries
// --------------------
#ifdef ALMOND_USING_GLFW
#define ALMOND_USING_OPENGL
#define ALMOND_USING_OPENGLTEXTURE
#include <glad/glad.h>
//#include <glu/glu.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#endif

#ifdef ALMOND_USING_SDL
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#endif

// --------------------
// Compile-Time Guards
// --------------------
//#if defined(ALMOND_USING_OPENGL) + defined(ALMOND_USING_VULKAN) + defined(ALMOND_USING_DIRECTX) != 1
//#error "Define exactly one rendering backend (e.g., ALMOND_USING_OPENGL)."
//#endif
// --------------------
//#if defined(ALMOND_USING_SDL) + defined(ALMOND_USING_GLFW) + defined(ALMOND_USING_OPENGL) != 1
//#error "Define exactly one windowing backend (e.g., ALMOND_USING_SDL)."
//#endif
