
#include "EntryPoint.h"
#include "EntryPoint_Win32.h"

#ifdef _WIN32
#ifndef _CONSOLE

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    almond::Win32EntryPoint entryPoint;
    entryPoint.createEntryPoint(1280, 720, L"Almond Shell - Windows");

    // Register a key callback function
    entryPoint.setKeyCallback([](WPARAM key) {
        // Simple key event handling: print key value to debug output
        if (key == VK_ESCAPE) {
            PostQuitMessage(0);  // Quit if ESC is pressed
        }
        });

    // Main game loop
    bool running = true;
    while (running) {
        // Poll and handle window events
        running = entryPoint.pollEvents();  // Will return false when WM_QUIT is posted

        // Your rendering and game logic goes here...
        entryPoint.show();  // Just showing the window for now
    }

    return 0;
}

#endif
#endif // _WIN32
