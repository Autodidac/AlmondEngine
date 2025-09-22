/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
// aengine.cpp
#include "pch.h"

#include "aengineconfig.hpp"   // All ENGINE-specific includes

#include "aengine.hpp"
#include "acontext.hpp"
#include "acontextmultiplexer.hpp"
#include "acontextwindow.hpp"
#include "aplatformpump.hpp"
#include "aversion.hpp"
#include "aguimenu.hpp"
#include "aapplicationmodule.hpp"
#include "aenduserapplication.hpp"
#include "acommandline.hpp"
#include "awindowdata.hpp"

// Code Analysis
#include "acodeinspector.hpp"

// games
#include "a2048like.hpp"
#include "acellularsim.hpp"
#include "afroggerlike.hpp"
#include "amatch3like.hpp"
#include "aminesweeperlike.hpp"
#include "apacmanlike.hpp"
#include "aslidingpuzzlelike.hpp"
#include "asnakelike.hpp"
#include "asokobanlike.hpp"
#include "atetrislike.hpp"
#include "asandsim.hpp"

#include "araylibcontext.hpp"
#include "asdlcontext.hpp"
#include "asfmlcontext.hpp"

// STL
#include <filesystem>
#include <atomic>
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <queue>
#include <mutex>
#include <optional>
#include <memory>
#include <unordered_map>
#include <map>

//#if !defined(ALMOND_SINGLE_PARENT) || (ALMOND_SINGLE_PARENT == 0)
//#undef ALMOND_SHARED_CONTEXT
//#define ALMOND_SHARED_CONTEXT 0
//#else
//#define ALMOND_USING_DOCKING 1  // Enable docking features
//#define ALMOND_SHARED_CONTEXT 1
//#endif

#ifndef ALMOND_MAIN_HEADLESS
#include <shellscalingapi.h>  // For GetDpiForMonitor
#pragma comment(lib, "Shcore.lib")
#include <CommCtrl.h>
#pragma comment(lib, "comctl32.lib")
#endif // !ALMOND_MAIN_HEADLESS

namespace cli = almondnamespace::core::cli;

namespace almondnamespace::core
{
    class MultiContextManager; // Forward declaration
    //std::shared_ptr<core::Context> MultiContextManager::currentContext = nullptr;
	//std::shared_ptr<core::Context> MultiContextManager::sharedContext = nullptr;
    
    // ─── 1) Declare a global ContextPtr somewhere visible to WinMain & WndProc ───
   // inline std::shared_ptr<almondnamespace::Context> g_ctx;

    //std::filesystem::path myexepath;

    //void printEngineInfo() {
    //    std::cout << almondnamespace::GetEngineName() << " v" 
    //              << almondnamespace::GetEngineVersion() << '\n';
    //}

    //void handleCommandLine(int argc, char* argv[]) {
    //    std::cout << "Command-line arguments for: ";
    //    for (int i = 0; i < argc; ++i) {
    //        if (i > 0) {
    //            std::cout << "  [" << i << "]: " << argv[i] << '\n';
    //        }
    //        else {
    //            myexepath = argv[i];
    //            std::string filename = myexepath.filename().string();
    //            std::cout << filename << '\n';
    //        }
    //    }
    //    std::cout << '\n'; // End arguments with a new line before printing anything else

    //    // Handle specific commands
    //    for (int i = 1; i < argc; ++i) {
    //        std::string arg = argv[i];

    //        if (arg == "--help" || arg == "-h") {
    //            std::cout << "Available commands:\n"
    //                << "  --help, -h            Show this help message\n"
    //                << "  --version, -v         Display the engine version\n"
    //                << "  --fullscreen          Run the engine in fullscreen mode\n"
    //                << "  --windowed            Run the engine in windowed mode\n"
    //                << "  --width [value]       Set the window width (e.g., --width 1920)\n"
    //                << "  --height [value]      Set the window height (e.g., --height 1080)\n";
    //        }
    //        else if (arg == "--version" || arg == "-v") {
    //            printEngineInfo();
    //        }
    //        else if (arg == "--fullscreen") {
    //            std::cout << "Running in fullscreen mode.\n";
    //            // Set fullscreen mode flag
    //        }
    //        else if (arg == "--windowed") {
    //            std::cout << "Running in windowed mode.\n";
    //            // Set windowed mode flag
    //        }
    //        else if (arg == "--width" && i + 1 < argc) {
    //            almondnamespace::core::cli::window_width = std::stoi(argv[++i]);
    //            std::cout << "Window width set to: " << almondnamespace::core::cli::window_width << '\n';
    //        }
    //        else if (arg == "--height" && i + 1 < argc) {
    //            almondnamespace::core::cli::window_height = std::stoi(argv[++i]);
    //            std::cout << "Window height set to: " << almondnamespace::core::cli::window_height << '\n';
    //        }
    //        else {
    //            std::cerr << "Unknown argument: " << arg << '\n';
    //        }
    //    }
    //}

    void RunEngine()
    {
        using namespace almondnamespace;

#if defined(RUN_CODE_INSPECTOR)
        codeinspector::LineStats totalStats{};
        auto results = codeinspector::inspect_directory(
            "C:/Users/iammi/source/repos/NewEngine/AlmondShell/", totalStats);
        for (const auto& res : results) {
            std::cout << "File: \"" << res.filePath.string() << "\"\n";
            for (const auto& issue : res.issues)
                std::cout << "  - " << issue << '\n';
            std::cout << '\n';
        }
        std::cout << "=== Combined Totals Across All Files ===\n";
        for (const auto& line : totalStats.format_summary())
            std::cout << line << '\n';
#endif


    }

	// ─── 2) Start the engine in WinMain or main() ──────────────────────────────
    static void StartEngine() {

		// Print engine version
		std::cout << "Almond Engine v" << almondnamespace::GetEngineVersion() << '\n';

       // bool multicontext = false;// cli::multi_context;
        //int choice;

    //    while (true) {
    //        std::cout << "1. Single\n"
    //            << "2. Multi\n"
    //            << "> ";

    //        std::cin >> choice;

    //        if (std::cin.fail()) {
    //            // Handle non-integer input
    //            std::cin.clear();
    //            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    //            std::cout << "Invalid input. Please enter a number.\n";
    //            continue;
    //        }

    //        if (choice == 1) {
    //            std::cout << "Selected: Single\n";
				//multicontext = false; // Single context mode
    //            break;
    //        }
    //        else if (choice == 2) {
    //            std::cout << "Selected: Multi\n";
				//multicontext = true; // Multi context mode
    //            break;
    //        }
    //        else {
    //            std::cout << "Invalid choice. Please enter 1 or 2.\n";
    //        }
    //    }

    //    if (multicontext == true)
    //    {

        //}
      //  else
        //{
            // Main engine loop - single context
            RunEngine();
        //}
    }

#ifdef ALMOND_USING_WINMAIN
    //struct Window {
    //    HWND hwnd{};
    //    HDC hdc{};
    //    HGLRC context{};
    //   // GLuint texture{};
    //    bool running{ true };
    //    Window() = default;
    //    Window(const Window&) = delete;
    //    Window& operator=(const Window&) = delete;
    //    Window(Window&&) = default;
    //    Window& operator=(Window&&) = default;
    //};
    //struct App {
    //    HWND parent{};
    //    HWND child{};
    //    HDC parentDC{};
    //    HDC childDC{};
    //    HGLRC sharedContext{};
    //    bool running{ true };
    //    std::vector<std::unique_ptr<Window>> dockedWindows;
    //    std::vector<std::unique_ptr<Window>> floatingWindows;
    //};

    //inline std::atomic<bool> g_windowRunning{ true };
    //inline App g_app;

    struct TextureUploadTask
    {
        //GLuint texture;
        int w, h;
        const void* pixels;
    };

    struct TextureUploadQueue
    {
        std::queue<TextureUploadTask> tasks;
        std::mutex mtx;
    public:
        void push(TextureUploadTask&& task) {
            std::lock_guard lock(mtx);
            tasks.push(std::move(task));
        }
        std::optional<TextureUploadTask> try_pop() {
            std::lock_guard lock(mtx);
            if (tasks.empty()) return {};
            auto task = tasks.front();
            tasks.pop();
            return task;
        }
    };

    inline std::vector<std::unique_ptr<TextureUploadQueue>> uploadQueues;

    // --- Docking logic ---
    //struct DragState
    //{
    //    bool dragging = false;
    //    POINT clickPos{};
    //};
   // inline std::unordered_map<HWND, DragState> dragStates;

    //void DetachWindow(HWND hwndChild) {
    //    // 1. Get child rect relative to parent BEFORE messing with styles
    //    RECT childRect;
    //    GetWindowRect(hwndChild, &childRect);
    //    HWND hwndParent = GetParent(hwndChild);
    //    if (hwndParent) {
    //        POINT tl = { childRect.left, childRect.top };
    //        POINT br = { childRect.right, childRect.bottom };
    //        ScreenToClient(hwndParent, &tl);
    //        ScreenToClient(hwndParent, &br);
    //        RECT clientRelative = { tl.x, tl.y, br.x, br.y };

    //        // 2. Blackout the child window in its own DC
    //        HDC hdcChild = GetDC(hwndChild);
    //        RECT localRect;
    //        GetClientRect(hwndChild, &localRect);
    //       // HBRUSH blackBrush = (HBRUSH)GetStockObject(BLACK_BRUSH);
    //       // FillRect(hdcChild, &localRect, blackBrush);
    //        ReleaseDC(hwndChild, hdcChild);

    //        // 3. Immediately invalidate the region on the parent
    //        InvalidateRect(hwndParent, &clientRelative, TRUE);
    //    }

    //    // 4. Hide & reparent
    //    ShowWindow(hwndChild, SW_HIDE);


    //    LONG_PTR style = GetWindowLongPtr(hwndChild, GWL_STYLE);
    //    style &= ~WS_CHILD;
    //    style |= WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    //    SetWindowLongPtr(hwndChild, GWL_STYLE, style);
    //    SetParent(hwndChild, nullptr);

    //    // Remove from docked list:
    //    auto& docked = almondnamespace::core::g_app.dockedWindows;
    //    auto it = std::find_if(docked.begin(), docked.end(),
    //        [hwndChild](const auto& win) { return win->hwnd == hwndChild; });
    //    if (it != docked.end()) {
    //        almondnamespace::core::g_app.floatingWindows.push_back(std::move(*it));
    //        docked.erase(it);
    //    }

    //    // 5. Drop it on the grid
    //    POINT cursorPos;
    //    GetCursorPos(&cursorPos);
    //    constexpr int width = 400, height = 300, gridSize = 50;
    //    int x = (cursorPos.x / gridSize) * gridSize;
    //    int y = (cursorPos.y / gridSize) * gridSize;
    //    SetWindowPos(hwndChild, HWND_TOP, x, y, width, height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    //    // 6. Force parent repaint
    //    if (hwndParent) {
    //        UpdateWindow(hwndParent);
    //    }
    //}

    //void TryDockWindow(HWND hwndChild, HWND hwndParent) {

    //    // Remove from floating:
    //    auto& floating = almondnamespace::core::g_app.floatingWindows;
    //    auto it = std::find_if(floating.begin(), floating.end(),
    //        [hwndChild](const auto& win) { return win->hwnd == hwndChild; });
    //    if (it != floating.end()) {
    //        almondnamespace::core::g_app.dockedWindows.push_back(std::move(*it));
    //        floating.erase(it);
    //    }
    //    POINT cursor;
    //    GetCursorPos(&cursor);
    //    RECT parentRect;
    //    GetWindowRect(hwndParent, &parentRect);
    //    if (PtInRect(&parentRect, cursor)) {
    //        LONG_PTR style = GetWindowLongPtr(hwndChild, GWL_STYLE);
    //        style &= ~WS_OVERLAPPEDWINDOW;
    //        style |= WS_CHILD | WS_VISIBLE;
    //        SetWindowLongPtr(hwndChild, GWL_STYLE, style);
    //        SetParent(hwndChild, hwndParent);
    //        SetWindowPos(hwndChild, nullptr, 0, 0, 400, 300, SWP_FRAMECHANGED | SWP_SHOWWINDOW);


    //        // Trigger rearrangement of docked windows
    //        RECT rc;
    //        GetClientRect(hwndParent, &rc);
    //        PostMessage(hwndParent, WM_SIZE, 0, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
    //    }
    //}

    //inline void RefreshDockLayout() {
    //    if (g_app.parent) {
    //        RECT rc;
    //        GetClientRect(g_app.parent, &rc);
    //        PostMessage(g_app.parent, WM_SIZE, 0, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
    //    }
    //}

    void PromoteToTopLevel(HWND hwnd) {
        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);

        style &= ~WS_CHILD;                // Remove child style
        style |= WS_OVERLAPPEDWINDOW;     // Add top-level window styles

        SetWindowLongPtr(hwnd, GWL_STYLE, style);
        SetParent(hwnd, nullptr);

        SetWindowPos(hwnd, HWND_TOP, 100, 100, 400, 300,
            SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        ShowWindow(hwnd, SW_SHOW);
    }
    // --- Window procs ---
    //LRESULT CALLBACK ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //    switch (uMsg) {
    //    case WM_SIZE: {
    //        int w = LOWORD(lParam), h = HIWORD(lParam);
    //        int numWindows = static_cast<int>(g_app.dockedWindows.size());
    //        int cols = 1, rows = 1;
    //        while (cols * rows < numWindows) {
    //            if (cols <= rows) ++cols; else ++rows;
    //        }
    //        int cw = w / cols;
    //        int ch = h / rows;
    //        for (int i = 0; i < numWindows; ++i) {
    //            int col = i % cols;
    //            int row = i / cols;
    //            auto& win = g_app.dockedWindows[i];
    //            LONG_PTR style = GetWindowLongPtr(win->hwnd, GWL_STYLE);
    //            if (style & WS_CHILD) {
    //                SetWindowPos(win->hwnd, nullptr, col * cw, row * ch, cw, ch,
    //                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    //            }
    //        }
    //        return 0;
    //    }

    //    case WM_PAINT: {
    //        PAINTSTRUCT ps;
    //        HDC hdc = BeginPaint(hwnd, &ps);

    //        // Fill the whole client area with a dark brush
    //        HBRUSH brush = CreateSolidBrush(RGB(20, 20, 20));  // Or GetStockObject(BLACK_BRUSH)
    //        FillRect(hdc, &ps.rcPaint, brush);
    //        DeleteObject(brush);

    //        EndPaint(hwnd, &ps);
    //        return 0;
    //    }

    //    case WM_CLOSE:
    //        // g_windowRunning = false;
    //        DestroyWindow(hwnd);
    //        return 0;

    //    case WM_DESTROY:
    //        PostQuitMessage(0);
    //        return 0;
    //    }

    //    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    //}

    //LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //    switch (uMsg) {
    //    case WM_LBUTTONDOWN: {
    //        SetCapture(hwnd);
    //        POINTS pts = MAKEPOINTS(lParam);
    //        dragStates[hwnd] = DragState{ false, {pts.x, pts.y} };
    //        return 0;
    //    }

    //    case WM_MOUSEMOVE: {
    //        if (GetCapture() == hwnd) {
    //            POINTS pts = MAKEPOINTS(lParam);
    //            DragState& state = dragStates[hwnd];
    //            if (!state.dragging) {
    //                int dx = pts.x - state.lastMousePos.x;
    //                int dy = pts.y - state.lastMousePos.y;
    //                if ((dx * dx + dy * dy) > 9) { // threshold 3 pixels squared
    //                    PromoteToTopLevel(hwnd);
    //                    DetachWindow(hwnd);
    //                    RefreshDockLayout();

    //                    state.dragging = true;
    //                }
    //            }

    //            if (state.dragging) {
    //                POINT cursorPos;
    //                GetCursorPos(&cursorPos);
    //                // Offset the window so mouse grabs near top-left corner
    //                SetWindowPos(hwnd, nullptr, cursorPos.x - 50, cursorPos.y - 15, 0, 0,
    //                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    //            }
    //        }
    //        return 0;
    //    }
    //    case WM_LBUTTONUP: {
    //        if (GetCapture() == hwnd) {
    //            ReleaseCapture();
    //            DragState& state = dragStates[hwnd];
    //            if (state.dragging && g_app.parent) {
    //                TryDockWindow(hwnd, g_app.parent);
    //                RefreshDockLayout();
    //                state.dragging = false;
    //            }
    //        }
    //        return 0;
    //    }
    //    case WM_CLOSE:

    //        DestroyWindow(hwnd);
    //        return 0;
    //    case WM_DESTROY:
    //        if (hwnd == g_app.parent) {
    //            // Only post quit message if main window is destroyed
    //            // This allows child windows to be closed without quitting the app
    //            PostQuitMessage(0);
    //        }
    //        return 0;
    //    }
    //    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    //}

    //// --- Helper funcs ---
    //HWND create_invisible_dummy_window(HINSTANCE hi, const wchar_t* className) {
    //    DWORD exStyle = WS_EX_TOOLWINDOW; // Prevent taskbar icon
    //    DWORD style = WS_POPUP;            // No border, no titlebar

    //    HWND hwnd = CreateWindowEx(
    //        exStyle,
    //        className,
    //        L"Dummy",
    //        style,
    //        CW_USEDEFAULT, CW_USEDEFAULT, 1, 1,
    //        nullptr, nullptr, hi, nullptr);

    //    if (!hwnd) {
    //        std::cerr << "Failed to create invisible dummy window\n";
    //        return nullptr;
    //    }

    //    // Do NOT ShowWindow(hwnd, ...) or just SW_HIDE to be safe
    //    ShowWindow(hwnd, SW_HIDE);

    //    return hwnd;
    //}

    //bool register_classes(HINSTANCE hi, const wchar_t* parentName, const wchar_t* windowName) {
    //    WNDCLASS wc{};
    //    wc.hInstance = hi;
    //    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

    //    wc.lpfnWndProc = MultiContextManager::ParentProc;
    //    wc.lpszClassName = parentName;
    //    if (!RegisterClass(&wc)) {
    //        std::cerr << "Failed to register parent window class\n";
    //        return false;
    //    }

    //    wc.lpfnWndProc = MultiContextManager::ChildProc;
    //    wc.lpszClassName = windowName;
    //    if (!RegisterClass(&wc)) {
    //        std::cerr << "Failed to register child window class\n";
    //        return false;
    //    }
    //    return true;
    //}

    //HWND create_window(HINSTANCE hi, int x, int y, int w, int h, const wchar_t* cname, const wchar_t* title, HWND parent) {
    //    DWORD style = (parent) ? (WS_CHILD | WS_VISIBLE | WS_BORDER) : (WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    //    HWND hwnd = CreateWindowEx(0, cname, title, style, x, y, w, h, parent, nullptr, hi, nullptr);
    //    if (!hwnd) {
    //        std::wcerr << L"Failed to create window: " << title << L"\n";
    //        return nullptr;
    //    }
    //    ShowWindow(hwnd, SW_SHOW);
    //    UpdateWindow(hwnd);
    //    return hwnd;
    //}

    //HGLRC make_context(HDC hdc, HGLRC shared = nullptr) {
    //    PIXELFORMATDESCRIPTOR pfd{};
    //    pfd.nSize = sizeof pfd;
    //    pfd.nVersion = 1;
    //    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    //    pfd.iPixelType = PFD_TYPE_RGBA;
    //    pfd.cColorBits = 32;
    //    pfd.cDepthBits = 24;
    //    pfd.iLayerType = PFD_MAIN_PLANE;

    //    int pf = ChoosePixelFormat(hdc, &pfd);
    //    if (!pf) return nullptr;
    //    if (!SetPixelFormat(hdc, pf, &pfd)) return nullptr;

    //    HGLRC ctx = wglCreateContext(hdc);
    //    if (ctx && shared) {
    //        if (!wglShareLists(shared, ctx)) {
    //            std::cerr << "wglShareLists failed\n";
    //        }
    //    }
    //    return ctx;
    //}

    //// --- Window procs ---
    //LRESULT CALLBACK ParentProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //    switch (uMsg) {
    //   case WM_SIZE:
    //    {
    //        //HWND hwndRaylib = /* retrieve from your context */;
    //        //RECT rc;
    //        //GetClientRect(hwnd, &rc);
    //        //SetWindowPos(hwnd, nullptr, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top,
    //        //    SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

    //    //           POINT cursor;
    //    //GetCursorPos(&cursor);

    //    //RECT parentRect;
    //    //GetWindowRect(hwndParent, &parentRect);
    //    ////if (PtInRect(&parentRect, cursor)) {
    //    //LONG_PTR style = GetWindowLongPtr(hwndChild, GWL_STYLE);
    //    //style &= ~WS_OVERLAPPED;
    //    //style |= WS_CHILD | WS_VISIBLE;
    //    //SetWindowLongPtr(hwndChild, GWL_STYLE, style);
    //    //SetParent(hwndChild, hwndParent);
    //    //SetWindowPos(hwndChild, nullptr, 0, 0, core::cli::window_width, core::cli::window_height, SWP_FRAMECHANGED | SWP_SHOWWINDOW);

    //    //RECT rc;
    //    //GetClientRect(hwndParent, &rc);
    //    //PostMessage(hwndParent, WM_SIZE, 0, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));


    //       int w = LOWORD(lParam), h = HIWORD(lParam);
    //       int numWindows = static_cast<int>(g_app.dockedWindows.size());
    //       int cols = 1, rows = 1;
    //       while (cols * rows < numWindows) {
    //           if (cols <= rows) ++cols; else ++rows;
    //       }
    //       int cw = w / cols;
    //       int ch = h / rows;
    //       for (int i = 0; i < numWindows; ++i) {
    //           int col = i % cols;
    //           int row = i / cols;
    //           auto& win = g_app.dockedWindows[i];
    //           LONG_PTR style = GetWindowLongPtr(win->hwnd, GWL_STYLE);
    //           if (style & WS_CHILD) {
    //               SetWindowPos(win->hwnd, nullptr, col * cw, row * ch, cw, ch,
    //                   SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
    //           }
    //       }
    //        return 0;
    //    }

    //    case WM_PAINT: {
    //        //PAINTSTRUCT ps;
    //        //HDC hdc = BeginPaint(hwnd, &ps);

    //        //// Fill the whole client area with a dark brush
    //        //HBRUSH brush = CreateSolidBrush(RGB(20, 20, 20));  // Or GetStockObject(BLACK_BRUSH)
    //        //FillRect(hdc, &ps.rcPaint, brush);
    //        //DeleteObject(brush);

    //        //EndPaint(hwnd, &ps);
    //        return 0;
    //    }

    //    case WM_CLOSE:
    //        // g_windowRunning = false;
    //        DestroyWindow(hwnd);
    //        return 0;

    //    case WM_DESTROY:
    //        PostQuitMessage(0);
    //        return 0;
    //    }

    //    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    //}

    //LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    //    switch (uMsg) {
    //    case WM_LBUTTONDOWN: {
    //        SetCapture(hwnd);
    //        POINTS pts = MAKEPOINTS(lParam);
    //        dragStates[hwnd] = DragState{ false, {pts.x, pts.y} };
    //        return 0;
    //    }

    //    case WM_MOUSEMOVE: {
    //        if (GetCapture() == hwnd) {
    //            POINTS pts = MAKEPOINTS(lParam);
    //            DragState& state = dragStates[hwnd];
    //            if (!state.dragging) {
    //                int dx = pts.x - state.clickPos.x;
    //                int dy = pts.y - state.clickPos.y;
    //                if ((dx * dx + dy * dy) > 9) { // threshold 3 pixels squared
    //                    PromoteToTopLevel(hwnd);
    //                    DetachWindow(hwnd);
    //                    RefreshDockLayout();

    //                    state.dragging = true;
    //                }
    //            }

    //            if (state.dragging) {
    //                POINT cursorPos;
    //                GetCursorPos(&cursorPos);
    //                // Offset the window so mouse grabs near top-left corner
    //                SetWindowPos(hwnd, nullptr, cursorPos.x - 50, cursorPos.y - 15, 0, 0,
    //                    SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    //            }
    //        }
    //        return 0;
    //    }
    //    case WM_LBUTTONUP: {
    //        if (GetCapture() == hwnd) {
    //            ReleaseCapture();
    //            DragState& state = dragStates[hwnd];
    //            if (state.dragging && g_app.parent) {
    //                TryDockWindow(hwnd, g_app.parent);
    //                RefreshDockLayout();
    //                state.dragging = false;
    //            }
    //        }
    //        return 0;
    //    }
    //    case WM_CLOSE:

    //        DestroyWindow(hwnd);
    //        return 0;
    //    case WM_DESTROY:
    //        if (hwnd == g_app.parent) {
    //            // Only post quit message if main window is destroyed
    //            // This allows child windows to be closed without quitting the app
    //            PostQuitMessage(0);
    //        }
    //        return 0;
    //    }
    //    return DefWindowProc(hwnd, uMsg, wParam, lParam);
    //}

//bool register_classes(HINSTANCE hi, const wchar_t* parentName, const wchar_t* windowName) {
//    WNDCLASSW wc = {};
//    wc.hInstance = hi;
//    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
//
//    wc.lpfnWndProc = ParentProc;
//    wc.lpszClassName = parentName;
//    if (!RegisterClassW(&wc)) {
//        std::cerr << "Failed to register parent window class\n";
//        return false;
//    }
//
//    wc.lpfnWndProc = WindowProc;
//    wc.lpszClassName = windowName;
//    if (!RegisterClassW(&wc)) {
//        std::cerr << "Failed to register child window class\n";
//        return false;
//    }
//    return true;
//}


    // Function to show the console in debug mode
// ShowConsole() in your core namespace
    void ShowConsole() {
#ifdef _DEBUG       // “if I’m in a Debug build”
        AllocConsole();
        FILE* fp = nullptr;
        freopen_s(&fp, "CONIN$", "r", stdin);
        freopen_s(&fp, "CONOUT$", "w", stdout);
        freopen_s(&fp, "CONOUT$", "w", stderr);
#else
        FreeConsole(); // if you somehow still have one
#endif
    }

    //HWND create_window(HINSTANCE hInstance, int x, int y, int w, int h, const wchar_t* cname, const wchar_t* title, HWND parent) {
    //    DWORD style = (parent) ? (WS_CHILD | WS_VISIBLE | WS_BORDER | WS_CLIPSIBLINGS | WS_CLIPCHILDREN) : (WS_OVERLAPPEDWINDOW | WS_VISIBLE);
    //    HWND hwnd = CreateWindowEx(0, cname, title, style, x, y, w, h, parent, nullptr, hInstance, nullptr);
    //    // if (!hwnd) {
    //    //     std::wcerr << L"Failed to create window: " << title << L"\n";
    //    //     return nullptr;
    //    // }
    //    ShowWindow(hwnd, SW_SHOW);
    //    UpdateWindow(hwnd);
    //    return hwnd;
    //}
}

// Tell the linker which subsystem to use:
#ifdef NDEBUG     // in Release
  #pragma comment(linker, "/SUBSYSTEM:WINDOWS")
#else              // in Debug
  #pragma comment(linker, "/SUBSYSTEM:CONSOLE")
#endif

    // Define global variables here
WCHAR szTitle[MAX_LOADSTRING] = L"Automatic WinMain Example";
WCHAR szChildTitle[MAX_LOADSTRING] = L"Automatic WinMain With Child Example";
WCHAR szWindowClass[MAX_LOADSTRING] = L"SampleWindowClass";

almondnamespace::contextwindow::WindowData windowContext{};






////////////  multi context manager ///////////////////
//
//
//    // -----------------------------------------------------------------
//    // Global thread table (keeps WindowData copyable)
//    // -----------------------------------------------------------------
//static std::unordered_map<HWND, std::thread> gThreads;
//
//// -----------------------------------------------------------------
//// Forward-declare helper so AddWindow can see it
//// -----------------------------------------------------------------
//void MakeDockable(HWND hwnd, HWND parent);
//
//struct DragState {
//    bool dragging = false;
//    POINT lastMousePos{};
//    HWND draggedWindow = nullptr;
//    HWND originalParent = nullptr;
//};
//static DragState gDragState;
//
//// -----------------------------------------------------------------
//// MultiContextManager - manages multiple OpenGL contexts with shared glcontext support
//// also manages docking of child windows into a parent window
//// as well as handling drag-and-drop file operations
//// as well as resizing and rendering, input events, window creation and destruction
//// as well as handling console output, pixel format setup, and other context-related creation and operations
//// -----------------------------------------------------------------
//class MultiContextManager
//{
//public:
//    using ResizeCallback = std::function<void(int, int)>;
//
//    struct MultiContextWindowData {
//        HWND hwnd{};
//        HDC  hdc{};
//        HGLRC glContext{};
//        bool usesSharedContext = false;
//        bool running = true;
//        bool clicked = false;
//        ResizeCallback onResize;
//
//        MultiContextWindowData() = default;
//        MultiContextWindowData(HWND h, HDC dc, HGLRC ctx, bool shared)
//            : hwnd(h), hdc(dc), glContext(ctx), usesSharedContext(shared) {
//        }
//        MultiContextWindowData(const MultiContextWindowData&) = delete;
//        MultiContextWindowData& operator=(const MultiContextWindowData&) = delete;
//        MultiContextWindowData(MultiContextWindowData&&) noexcept = default;
//        MultiContextWindowData& operator=(MultiContextWindowData&&) = default;
//    };
//
//private:
//    std::vector<MultiContextWindowData> windows;
//    HGLRC sharedContext = nullptr;
//    HWND  parent = nullptr;
//    std::atomic<bool> running{ true };
//
//public:
//    static void ShowConsole();
//
//    bool Initialize(HINSTANCE hInst, int windowCount, bool parented = ALMOND_SINGLE_PARENT == 1);
//    void StartRenderThreads();
//    void StopAll();
//    bool IsRunning() const noexcept { return running.load(std::memory_order_acquire); }
//    void StopRunning() noexcept { running.store(false, std::memory_order_release); }
//
//    void AddWindow(HWND hwnd, HDC hdc, HGLRC glContext, bool usesSharedContext,
//        ResizeCallback onResize = {});
//    void RemoveWindow(HWND hwnd);
//    void ArrangeDockedWindowsGrid();
//    HWND GetParentWindow() const { return parent; }
//    void SetParentWindow(HWND hwnd) { parent = hwnd; }
//
//    const std::vector<MultiContextWindowData>& GetWindows() const { return windows; }
//
//    static LRESULT CALLBACK ParentProc(HWND, UINT, WPARAM, LPARAM);
//    static LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
//    void HandleDropFiles(HWND, HDROP);
//    void InitializeAllContexts();
//
//private:
//    void RenderLoop(MultiContextWindowData& win);
//    static ATOM RegisterParentClass(HINSTANCE, LPCWSTR);
//    static ATOM RegisterChildClass(HINSTANCE, LPCWSTR);
//    void SetupPixelFormat(HDC hdc);
//    HGLRC CreateSharedGLContext(HDC hdc);
//};
//
//// -----------------------------------------------------------------
//// Implementation
//// -----------------------------------------------------------------
//void MultiContextManager::ShowConsole() {
//    if (AllocConsole()) {
//        FILE* f;
//        freopen_s(&f, "CONOUT$", "w", stdout);
//        freopen_s(&f, "CONIN$", "r", stdin);
//        freopen_s(&f, "CONOUT$", "w", stderr);
//        std::ios::sync_with_stdio(true);
//    }
//}
//
//ATOM MultiContextManager::RegisterParentClass(HINSTANCE hInst, LPCWSTR name) {
//    WNDCLASS wc{};
//    wc.lpfnWndProc = ParentProc;
//    wc.hInstance = hInst;
//    wc.lpszClassName = name;
//    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
//    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//    return RegisterClass(&wc);
//}
//
//ATOM MultiContextManager::RegisterChildClass(HINSTANCE hInst, LPCWSTR name) {
//    WNDCLASS wc{};
//    wc.lpfnWndProc = ChildProc;
//    wc.hInstance = hInst;
//    wc.lpszClassName = name;
//    wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
//    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    return RegisterClass(&wc);
//}
//
//void MultiContextManager::SetupPixelFormat(HDC hdc) {
//    PIXELFORMATDESCRIPTOR pfd{};
//    pfd.nSize = sizeof(pfd);
//    pfd.nVersion = 1;
//    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//    pfd.iPixelType = PFD_TYPE_RGBA;
//    pfd.cColorBits = 32;
//    pfd.cDepthBits = 24;
//    pfd.iLayerType = PFD_MAIN_PLANE;
//
//    int pf = ChoosePixelFormat(hdc, &pfd);
//    if (pf == 0 || !SetPixelFormat(hdc, pf, &pfd))
//        throw std::runtime_error("Failed to set pixel format");
//}
//
//HGLRC MultiContextManager::CreateSharedGLContext(HDC hdc) {
//    SetupPixelFormat(hdc);
//    HGLRC ctx = wglCreateContext(hdc);
//    if (!ctx) throw std::runtime_error("Failed to create OpenGL context");
//    if (sharedContext && !wglShareLists(sharedContext, ctx)) {
//        wglDeleteContext(ctx);
//        throw std::runtime_error("Failed to share GL context");
//    }
//    return ctx;
//}
//
//
//bool MultiContextManager::Initialize(HINSTANCE hInst, int windowCount, bool parented) {
//    if (windowCount <= 0) return false;
//
//    RegisterParentClass(hInst, L"AlmondParent");
//    RegisterChildClass(hInst, L"AlmondChild");
//
//    int actualWidth = cli::window_width / 2;
//    int actualHeight = cli::window_height / 2;
//
//    if (parented) {
//        parent = CreateWindowEx(0, L"AlmondParent", L"Almond Docking",
//            WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//            //(GetSystemMetrics(SM_CXSCREEN) - actualWidth) / 2,  // Center based on actual size
//           // (GetSystemMetrics(SM_CYSCREEN) - actualHeight) / 2,
//            CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800,
//            nullptr, nullptr, hInst, this);
//        if (!parent) return false;
//        DragAcceptFiles(parent, TRUE);
//    }
//    else {
//        parent = nullptr;
//    }
//
//    HWND dummy = CreateWindowEx(WS_EX_TOOLWINDOW, L"AlmondChild", L"Dummy",
//        WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst, nullptr);
//    if (!dummy) return false;
//    HDC dummyDC = GetDC(dummy);
//    SetupPixelFormat(dummyDC);
//    sharedContext = wglCreateContext(dummyDC);
//    wglMakeCurrent(dummyDC, sharedContext);
//#ifdef ALMOND_USING_OPENGL
//    gladLoadGL();
//#endif
//    wglMakeCurrent(nullptr, nullptr);
//    ReleaseDC(dummy, dummyDC);
//    DestroyWindow(dummy);
//
//    for (int i = 0; i < windowCount; ++i) {
//        HWND hwnd = CreateWindowEx(0, L"AlmondChild", L"OpenGL Window",
//            WS_CHILD | WS_VISIBLE,
//            0, 0, 400, 300,
//            parent ? parent : nullptr,
//            nullptr, hInst, nullptr);
//        if (!hwnd) continue;
//        if (i == 0) {
//            HDC hdc = GetDC(hwnd);
//            HGLRC ctx = CreateSharedGLContext(hdc);
//            windows.emplace_back(hwnd, hdc, ctx, true);
//        }
//        else {
//            windows.emplace_back(hwnd, nullptr, nullptr, false);
//        }
//    }
//
//    ArrangeDockedWindowsGrid();
//
//    return !windows.empty();
//}
//
////void MultiContextManager::AddWindow(HWND hwnd, HDC hdc, HGLRC glContext,
////    bool usesSharedContext, ResizeCallback onResize) {
////    windows.emplace_back(hwnd, hdc, glContext, usesSharedContext);
////    windows.back().onResize = std::move(onResize);
////    if (!hdc) MakeDockable(hwnd, parent);
////}
//
//void MultiContextManager::AddWindow(HWND hwnd, HDC hdc, HGLRC glContext, bool usesSharedContext, ResizeCallback onResize) {
//    if (!hwnd) return;
//
//    // Acquire HDC if not provided
//    if (!hdc) hdc = GetDC(hwnd);
//
//    // Create context if needed
//    if (!glContext) {
//        SetupPixelFormat(hdc); // <-- Required if HDC never had it
//        glContext = CreateSharedGLContext(hdc);
//
//        // Glad only needs to be loaded once (on some valid context)
//        static bool gladInitialized = false;
//        if (!gladInitialized) {
//            wglMakeCurrent(hdc, glContext);
//#ifdef ALMOND_USING_OPENGL
//            gladInitialized = gladLoadGL();  // Load function pointers
//#endif
//            wglMakeCurrent(nullptr, nullptr);
//        }
//    }
//
//    MultiContextWindowData data(hwnd, hdc, glContext, usesSharedContext);
//    data.onResize = std::move(onResize);
//    windows.emplace_back(std::move(data));
//
//   // if (parent) {
//        std::cout << "[Debug] Making HWND dockable: hwnd = " << hwnd << ", parent = " << parent << std::endl;
//        MakeDockable(hwnd, parent);
//   // }
//   // else {
//   //     std::cout << "[Debug] Skipping dock (null parent): hwnd = " << hwnd << std::endl;
//    //}
//
//    // Attach per-window render thread if it's a shared context
//    if (usesSharedContext && !gThreads.contains(hwnd)) {
//        gThreads[hwnd] = std::thread([this, hwnd]() {
//            auto it = std::find_if(windows.begin(), windows.end(),
//                [hwnd](const MultiContextWindowData& w) { return w.hwnd == hwnd; });
//            if (it != windows.end()) RenderLoop(*it);
//            });
//    }
//
//    ArrangeDockedWindowsGrid();
//}
//
//void MultiContextManager::RemoveWindow(HWND hwnd) {
//    auto it = std::find_if(windows.begin(), windows.end(),
//        [hwnd](const MultiContextWindowData& w) { return w.hwnd == hwnd; });
//    if (it == windows.end()) return;
//    it->running = false;
//    if (gThreads.contains(hwnd)) {
//        gThreads[hwnd].join();
//        gThreads.erase(hwnd);
//    }
//    if (it->glContext) {
//        wglMakeCurrent(nullptr, nullptr);
//        wglDeleteContext(it->glContext);
//    }
//    if (it->hdc && it->hwnd) ReleaseDC(it->hwnd, it->hdc);
//    windows.erase(it);
//}
//
//void MultiContextManager::ArrangeDockedWindowsGrid() {
//    if (!parent || windows.empty()) return;
//    RECT rc; GetClientRect(parent, &rc);
//    int cols = 1, rows = 1;
//    while (cols * rows < (int)windows.size())
//        (cols <= rows ? ++cols : ++rows);
//    int cw = rc.right / cols, ch = rc.bottom / rows;
//    for (size_t i = 0; i < windows.size(); ++i) {
//        int c = int(i) % cols, r = int(i) / cols;
//        SetWindowPos(windows[i].hwnd, nullptr, c * cw, r * ch, cw, ch,
//            SWP_NOZORDER | SWP_NOACTIVATE);
//        if (windows[i].onResize) windows[i].onResize(cw, ch);
//    }
//}
//
//void MultiContextManager::StartRenderThreads() {
//    for (const auto& w : windows) {
//        if (w.usesSharedContext && !gThreads.contains(w.hwnd)) {
//            gThreads[w.hwnd] = std::thread([this, hwnd = w.hwnd]() {
//                auto it = std::find_if(windows.begin(), windows.end(),
//                    [hwnd](const MultiContextWindowData& w) { return w.hwnd == hwnd; });
//                if (it != windows.end()) RenderLoop(*it);
//                });
//        }
//    }
//}
//
//void MultiContextManager::StopAll() {
//    running = false;
//    for (auto& w : windows) w.running = false;
//    for (auto& [hwnd, th] : gThreads) {
//        if (th.joinable()) th.join();
//    }
//    gThreads.clear();
//}
//
//void MultiContextManager::RenderLoop(MultiContextWindowData& win) {
//    if (!wglMakeCurrent(win.hdc, win.glContext)) return;
//    while (running && win.running) {
//#ifdef ALMOND_USING_OPENGL
//        glClearColor(win.clicked ? 1.f : 0.f, 0.f, win.clicked ? 0.f : 1.f, 1.f);
//        glClear(GL_COLOR_BUFFER_BIT);
//#endif
//        SwapBuffers(win.hdc);
//        std::this_thread::sleep_for(std::chrono::milliseconds(16));
//    }
//    wglMakeCurrent(nullptr, nullptr);
//}
//
//void MultiContextManager::HandleDropFiles(HWND hwnd, HDROP hDrop) {
//    UINT count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
//    for (UINT i = 0; i < count; ++i) {
//        wchar_t path[MAX_PATH]{};
//        DragQueryFile(hDrop, i, path, MAX_PATH);
//        std::wcout << L"[Drop] " << path << L"\n";
//    }
//}
//
//LRESULT CALLBACK MultiContextManager::ParentProc(HWND hwnd, UINT msg,
//    WPARAM wParam, LPARAM lParam) {
//    if (msg == WM_NCCREATE) {
//        auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
//        SetWindowLongPtr(hwnd, GWLP_USERDATA,
//            reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
//        return TRUE;
//    }
//    auto* mgr = reinterpret_cast<MultiContextManager*>(
//        GetWindowLongPtr(hwnd, GWLP_USERDATA));
//    if (!mgr) return DefWindowProc(hwnd, msg, wParam, lParam);
//
//    switch (msg) {
//    case WM_SIZE: mgr->ArrangeDockedWindowsGrid(); return 0;
//    case WM_PAINT: {
//        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
//        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
//        EndPaint(hwnd, &ps); return 0;
//    }
//    case WM_DROPFILES:
//        mgr->HandleDropFiles(hwnd, reinterpret_cast<HDROP>(wParam));
//        DragFinish(reinterpret_cast<HDROP>(wParam));
//        return 0;
//    case WM_CLOSE: DestroyWindow(hwnd); return 0;
//    case WM_DESTROY: PostQuitMessage(0); return 0;
//    default: return DefWindowProc(hwnd, msg, wParam, lParam);
//    }
//}
//
//LRESULT CALLBACK MultiContextManager::ChildProc(HWND hwnd, UINT msg,
//    WPARAM wParam, LPARAM lParam) {
//    static DragState& drag = gDragState;
//    if (msg == WM_NCCREATE) {
//        auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
//        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
//        return TRUE;
//    }
//
//    switch (msg) {
//    case WM_LBUTTONDOWN: {
//        SetCapture(hwnd);
//        drag.dragging = true;
//        drag.draggedWindow = hwnd;
//        drag.originalParent = GetParent(hwnd);
//        POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//        ClientToScreen(hwnd, &pt);
//        drag.lastMousePos = pt;
//        return 0;
//    }
//    case WM_MOUSEMOVE: {
//        if (!drag.dragging || drag.draggedWindow != hwnd)
//            return DefWindowProc(hwnd, msg, wParam, lParam);
//
//        POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//        ClientToScreen(hwnd, &pt);
//        int dx = pt.x - drag.lastMousePos.x;
//        int dy = pt.y - drag.lastMousePos.y;
//        drag.lastMousePos = pt;
//
//        RECT wndRect; GetWindowRect(hwnd, &wndRect);
//        int newX = wndRect.left + dx;
//        int newY = wndRect.top + dy;
//
//        if (drag.originalParent) {
//            RECT prc; GetClientRect(drag.originalParent, &prc);
//            POINT tl{ 0, 0 }; ClientToScreen(drag.originalParent, &tl);
//            OffsetRect(&prc, tl.x, tl.y);
//
//            bool inside =
//                newX >= prc.left && newY >= prc.top &&
//                (newX + (wndRect.right - wndRect.left)) <= prc.right &&
//                (newY + (wndRect.bottom - wndRect.top)) <= prc.bottom;
//
//            if (inside) {
//                if (GetParent(hwnd) != drag.originalParent) {
//                    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
//                    style &= ~(WS_POPUP | WS_OVERLAPPEDWINDOW);
//                    style |= WS_CHILD;
//                    SetWindowLongPtr(hwnd, GWL_STYLE, style);
//                    SetParent(hwnd, drag.originalParent);
//
//                    POINT cp{ newX, newY };
//                    ScreenToClient(drag.originalParent, &cp);
//                    SetWindowPos(hwnd, nullptr, cp.x, cp.y,
//                        wndRect.right - wndRect.left,
//                        wndRect.bottom - wndRect.top,
//                        SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
//                }
//                else {
//                    POINT cp{ newX, newY };
//                    ScreenToClient(drag.originalParent, &cp);
//                    SetWindowPos(hwnd, nullptr, cp.x, cp.y,
//                        0, 0, SWP_NOZORDER | SWP_NOSIZE |
//                        SWP_NOACTIVATE);
//                }
//            }
//            else {
//                if (GetParent(hwnd) == drag.originalParent) {
//                    LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
//                    style &= ~WS_CHILD;
//                    style |= WS_OVERLAPPEDWINDOW | WS_VISIBLE;
//                    SetWindowLongPtr(hwnd, GWL_STYLE, style);
//                    SetParent(hwnd, nullptr);
//                    SetWindowPos(hwnd, nullptr, newX, newY,
//                        wndRect.right - wndRect.left,
//                        wndRect.bottom - wndRect.top,
//                        SWP_NOZORDER | SWP_FRAMECHANGED |
//                        SWP_SHOWWINDOW);
//                }
//                else {
//                    SetWindowPos(hwnd, nullptr, newX, newY,
//                        0, 0, SWP_NOZORDER | SWP_NOSIZE |
//                        SWP_NOACTIVATE);
//                }
//            }
//        }
//        else {
//            SetWindowPos(hwnd, nullptr, newX, newY,
//                0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
//        }
//        return 0;
//    }
//    case WM_LBUTTONUP: {
//        if (drag.dragging && drag.draggedWindow == hwnd) {
//            ReleaseCapture();
//            drag.dragging = false;
//            drag.draggedWindow = nullptr;
//            drag.originalParent = nullptr;
//        }
//        return 0;
//    }
//    case WM_DROPFILES: {
//        if (HWND p = GetParent(hwnd))
//            SendMessage(p, WM_DROPFILES, wParam, lParam);
//        DragFinish(reinterpret_cast<HDROP>(wParam));
//        return 0;
//    }
//    case WM_PAINT: {
//        PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
//        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
//        EndPaint(hwnd, &ps);
//        return 0;
//    }
//    default:
//        return DefWindowProc(hwnd, msg, wParam, lParam);
//    }
//}
//
//// Register window class function
//struct WindowClassAtoms {
//    ATOM parentAtom;
//    ATOM childAtom;
//    LPCWSTR parentName;
//    LPCWSTR childName;
//    // Conversion to bool to allow usage with '!' operator  
//    explicit operator bool() const {
//        return parentAtom != 0 && childAtom != 0;
//    }
//};
//
//// Rename the function to avoid overloading by return type alone  
//WindowClassAtoms RegisterWindowClassAtoms(HINSTANCE hInstance, LPCWSTR parentClass, LPCWSTR childClass) {
//    WNDCLASSW wc = {};
//    wc.hInstance = hInstance;
//    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
//
//    // Parent class (could be regular window)
//    //wc.style = CS_HREDRAW | CS_VREDRAW;
//    wc.lpfnWndProc = MultiContextManager::ParentProc;
//    wc.lpszClassName = parentClass;
//    ATOM parentAtom = RegisterClassW(&wc);
//
//    // Child class (needs CS_OWNDC for OpenGL!)
//    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // <--- KEY FIX
//    wc.lpfnWndProc = MultiContextManager::ChildProc;
//    wc.lpszClassName = childClass;
//    ATOM childAtom = RegisterClassW(&wc);
//
//    return { parentAtom, childAtom, parentClass, childClass };
//}
//
//
//////// Window procedure function to handle messages
////LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
////    switch (message) {
////    case WM_DESTROY:
////        PostQuitMessage(0);
////        break;
////    default:
////        return DefWindowProc(hWnd, message, wParam, lParam);
////    }
////    return 0;
////}
//
//// Print Win32 errors
//void PrintLastWin32Error(const wchar_t* lpszFunction) {
//    DWORD error = GetLastError();
//    if (error) {
//        LPWSTR buffer = nullptr;
//        FormatMessageW(
//            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
//            NULL, error, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//            (LPWSTR)&buffer, 0, NULL);
//
//        std::wcout << L"Error in " << lpszFunction << L": " << buffer;
//        LocalFree(buffer);
//    }
//}
//
//// Initialize and create the window instance
//HWND InitWindowInstance(HINSTANCE hInstance, int nCmdShow, LPCWSTR szWindowClass, LPCWSTR szTitle, int32_t windowWidth, int32_t windowHeight) {
//    if (!RegisterWindowClassAtoms(hInstance, szWindowClass, L"AlmondChild")) {
//        return nullptr;
//    }
//
//    HWND window_handle = CreateWindowW(
//        szWindowClass,
//        szTitle,
//        WS_OVERLAPPEDWINDOW,
//        CW_USEDEFAULT,
//        CW_USEDEFAULT,
//        (windowWidth != 0) ? windowWidth : CW_USEDEFAULT,
//        (windowHeight != 0) ? windowHeight : CW_USEDEFAULT,
//        nullptr,
//        nullptr,
//        hInstance,
//        nullptr);
//
//    if (!window_handle) {
//        PrintLastWin32Error(L"CreateWindowW");
//        return nullptr;
//    }
//
//    // === Get DPI for this window ===
//    UINT dpi = GetDpiForWindow(window_handle);
//
//    // === Get monitor where the window is ===
//    HMONITOR monitor = MonitorFromWindow(window_handle, MONITOR_DEFAULTTONEAREST);
//    MONITORINFO mi = { sizeof(mi) };
//    GetMonitorInfo(monitor, &mi);
//
//    int monitorWidth = mi.rcWork.right - mi.rcWork.left;
//    int monitorHeight = mi.rcWork.bottom - mi.rcWork.top;
//
//    // Desired *client* area size
//    int desiredClientWidth = monitorWidth / 2;
//    int desiredClientHeight = monitorHeight / 2;
//
//    // Adjust for window chrome
//    RECT windowRect = { 0, 0, desiredClientWidth, desiredClientHeight };
//    AdjustWindowRectExForDpi(&windowRect, WS_OVERLAPPEDWINDOW, FALSE, 0, dpi);
//
//    int winWidth = windowRect.right - windowRect.left;
//    int winHeight = windowRect.bottom - windowRect.top;
//
//    // Center
//    int posX = mi.rcWork.left + (monitorWidth - winWidth) / 2;
//    int posY = mi.rcWork.top + (monitorHeight - winHeight) / 2;
//
//    // Apply new size & position
//    MoveWindow(window_handle, posX, posY, windowWidth, windowHeight, TRUE);  //center the window on the screen
//    //MoveWindow(window_handle, 0, 0, windowWidth, windowHeight, TRUE);  //center the window on the screen
//    ShowWindow(window_handle, nCmdShow);
//    UpdateWindow(window_handle);
//
//
//    // --- Create Child Window ---
//    //HWND hwndChild = CreateWindowW(
//    //    L"AlmondChild",           // Child class name
//    //    L"Child",                 // Window title (can be anything)
//    //    WS_CHILD | WS_VISIBLE,
//    //    0, 0, desiredClientWidth, desiredClientHeight, // Position and size (relative to parent)
//    //    window_handle,               // Parent HWND
//    //    nullptr,
//    //    hInstance,
//    //    nullptr);
//
//    //if (!hwndChild) {
//    //    PrintLastWin32Error(L"CreateWindowW (child)");
//    //    // Optionally: Destroy parent if child creation failed
//    //    DestroyWindow(window_handle);
//    //    return { nullptr };
//    //}
//    //else {// >>> ADD THIS: Store child HWND in context <<<
//    //    almondnamespace::contextwindow::WindowContext::get_global_instance()->setChildHandle(hwndChild);
//    //}
//
//
//    return window_handle;
//}
//
//// -----------------------------------------------------------------
//// Helper definitions that must be visible to the linker
//// -----------------------------------------------------------------
//struct SubCtx { HWND originalParent; };
//
//LRESULT CALLBACK DockableProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
//    UINT_PTR, DWORD_PTR dw) {
//    switch (msg) {
//    case WM_LBUTTONDOWN:
//    case WM_MOUSEMOVE:
//    case WM_LBUTTONUP:
//        return MultiContextManager::ChildProc(hwnd, msg, wp, lp);
//    }
//    return DefSubclassProc(hwnd, msg, wp, lp);
//}
//
//void MakeDockable(HWND hwnd, HWND parent) {
//    auto* ctx = new SubCtx{ parent };
//    SetWindowSubclass(hwnd, DockableProc, 1, reinterpret_cast<DWORD_PTR>(ctx));
//}
//
//
/////////////////////////////////////////////////////////



//// Now ALWAYS define WinMain so the linker will find it
//int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
//    _In_opt_ HINSTANCE hPrevInstance,
//    _In_ LPWSTR lpCmdLine,
//    _In_ int nCmdShow) {
//    UNREFERENCED_PARAMETER(hPrevInstance);
//    UNREFERENCED_PARAMETER(lpCmdLine);
//
//#ifdef _DEBUG
//    almondnamespace::core::ShowConsole();    // AllocConsole() + freopen()
//#endif
//
//    try {
//        // Command line processing
//        int argc = __argc;
//        char** argv = __argv;
//
//        // Convert command line to wide string array (LPWSTR)
//        int32_t numCmdLineArgs = 0;
//        LPWSTR* pCommandLineArgvArray = CommandLineToArgvW(GetCommandLineW(), &numCmdLineArgs);
//
//        if (pCommandLineArgvArray == nullptr) {
//            MessageBoxW(NULL, L"Command line parsing failed!", L"Error", MB_ICONERROR | MB_OK);
//            return -1;
//        }
//
//        // Free the argument array after use
//        LocalFree(pCommandLineArgvArray);
//
//        cli::parse(__argc, __argv);
//       // almondnamespace::core::handleCommandLine(argc, argv);
//
//        //LPCWSTR window_name = L"Almond WINMAIN Example Window";
//        //almondnamespace::core::RegisterWindowClass(hInstance, szTitle, szChildTitle);
//
//        almondnamespace::contextwindow::WindowData::set_global_instance(&windowContext);
//        almondnamespace::contextwindow::WindowData* corectx = almondnamespace::contextwindow::WindowData::get_global_instance();
//
//		//// Intialize function from above
//  //      HWND window_handle = InitWindowInstance(hInstance, nCmdShow, szWindowClass, szTitle, cli::window_width, cli::window_height);
//  //      if (!window_handle) {
//  //          return -1;
//  //      }
//
//		// Set the HWND globally so it can be accessed anywhere in the engine's window context
//        // corectx->setChildHandle(window_handle);
//     //   corectx->setParentHandle(window_handle);
//
//
//// ─── Initialize contexts only the enabled backends with macro guards ──────────────────────────────
//
//#ifdef ALMOND_USING_OPENGL
//        auto openglCtx = std::make_shared<almondnamespace::core::Context>();
//        almondnamespace::core::AddContextForBackend("OPENGL", openglCtx); // register to global map
//        // almondnamespace::core::g_contexts["SDL"].emplace_back(sdlCtx);
//        bool openglOk = false; // Will be set properly during initialization
//#else
//        std::shared_ptr<almondnamespace::core::Context> openglCtx = nullptr;
//        constexpr bool openglOk = false;
//#endif
//
//#ifdef ALMOND_USING_SDL
//        auto sdlCtx = std::make_shared<almondnamespace::core::Context>();
//        almondnamespace::core::AddContextForBackend("SDL", sdlCtx); // register to global map
//       // almondnamespace::core::g_contexts["SDL"].emplace_back(sdlCtx);
//        bool sdlOk = false; // Will be set properly during initialization
//#else
//        std::shared_ptr<almondnamespace::core::Context> sdlCtx = nullptr;
//        constexpr bool sdlOk = false;
//#endif
//
//#ifdef ALMOND_USING_SFML
//        auto sfmlCtx = std::make_shared<almondnamespace::core::Context>();
//        almondnamespace::core::AddContextForBackend("SFML", sfmlCtx);
//        //almondnamespace::core::g_contexts["SFML"].emplace_back(sfmlCtx);
//        bool sfmlOk = false; // Will be set properly during initialization
//#else
//        std::shared_ptr<almondnamespace::core::Context> sfmlCtx = nullptr;
//        constexpr bool sfmlOk = false;
//#endif
//
//#ifdef ALMOND_USING_RAYLIB
//        auto raylibCtx = std::make_shared<almondnamespace::core::Context>();
//        almondnamespace::core::AddContextForBackend("RayLib", raylibCtx);
//        // almondnamespace::core::g_contexts["Raylib"].emplace_back(raylibCtx);
//        bool raylibOk = false; // Will be set properly during initialization
//#else
//        std::shared_ptr<almondnamespace::core::Context> raylibCtx = nullptr;
//        constexpr bool raylibOk = false;
//#endif
//
//       // almondnamespace::core::InitializeAllContexts();
//		// Create the MultiContextManager instance
//        almondnamespace::core::MultiContextManager mgr;
//
//        HINSTANCE hi = GetModuleHandle(nullptr);
//        //mgr.SetParentWindow(window_handle);
//        bool multiOk = mgr.Initialize(hi, 0, ALMOND_SINGLE_PARENT == 1);
//
//
//        // setup the parent throughout the engine.. sloppy but works for now, can clean this up with a proper context manager
//        // the way resize, input, and the backwards context itself because of the docking and manager code... todo: unify this
//        HWND parent = mgr.GetParentWindow();
//		std::cout << "Parent HWND: " << std::hex << reinterpret_cast<uintptr_t>(parent) << '\n';
//
//       // corectx->setParentHandle(mgr.GetParentWindow());
//      //  parent = corectx->getParentHandle();//mgr.GetParentWindow();
//
//
//
//        almondnamespace::core::MultiContextManager::RegisterParentClass(hi, L"AlmondParent");
//        if (ALMOND_SINGLE_PARENT) {
//            parent = CreateWindowEx(0, L"AlmondParent", L"Almond Docking",
//                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//                CW_USEDEFAULT, CW_USEDEFAULT, 1920, 1080,
//                nullptr, nullptr, hi, nullptr);
//            if (!parent) return false;
//            DragAcceptFiles(parent, TRUE);
//        }
//        else {
//            parent = nullptr;
//        }
//
//        std::cout << "Parent HWND: " << std::hex << reinterpret_cast<uintptr_t>(parent) << '\n';
//
//
//
//
//        // Initialize enabled backends
//
//#ifdef ALMOND_USING_OPENGL
//        {
//            openglOk = almondnamespace::openglcontext::opengl_initialize(openglCtx, parent, 400, 300);
//            if (openglOk) {
//                mgr.AddWindow(openglCtx->hwnd, nullptr, nullptr, false, [openglCtx](int w, int h) {
//                    SetWindowPos(openglCtx->hwnd, nullptr, 0, 0, w, h,
//                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
//                    openglCtx->width = w;
//                    openglCtx->height = h;
//                    if (openglCtx->onResize) openglCtx->onResize(w, h);
//                    });
//            }
//        }
//#endif
//
//
//#ifdef ALMOND_USING_SDL
//        {
//            sdlOk = almondnamespace::sdlcontext::sdl_initialize(sdlCtx, parent, 400, 300);
//            if (sdlOk) {
//                mgr.AddWindow(sdlCtx->hwnd, nullptr, nullptr, true, [sdlCtx](int w, int h) {
//                    sdlCtx->width = w;
//                    sdlCtx->height = h;
//                    });
//            }
//        }
//#endif
//
//#ifdef ALMOND_USING_SFML
//        {
//            sfmlOk = almondnamespace::sfmlcontext::sfml_initialize(sfmlCtx, parent, 400, 300);
//            if (sfmlOk) {
//                mgr.AddWindow(sfmlCtx->hwnd, nullptr, nullptr, false, [sfmlCtx](int w, int h) {
//                    sfmlCtx->width = w;
//                    sfmlCtx->height = h;
//                    if (sfmlCtx->onResize) sfmlCtx->onResize(w, h);
//                    });
//            }
//        }
//#endif
//
//#ifdef ALMOND_USING_RAYLIB
//        {
//            raylibOk = almondnamespace::raylibcontext::raylib_initialize(raylibCtx, parent, 400, 300);
//            if (raylibOk) {
//                mgr.AddWindow(raylibCtx->hwnd, nullptr, nullptr, false, [raylibCtx](int w, int h) {
//                    SetWindowPos(raylibCtx->hwnd, nullptr, 0, 0, w, h,
//                        SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
//                    raylibCtx->width = w;
//                    raylibCtx->height = h;
//                    if (raylibCtx->onResize) raylibCtx->onResize(w, h);
//                    });
//            }
//        }
//#endif
//
//       // std::cout << "Passing parent HWND: " << std::hex << reinterpret_cast<uintptr_t>(parent) << '\n';
//        //mgr.AddWindow(sdlCtx->hwnd, nullptr, nullptr, true, [&](int w, int h) {
//        //    sdlCtx->width = w; sdlCtx->height = h;
//        //    });
//
//        //mgr.AddWindow(sfmlCtx->hwnd, nullptr, nullptr, true, [&](int w, int h) {
//        //    sfmlCtx->onResize( w,h );
//        //    sfmlCtx->width = w; sfmlCtx->height = h;
//        //    });
//
//        //mgr.AddWindow(raylibCtx->hwnd, nullptr, nullptr, true, [&](int w, int h) {
//        //    SetWindowPos(raylibCtx->hwnd, nullptr, 0, 0, w, h, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);
//        //    raylibCtx->width = w; raylibCtx->height = h;
//        //    });
//        //
//        //bool sdlOk = almondnamespace::sdlcontext::sdl_initialize(sdlCtx, parent, 400, 300);
//        //bool raylibOk = almondnamespace::raylibcontext::raylib_initialize(raylibCtx, parent, 400, 300);
//        //bool sfmlOk = almondnamespace::sfmlcontext::sfml_initialize(sfmlCtx, parent, 400, 300, nullptr);
//
//      //  if (multiOk) {
//           mgr.StartRenderThreads();
//            mgr.ArrangeDockedWindowsGrid();
//       // }
//
////#define menu_on	// If menu is enabled, show the menu loop
//bool running = true;
//	
////#define menu_off // If menu is disabled, just run the contexts without a menu loop
//#ifndef menu_off
//
//		// print all contexts hwnds
//        for (const auto& ctx : almondnamespace::core::g_contexts) {
//            std::cout << "HWND: " << ctx.first << "\n";
//        }
//
////        // ─── Game Menu Loop ──────────────────────────────────────────────
////        std::vector<std::shared_ptr<almondnamespace::core::Context>> menuCapable;
////
////        for (auto& [_, group] : almondnamespace::core::g_contexts)
////            for (auto& ctx : group)
////            {
////                std::cout << "Backend: " << _ << ", Contexts: " << group.size() << "\n";
////                if (ctx) menuCapable.emplace_back(ctx);
////            }
////#ifndef ALMOND_USING_NOOP
////        if (menuCapable.empty()) {
////            std::cerr << "No active contexts for game loop.\n";
////            return 0;
////        }
////#endif
////                std::cout << "[Menu] Showing on backend: " << corectx->hwnd << "\n";
//
//        // ─── Main Update Loop ────────────────────────────────────────────
//        MSG msg{};
//        while (running)
//        {
//            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) 
//            {
//
//                if (msg.message == WM_QUIT) { running = false; }
//
//                else if (msg.message == WM_DROPFILES) {
//                    if (HWND p = GetParent(msg.hwnd))
//                        SendMessage(p, WM_DROPFILES, msg.wParam, msg.lParam);
//                    DragFinish(reinterpret_cast<HDROP>(msg.wParam));
//                    continue;
//				}
//                else  if (msg.message == WM_SIZE) {
//                    // Handle resizing here if needed
//                    // For example, update context sizes
//                    for (auto& ctx : almondnamespace::core::g_contexts) {
//                        for (auto& c : ctx.second) {
//                            if (c && c->hwnd == msg.hwnd) {
//                                c->width = LOWORD(msg.lParam);
//                                c->height = HIWORD(msg.lParam);
//                            }
//                        }
//                    }
//                    continue;
//                }
//                else if (msg.message == WM_CLOSE) {
//                    // Handle window close
//                    for (auto& ctx : almondnamespace::core::g_contexts) {
//                        for (auto& c : ctx.second) {
//                            if (c && c->hwnd == msg.hwnd) {
//                                running = false;  // Stop the context
//                                mgr.RemoveWindow(c->hwnd);  // Remove from manager
//                            }
//                        }
//                    }
//                    continue;
//				}
//                else  if (msg.message == WM_DESTROY) {
//                    // Handle window destruction
//                    for (auto& ctx : almondnamespace::core::g_contexts) {
//                        for (auto& c : ctx.second) {
//                            if (c && c->hwnd == msg.hwnd) {
//                                running = false;  // Stop the context
//                                mgr.RemoveWindow(c->hwnd);  // Remove from manager
//                            }
//                        }
//                    }
//					continue;
//                }
//                else
//                {
//                    TranslateMessage(&msg);
//                    DispatchMessage(&msg);
//                }
//            }
//#endif // menu_on
//
//#ifdef menu_off
//            // If no menu, just run the contexts without a menu loop
//            while (running) {
//           
//                if (openglCtx && almondnamespace::openglcontext::OpenGLIsRunning(openglCtx))
//                almondnamespace::openglcontext::opengl_process(openglCtx);
//				else openglOk = false;
//
//#ifdef ALMOND_USING_RAYLIB
//            if (raylibOk && almondnamespace::raylibcontext::RaylibIsRunning(raylibCtx))
//                almondnamespace::raylibcontext::raylib_process(raylibCtx);
//            else raylibOk = false;
//#endif
//
//#ifdef ALMOND_USING_SDL
//           if (sdlOk && almondnamespace::sdlcontext::SDLIsRunning(sdlCtx))
//                almondnamespace::sdlcontext::sdl_process(sdlCtx);
//            else sdlOk = false;
//#endif
//
//#ifdef ALMOND_USING_SFML
//            if (sfmlOk && almondnamespace::sfmlcontext::SFMLIsRunning(sfmlCtx))
//                almondnamespace::sfmlcontext::sfml_process(sfmlCtx);
//            else sfmlOk = false;
//#endif
//
//                if (!mgr.IsRunning())
//            {
//               // multiOk = false;
//            }
//
//                if (!raylibOk && !sdlOk && !sfmlOk && !openglOk)
//            //if (!raylibOk && !sdlOk && !sfmlOk && !multiOk)
//               // if (!multiOk)
//                running = false;
//                std::this_thread::sleep_for(std::chrono::milliseconds(16));
// }
//
//#else
//
//
//
//#ifdef ALMOND_USING_RAYLIB
//            if (raylibOk && almondnamespace::raylibcontext::RaylibIsRunning(raylibCtx))
//                almondnamespace::raylibcontext::raylib_process(raylibCtx);
//            else raylibOk = false;
//#endif
//
//#ifdef ALMOND_USING_SDL
//            if (sdlOk && almondnamespace::sdlcontext::SDLIsRunning(sdlCtx))
//                almondnamespace::sdlcontext::sdl_process(sdlCtx);
//            else sdlOk = false;
//#endif
//
//#ifdef ALMOND_USING_SFML
//            if (sfmlOk && almondnamespace::sfmlcontext::SFMLIsRunning(sfmlCtx))
//                almondnamespace::sfmlcontext::sfml_process(sfmlCtx);
//            else sfmlOk = false;
//#endif
//
//            if (!mgr.IsRunning())
//              //  multiOk = false;
//
//            if (!raylibOk && !sdlOk && !sfmlOk)// && !multiOk)
//                //if (!multiOk)
//                running = false;
//            // Pick *first valid context* for menu — future: rotate/menu selector
////                static size_t currentContext = 0;
////#ifndef ALMOND_USING_NOOP
////                if (menuCapable.empty()) {
////                    std::cerr << "No menu-capable contexts found!\n";
////                    return 0;
////                }
////
////#endif
////                // Wrap around if out of bounds
////                if (currentContext >= menuCapable.size())
////                    currentContext = 0;
////
////                auto ctx = menuCapable[currentContext];
//
//#ifndef ALMOND_USING_NOOP
//                //if (!ctx) {
//               //     std::cerr << "Menu context invalid at index: " << (currentContext - 1) << "\n";
//               //     return 0;
//              //  }
//#endif
//
//            auto myctx = almondnamespace::core::g_contexts["OPENGL"][0];
//            //auto choice = almondnamespace::menu::show_menu(*ctx);
//
//
//            //if (myctx && almondnamespace::openglcontext::OpenGLIsRunning(myctx))
//            //    almondnamespace::openglcontext::opengl_process();
//            //else openglOk = false;
//
//
//
//
//            auto choice = almondnamespace::menu::show_menu(*myctx);
//            // Increment only after processing
//         //   currentContext = (currentContext + 1) % menuCapable.size();
//            using enum almondnamespace::menu::Choice;
//
//            switch (choice) 
//            {
//                case Snake:      running = almondnamespace::snake::run_snake_ecs(*myctx); break;
//                case Tetris:     running = almondnamespace::tetris::run_tetris(*myctx); break;
//                case Pacman:     running = almondnamespace::pacman::run_pacman(*myctx); break;
//                case Sokoban:    running = almondnamespace::sokoban::run_sokoban(*myctx); break;
//                case Minesweep:  running = almondnamespace::minesweeper::run_minesweeper(*myctx); break;
//                case Puzzle:     running = almondnamespace::sliding::run_sliding(*myctx); break;
//                case Bejeweled:  running = almondnamespace::match3::run_match3(*myctx); break;
//                case Fourty:     running = almondnamespace::game2048::run_2048(*myctx); break;
//                case Sandsim:    running = almondnamespace::sandsim::run_sand(*myctx); break;
//                case Cellular:   running = almondnamespace::cellular::run_cellular(*myctx); break;
//                case Settings:   std::cout << "[Menu] Settings selected.\n"; break;
//                case Exit:       running = false; break;
//            }
//			// Sleep to avoid busy-waiting
//            std::this_thread::sleep_for(std::chrono::milliseconds(16));
//
//        }
//#endif
//
//
//
//        mgr.StopAll();
//        // ─── Cleanup ─────────────────────────────────────────────────────
//#ifdef ALMOND_USING_SDL
//        almondnamespace::sdlcontext::sdl_cleanup(sdlCtx);
//#endif
//
//#ifdef ALMOND_USING_SFML
//        almondnamespace::sfmlcontext::sfml_cleanup(sfmlCtx);
//#endif
//
//#ifdef ALMOND_USING_RAYLIB
//        almondnamespace::raylibcontext::raylib_cleanup(raylibCtx);
//#endif
//
//        //almondnamespace::core::StartEngine();
//
//
//  //      auto& app = almondnamespace::core::g_app;
//  //      app.parent = almondnamespace::core::create_window(hInstance, 100, 100, 800, 600, L"AlmondParent", L"Parent", nullptr);
//  //      app.parentDC = GetDC(app.parent);
//  //      ShowWindow(app.parent, SW_SHOW);
//  //      UpdateWindow(app.parent);
//
//  //      RECT rc;
//  //      GetClientRect(app.parent, &rc);
//  //      PostMessage(app.parent, WM_SIZE, 0, MAKELPARAM(rc.right - rc.left, rc.bottom - rc.top));
//
//
//		//// Create a child window
//  //      app.child = almondnamespace::core::create_window(hInstance, 100, 100, 800, 600, L"AlmondChild", L"Child", app.parent);
//  //      app.parentDC = GetDC(app.parent );
//  //      windowContext.setParentHandle(app.parent);
//
//  //      //auto* winCtx = new almondnamespace::contextwindow::WindowContext();
//  //      ctx->setChildHandle(app.child);
//
//
//
//
//        //HWND child_handle = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
//        //    (cli::window_width != 0) ? cli::window_width : CW_USEDEFAULT, (cli::window_height != 0) ? cli::window_height : CW_USEDEFAULT,
//        //    window_handle, nullptr, hInstance, nullptr);
//
//
//        // std::cout << "Hello World!\n";
//    }
//    catch (const std::exception& ex) {
//        MessageBoxA(nullptr, ex.what(), "Error", MB_ICONERROR | MB_OK);
//        return -1;
//    }
//
//    return 0;
//#endif
//}

// Now ALWAYS define WinMain so the linker will find it
// entry.cpp
//#include "acontextmultiplexer.hpp"
//#include "amenu.hpp"
//#include "asnake.hpp"
//#include "atetris.hpp"
//#include "apacman.hpp"
//#include "asokoban.hpp"
//#include "aminesweeper.hpp"
//#include "aslidepuzzle.hpp"
//#include "amatch3.hpp"
//#include "a2048.hpp"
//#include "asandsim.hpp"
//#include "acellular.hpp"
//

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR lpCmdLine,
    _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

#ifdef _DEBUG
    almondnamespace::core::ShowConsole();
#endif

    try {
        // ---- Setup ----
        int argc = __argc;
        char** argv = __argv;

        int32_t numCmdLineArgs = 0;
        LPWSTR* pCommandLineArgvArray =
            CommandLineToArgvW(GetCommandLineW(), &numCmdLineArgs);

        if (!pCommandLineArgvArray) {
            MessageBoxW(NULL, L"Command line parsing failed!",
                L"Error", MB_ICONERROR | MB_OK);
            return -1;
        }
        LocalFree(pCommandLineArgvArray);

        // Initialize contexts globally (fills g_backends)
      //  almondnamespace::core::InitializeAllContexts();

        // Initialize multi-context manager
        almondnamespace::core::MultiContextManager mgr;
        HINSTANCE hi = GetModuleHandle(nullptr);
        bool multiOk = mgr.Initialize(hi,
            /*RayLib*/   1,
            /*SDL*/      1,
            /*SFML*/     0,
            /*OpenGL*/   1,
            /*Software*/ 1,
            ALMOND_SINGLE_PARENT == 1);

        if (!multiOk) {
            MessageBoxW(NULL, L"Failed to initialize contexts!",
                L"Error", MB_ICONERROR | MB_OK);
            return -1;
        }

        mgr.StartRenderThreads();
        mgr.ArrangeDockedWindowsGrid();

        // ---- Menu overlay ----
        almondnamespace::menu::MenuOverlay menu;
        for (auto& [type, state] : almondnamespace::core::g_backends) {
            if (state.master) menu.initialize(state.master);
            for (auto& dup : state.duplicates) menu.initialize(dup);
        }

        bool running = true;

        // ---- Main loop ----
        while (running) {
            MSG msg{};
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (msg.message == WM_QUIT) {
                    running = false;
                }
                else {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }

            // Update all backends
            for (auto& [type, state] : almondnamespace::core::g_backends) {
                auto update_on_ctx = [&](std::shared_ptr<almondnamespace::core::Context> ctx) -> bool {
                    if (!ctx) return true; // skip dead ctx

                    // Lookup matching WindowData
                    auto* win = mgr.findWindowByHWND(ctx->hwnd);
                    if (!win) return false;

                    bool ctxRunning = true;

                    // Backend-specific processing
                    switch (type) {
#ifdef ALMOND_USING_OPENGL
                    case almondnamespace::core::ContextType::OpenGL:
                        //ctxRunning = almondnamespace::openglcontext::opengl_process(
                        //    *ctx,
                        //    win->commandQueue);
                       // std::cout << "[DEBUG] opengl_process returned " << ctxRunning << "\n";
                        ctxRunning = win->running;
                        break;
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
                    case almondnamespace::core::ContextType::Software:
                        //ctxRunning = almondnamespace::anativecontext::softrenderer_process(
                        //    *ctx,
                        //    win->commandQueue);
                        break;
#endif
#ifdef ALMOND_USING_SDL
                    case almondnamespace::core::ContextType::SDL:
                        //ctxRunning = almondnamespace::sdlcontext::sdl_process(
                        //    *ctx,
                        //    win->commandQueue);
                        break;
#endif
#ifdef ALMOND_USING_SFML
                    case almondnamespace::core::ContextType::SFML:
                        //ctxRunning = almondnamespace::sfmlcontext::sfml_process(
                        //    *ctx,
                        //    win->commandQueue);
                        break;
#endif
#ifdef ALMOND_USING_RAYLIB
                    case almondnamespace::core::ContextType::RayLib:
                        //ctxRunning = almondnamespace::raylibcontext::raylib_process(
                        //    *ctx,
                        //    win->commandQueue);
                        break;
#endif
                    default:
                        break;
                    }

                    // Menu logic
                    if (auto choice = menu.update_and_draw(ctx, win)) {
                        switch (*choice) {
                        case almondnamespace::menu::Choice::Snake:     ctxRunning |= almondnamespace::snake::run_snake_ecs(ctx); break;
                        case almondnamespace::menu::Choice::Tetris:    ctxRunning |= almondnamespace::tetris::run_tetris(ctx); break;
                        case almondnamespace::menu::Choice::Pacman:    ctxRunning |= almondnamespace::pacman::run_pacman(ctx); break;
                        case almondnamespace::menu::Choice::Sokoban:   ctxRunning |= almondnamespace::sokoban::run_sokoban(ctx); break;
                        case almondnamespace::menu::Choice::Minesweep: ctxRunning |= almondnamespace::minesweeper::run_minesweeper(ctx); break;
                        case almondnamespace::menu::Choice::Puzzle:    ctxRunning |= almondnamespace::sliding::run_sliding(ctx); break;
                        case almondnamespace::menu::Choice::Bejeweled: ctxRunning |= almondnamespace::match3::run_match3(ctx); break;
                        case almondnamespace::menu::Choice::Fourty:    ctxRunning |= almondnamespace::game2048::run_2048(ctx); break;
                        case almondnamespace::menu::Choice::Sandsim:   ctxRunning |= almondnamespace::sandsim::run_sand(ctx); break;
                        case almondnamespace::menu::Choice::Cellular:  ctxRunning |= almondnamespace::cellular::run_cellular(ctx); break;
                        case almondnamespace::menu::Choice::Settings:
                            std::cout << "[Menu] Settings selected\n"; break;
                        case almondnamespace::menu::Choice::Exit:
                            std::cout << "[Menu] Exit requested\n";
                            running = false; // handled in outer loop
                            break;
                        }
                    }
                    return ctxRunning;
                    };

#if defined(ALMOND_SINGLE_PARENT)
                // All contexts tied to parent lifetime
                if (state.master && !update_on_ctx(state.master)) {
                    running = false;
                    break;
                }
                for (auto& dup : state.duplicates) {
                    if (!update_on_ctx(dup)) running = false;
                }
#else
                // Each context independent
                bool anyAlive = false;
                if (state.master) anyAlive |= update_on_ctx(state.master);
                for (auto& dup : state.duplicates) {
                    if (update_on_ctx(dup)) anyAlive = true;
                    else dup->running = false;
                }
                if (!anyAlive) {
                    std::cout << "[Engine] All contexts for backend "
                        << int(type) << " closed.\n";
                }
#endif
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }

        // ---- Cleanup ----
        menu.cleanup();
        mgr.StopAll();

        for (auto& [type, state] : almondnamespace::core::g_backends) {
            auto cleanup_backend = [&](std::shared_ptr<almondnamespace::core::Context> ctx) {
                if (!ctx) return;
                switch (type) {
#ifdef ALMOND_USING_OPENGL
                case almondnamespace::core::ContextType::OpenGL: almondnamespace::openglcontext::opengl_cleanup(ctx); break;
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
                case almondnamespace::core::ContextType::Software: almondnamespace::anativecontext::softrenderer_cleanup(ctx); break;
#endif
#ifdef ALMOND_USING_SDL
                case almondnamespace::core::ContextType::SDL: almondnamespace::sdlcontext::sdl_cleanup(ctx); break;
#endif
#ifdef ALMOND_USING_SFML
                case almondnamespace::core::ContextType::SFML: almondnamespace::sfmlcontext::sfml_cleanup(ctx); break;
#endif
#ifdef ALMOND_USING_RAYLIB
                case almondnamespace::core::ContextType::RayLib: almondnamespace::raylibcontext::raylib_cleanup(ctx); break;
#endif
                default: break;
                }
                };

            if (state.master) cleanup_backend(state.master);
            for (auto& dup : state.duplicates) cleanup_backend(dup);
        }
    }
    catch (const std::exception& ex) {
        MessageBoxA(nullptr, ex.what(), "Error", MB_ICONERROR | MB_OK);
        return -1;
    }

    return 0;
}
#endif


// -----------------------------------------------------------------------------
// Cross-platform Automatic Entry Point
// -----------------------------------------------------------------------------
int main(int argc, char* argv[]) {
#ifdef ALMOND_USING_WINMAIN
    LPWSTR pCommandLine = GetCommandLineW();
    return wWinMain(GetModuleHandle(NULL), NULL, pCommandLine, SW_SHOWNORMAL);
#else
    cli::parse(argc, argv);
    core::StartEngine(); // Replace with actual engine logic
    return 0;
#endif
}
