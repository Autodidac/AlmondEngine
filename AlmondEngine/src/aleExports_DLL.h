#pragma once

#include <string>

// API Visibility Macros
#ifdef _WIN32
    // Windows DLL export/import logic
    #ifndef ALMONDENGINE_STATICLIB
        #ifdef ALMONDENGINE_DLL_EXPORTS
            #define ALMONDENGINE_API __declspec(dllexport)
        #else
            #define ALMONDENGINE_API __declspec(dllimport)
        #endif
    #else
        #define ALMONDENGINE_API  // Static library, no special attributes
    #endif
#else
    // GCC/Clang visibility attributes
    #if (__GNUC__ >= 4) && !defined(ALMONDENGINE_STATICLIB) && defined(ALMONDENGINE_DLL_EXPORTS)
        #define ALMONDENGINE_API __attribute__((visibility("default")))
    #else
        #define ALMONDENGINE_API  // Static library or unsupported compiler
    #endif
#endif

// Calling Convention Macros
#ifndef _STDCALL_SUPPORTED
    #define ALECALLCONV __cdecl
#else
    #define ALECALLCONV __stdcall
#endif
