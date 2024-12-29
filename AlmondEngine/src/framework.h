#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

/*
* this was for raylib, direct calls to winapi commands to avoid namespace pollution
* it was abandoned because raylib directly conflicts with int main the heart of the library,
  having it's own internal main and me wanting out of the box easy solutions
*/
/*
//#define NOMINMAX
//#define NOGDI

#define _WIN64

//just define essential types
typedef void* HMODULE;
typedef unsigned long DWORD;
typedef const char* LPCSTR;
typedef unsigned long HRESULT;
#define S_OK ((HRESULT)0L)

#ifndef NULL
#define NULL 0
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

typedef struct tagMSG {
    void* hwnd;        // Handle to the window
    unsigned int message; // Message identifier
    unsigned int wParam;  // Additional message information
    long lParam;          // Additional message information
    unsigned int time;    // Timestamp
    long pt_x;            // X-coordinate of the cursor position
    long pt_y;            // Y-coordinate of the cursor position
} MSG;
*/
#endif // _WIN32

