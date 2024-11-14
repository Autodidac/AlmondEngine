// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

// The DLL entry point function (if needed)
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
       // Initialize(); // Call the initialization function when the DLL is loaded
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        //Cleanup();    // Cleanup when the DLL is unloaded
        break;
    }
    return TRUE;
}
