#pragma once

#ifdef _WIN32
// ENTRYPOINT_STATICLIB should be defined in both Almond build and user application 
// to use static linking
#ifndef ENTRYPOINT_STATICLIB
#ifdef ENTRYPOINT_DLL_EXPORTS
#define ENTRYPOINTLIBRARY_API __declspec(dllexport)
#else
#define ENTRYPOINTLIBRARY_API __declspec(dllimport)
#endif
#else
#define ENTRYPOINTLIBRARY_API  // Empty for static library usage
#endif
#else
// GCC visibility for Unix-like platforms
#if (__GNUC__ >= 4) && !defined(ENTRYPOINT_STATICLIB) && defined(ENTRYPOINT_DLL_EXPORTS)
#define ENTRYPOINTLIBRARY_API __attribute__((visibility("default")))
#else
#define ENTRYPOINTLIBRARY_API  // Empty for static library usage
#endif
#endif

// Define calling conventions based on platform compatibility
#if defined(_STDCALL_SUPPORTED)
#define ALECALLCONV __stdcall
#else
#define ALECALLCONV __cdecl
#endif  // _STDCALL_SUPPORTED
