#pragma once

#include "EntryPoint_Crossplatform.h"
#include "Utilities.h"
#include "Exports_DLL.h"

#include <functional>
#include <iostream>

#ifdef _WIN32
#ifndef _CONSOLE
#include "framework.h"

#ifdef _MSC_VER
// Only for MSVC compilers
#pragma warning(disable : 4251)
#endif

namespace almond {

    class Win32EntryPoint : public EntryPoint_Crossplatform {
    public:
        using KeyCallback = std::function<void(WPARAM)>;  // Define a callback type for key input

        Win32EntryPoint() : hwnd(nullptr), keyCallback(nullptr) {}

        // Method to create a window with the given parameters
        void createEntryPoint(int width, int height, const wchar_t* title) override {
            // Define the window class
            WNDCLASS wc = {};
            wc.lpfnWndProc = Win32EntryPoint::WndProc;  // Custom window procedure
            wc.hInstance = GetModuleHandle(nullptr);
            wc.lpszClassName = L"AlmondEntryPointClass";
            wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
            wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
            wc.style = CS_HREDRAW | CS_VREDRAW;

            if (!RegisterClass(&wc)) {
                std::cerr << "Failed to register window class: " << GetLastError() << std::endl;
                return;
            }

            // Create the window
            hwnd = CreateWindowEx(0, wc.lpszClassName, title, WS_OVERLAPPEDWINDOW,
                CW_USEDEFAULT, CW_USEDEFAULT, width, height,
                nullptr, nullptr, wc.hInstance, this);

            if (!hwnd) {
                std::cerr << "Window creation failed: " << GetLastError() << std::endl;
            }
        }

        // Show the window
        void show() override {
            ShowWindow(hwnd, SW_SHOW);
        }

        // Poll events (process Windows messages)
        bool pollEvents() override {
            MSG msg;
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                // Handle quit message (optional, for exiting loop cleanly)
                if (msg.message == WM_QUIT) {
                    return false;
                }
            }
            return true;
        }

        // Set the callback for key press events
        void setKeyCallback(KeyCallback callback) {
            keyCallback = std::move(callback);
        }

    private:
        HWND hwnd;
        KeyCallback keyCallback;  // Store the input callback

        // Custom window procedure to handle messages
        static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
            Win32EntryPoint* entryPoint = nullptr;

            if (uMsg == WM_CREATE) {
                // Store a pointer to the window's Win32EntryPoint instance
                CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
                entryPoint = reinterpret_cast<Win32EntryPoint*>(pCreate->lpCreateParams);
                SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)entryPoint);
            }
            else {
                // Retrieve the Win32EntryPoint instance
                entryPoint = reinterpret_cast<Win32EntryPoint*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            }

            if (entryPoint) {
                // Handle key press events
                switch (uMsg) {
                case WM_KEYDOWN:
                    if (entryPoint->keyCallback) {
                        entryPoint->keyCallback(wParam);  // Call the input callback
                    }
                    break;

                case WM_DESTROY:
                    PostQuitMessage(0);
                    break;
                }
            }

            // Default window procedure
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    };

} // namespace almond
#endif
#endif // _WIN32
