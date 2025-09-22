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
// acontextmultiplexer.cpp
#include "pch.h"

#include "acontextmultiplexer.hpp"

#include <stdexcept>
#include <glad/glad.h>

// -----------------------------------------------------------------
// Helper definitions that must be visible to the linker
// -----------------------------------------------------------------

// -----------------------------------------------------------------
// Dockable child handling
// -----------------------------------------------------------------
struct SubCtx { HWND originalParent; };

LRESULT CALLBACK DockableProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
    UINT_PTR, DWORD_PTR dw)
{
#if ALMOND_SINGLE_PARENT
    auto* ctx = reinterpret_cast<SubCtx*>(dw);
#endif

    switch (msg) {
    case WM_CLOSE:
#if ALMOND_SINGLE_PARENT
        if (ctx && ctx->originalParent) {
            if (hwnd == ctx->originalParent) {
                PostQuitMessage(0); // main parent closed -> shutdown engine
            }
            else {
                DestroyWindow(hwnd); // child closes itself
            }
        }
        else {
            DestroyWindow(hwnd); // no parent -> just close itself
        }
#else
        DestroyWindow(hwnd); // always independent
#endif
        return 0;

    case WM_LBUTTONDOWN:
    case WM_MOUSEMOVE:
    case WM_LBUTTONUP:
        return almondnamespace::core::MultiContextManager::ChildProc(hwnd, msg, wp, lp);
    }
    return DefSubclassProc(hwnd, msg, wp, lp);
}

void MakeDockable(HWND hwnd, HWND parent) {
#if ALMOND_SINGLE_PARENT
    auto* ctx = new SubCtx{ parent };
    SetWindowSubclass(hwnd, DockableProc, 1, reinterpret_cast<DWORD_PTR>(ctx));
#endif
}

namespace almondnamespace::core
{

    WindowData* MultiContextManager::findWindowByHWND(HWND hwnd) {
        std::scoped_lock lock(windowsMutex);
        auto it = std::find_if(windows.begin(), windows.end(),
            [hwnd](const std::unique_ptr<WindowData>& w) {
                return w && w->hwnd == hwnd;
            });
        return (it != windows.end()) ? it->get() : nullptr;
    }

    const WindowData* MultiContextManager::findWindowByHWND(HWND hwnd) const {
        std::scoped_lock lock(windowsMutex);
        auto it = std::find_if(windows.begin(), windows.end(),
            [hwnd](const std::unique_ptr<WindowData>& w) {
                return w && w->hwnd == hwnd;
            });
        return (it != windows.end()) ? it->get() : nullptr;
    }

    // ======================================================
    // Inline Implementations
    // ======================================================

    // ---------------- MultiContextManager (static) ----------------
    //inline std::shared_ptr<core::Context> MultiContextManager::currentContext = nullptr;

    inline void MultiContextManager::SetCurrent(std::shared_ptr<core::Context> ctx) {
        currentContext = std::move(ctx); }

    inline std::shared_ptr<almondnamespace::core::Context> almondnamespace::core::MultiContextManager::GetCurrent() {
        return currentContext; }

    // ---------------- MultiContextManager (public helpers) ----------------
    inline bool MultiContextManager::IsRunning() const noexcept {
        return running.load(std::memory_order_acquire); }

    inline void MultiContextManager::StopRunning() noexcept {
        running.store(false, std::memory_order_release); }

    inline void MultiContextManager::EnqueueRenderCommand(HWND hwnd, RenderCommand cmd) {
        std::scoped_lock lock(windowsMutex);
        auto it = std::find_if(windows.begin(), windows.end(),
            [hwnd](const std::unique_ptr<WindowData>& w) { return w->hwnd == hwnd; });
        if (it != windows.end()) {
            (*it)->EnqueueCommand(std::move(cmd));
        }
    }

    // -----------------------------------------------------------------
    // Implementation
    // -----------------------------------------------------------------
    void MultiContextManager::ShowConsole()
    {
        if (AllocConsole()) {
            FILE* f;
            freopen_s(&f, "CONOUT$", "w", stdout);
            freopen_s(&f, "CONIN$", "r", stdin);
            freopen_s(&f, "CONOUT$", "w", stderr);
            std::ios::sync_with_stdio(true);
        }
    }

    ATOM MultiContextManager::RegisterParentClass(HINSTANCE hInst, LPCWSTR name)
    {
        WNDCLASS wc{};
        wc.lpfnWndProc = ParentProc;
        wc.hInstance = hInst;
        wc.lpszClassName = name;
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        return RegisterClass(&wc);
    }

    ATOM MultiContextManager::RegisterChildClass(HINSTANCE hInst, LPCWSTR name)
    {
        WNDCLASS wc{};
        wc.lpfnWndProc = ChildProc;
        wc.hInstance = hInst;
        wc.lpszClassName = name;
        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        return RegisterClass(&wc);
    }

    void MultiContextManager::SetupPixelFormat(HDC hdc)
    {
        PIXELFORMATDESCRIPTOR pfd{};
        pfd.nSize = sizeof(pfd);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 32;
        pfd.cDepthBits = 24;
        pfd.iLayerType = PFD_MAIN_PLANE;

        int pf = ChoosePixelFormat(hdc, &pfd);
        if (pf == 0 || !SetPixelFormat(hdc, pf, &pfd))
            throw std::runtime_error("Failed to set pixel format");
    }

    HGLRC MultiContextManager::CreateSharedGLContext(HDC hdc)
    {
        SetupPixelFormat(hdc);
        HGLRC ctx = wglCreateContext(hdc);
        if (!ctx) throw std::runtime_error("Failed to create OpenGL context");
        if (sharedContext && !wglShareLists(sharedContext, ctx)) {
            wglDeleteContext(ctx);
            throw std::runtime_error("Failed to share GL context");
        }
        return ctx;
    }

    //bool almondnamespace::core::MultiContextManager::ProcessBackend(ContextType type) {
    //    auto it = g_backends.find(type);
    //    if (it == g_backends.end()) return false;

    //    auto& state = it->second;
    //    bool anyRunning = false;

    //    if (state.master && state.master->process) {
    //        anyRunning |= state.master->process_safe(*state.master);
    //    }

    //    for (auto& dup : state.duplicates) {
    //        if (dup && dup->process) {
    //            anyRunning |= dup->process_safe(*dup);
    //        }
    //    }

    //    return anyRunning;
    //}

    inline int MultiContextManager::get_title_bar_thickness(const HWND window_handle)
    {
        RECT window_rectangle{}, client_rectangle{};
        GetWindowRect(window_handle, &window_rectangle);
        GetClientRect(window_handle, &client_rectangle);

        int totalWidth = window_rectangle.right - window_rectangle.left;
        int totalHeight = window_rectangle.bottom - window_rectangle.top;
        int clientWidth = client_rectangle.right - client_rectangle.left;
        int clientHeight = client_rectangle.bottom - client_rectangle.top;

        int nonClientHeight = totalHeight - clientHeight; // title bar + borders (top+bottom)
        int nonClientWidth = totalWidth - clientWidth;   // vertical borders (left+right)

        int borderThickness = nonClientWidth / 2;          // assume symmetrical
        int titleBarHeight = nonClientHeight - borderThickness * 2;

#if defined(DEBUG_WINDOW_VERBOSE)
        std::cout << "Window size:  " << totalWidth << "x" << totalHeight << " px\n";
        std::cout << "Client size:  " << clientWidth << "x" << clientHeight << " px\n";
        std::cout << "Non-client H: " << nonClientHeight
            << " (title bar + top/bottom borders)\n";
        std::cout << "Non-client W: " << nonClientWidth
            << " (left+right borders total)\n";
        std::cout << "Border thickness: " << borderThickness << " px each side\n";
        std::cout << "Title bar height: " << titleBarHeight << " px\n";
#endif
        return titleBarHeight;
    }

    // ----------  MultiContext Manager Initialize  -------------------------------------------------------
    bool MultiContextManager::Initialize(HINSTANCE hInst,
        int RayLibWinCount, int SDLWinCount, int SFMLWinCount,
        int OpenGLWinCount, int SoftwareWinCount, bool parented)
    {
        const int totalRequested =
            RayLibWinCount + SDLWinCount + SFMLWinCount + OpenGLWinCount + SoftwareWinCount;
        if (totalRequested <= 0) return false;

        RegisterParentClass(hInst, L"AlmondParent");
        RegisterChildClass(hInst, L"AlmondChild");

        // ---------------- Parent (dock container) ----------------
        if (parented) {
            int cols = 1, rows = 1;
            while (cols * rows < totalRequested) (cols <= rows ? ++cols : ++rows);

            int cellW = (totalRequested == 1) ? cli::window_width : 400;
            int cellH = (totalRequested == 1) ? cli::window_height : 300;

            int clientW = cols * cellW;
            int clientH = rows * cellH;

            RECT want = { 0, 0, clientW, clientH };
            DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
            AdjustWindowRect(&want, style, FALSE);

            parent = CreateWindowEx(
                0, L"AlmondParent", L"Almond Docking",
                style,
                CW_USEDEFAULT, CW_USEDEFAULT,
                want.right - want.left, want.bottom - want.top,
                nullptr, nullptr, hInst, this);

            if (!parent) return false;

            DragAcceptFiles(parent, TRUE);
        }
        else {
            parent = nullptr;
        }

#ifdef ALMOND_USING_OPENGL
        // ---------------- Shared dummy GL context (for wglShareLists) ----------------
        {
            HWND dummy = CreateWindowEx(WS_EX_TOOLWINDOW, L"AlmondChild", L"Dummy",
                WS_POPUP, 0, 0, 1, 1, nullptr, nullptr, hInst, nullptr);
            if (!dummy) return false;

            HDC dummyDC = GetDC(dummy);
            SetupPixelFormat(dummyDC);
            sharedContext = wglCreateContext(dummyDC);
            if (!sharedContext) {
                ReleaseDC(dummy, dummyDC);
                DestroyWindow(dummy);
                throw std::runtime_error("Failed to create shared OpenGL context");
            }
            wglMakeCurrent(dummyDC, sharedContext);

            static bool gladInitialized = false;
            if (!gladInitialized) {
                gladInitialized = gladLoadGL();
                std::cerr << "[Init] GLAD loaded on dummy context\n";
            }
            wglMakeCurrent(nullptr, nullptr);
            ReleaseDC(dummy, dummyDC);
            DestroyWindow(dummy);
        }
#endif

        // ---------------- Helper: create N windows for a backend ----------------
        auto make_backend_windows = [&](ContextType type, int count) {
            if (count <= 0) return;

            std::vector<HWND> created;
            created.reserve(count);

            for (int i = 0; i < count; ++i) {
                HWND hwnd = CreateWindowEx(
                    0, L"AlmondChild", L"",
                    (parent ? WS_CHILD | WS_VISIBLE : WS_OVERLAPPEDWINDOW | WS_VISIBLE),
                    0, 0, 10, 10,
                    parent, nullptr, hInst, nullptr);

                if (!hwnd) continue;

                HDC hdc = GetDC(hwnd);
                HGLRC glrc = nullptr;

#ifdef ALMOND_USING_OPENGL
                if (type == ContextType::OpenGL) {
                    glrc = CreateSharedGLContext(hdc);
                }
#endif

                auto winPtr = std::make_unique<WindowData>(hwnd, hdc, glrc, true, type);
                if (parent) MakeDockable(hwnd, parent);
                {
                    std::scoped_lock lock(windowsMutex);
                    windows.emplace_back(std::move(winPtr));
                }
                created.push_back(hwnd);
            }

            // Wire contexts to the windows in order: master first, then duplicates
            BackendState& state = g_backends[type];

            if (!state.master) {
                state.master = std::make_shared<Context>();
                for (int i = 1; i < count; ++i)
                    state.duplicates.push_back(std::make_shared<Context>());
            }

            std::vector<std::shared_ptr<Context>> ctxs;
            ctxs.push_back(state.master);
            for (auto& d : state.duplicates) ctxs.push_back(d);

            const size_t n = std::min(created.size(), ctxs.size());
            for (size_t i = 0; i < n; ++i) {
                HWND hwnd = created[i];
                auto* w = findWindowByHWND(hwnd);
                auto& ctx = ctxs[i];

                ctx->type = type;
                ctx->hwnd = hwnd;
                if (w) {
                    ctx->hdc = w->hdc;
                    ctx->hglrc = w->glContext;
                    w->context = ctx;
                }

                RECT rc{};
                if (parent) GetClientRect(parent, &rc);
                else GetClientRect(hwnd, &rc);
                ctx->width = std::max<LONG>(1, rc.right - rc.left);
                ctx->height = std::max<LONG>(1, rc.bottom - rc.top);

                // ---------------- Immediate backend initialization ----------------
                switch (type) {
#ifdef ALMOND_USING_OPENGL
                case ContextType::OpenGL:
                    if (ctx->hdc && ctx->hglrc) {
                        if (!wglMakeCurrent(ctx->hdc, ctx->hglrc)) {
                            std::cerr << "[Init] wglMakeCurrent failed for hwnd=" << hwnd << "\n";
                        }
                        else {
                            std::cerr << "[Init] Running OpenGL init for hwnd=" << hwnd << "\n";
                            almondnamespace::openglcontext::opengl_initialize(
                                ctx, hwnd, ctx->width, ctx->height, w->onResize);
                            wglMakeCurrent(nullptr, nullptr);
                        }
                    }
                    break;
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
                case ContextType::Software:
                    std::cerr << "[Init] Initializing Software renderer for hwnd=" << hwnd << "\n";
                    almondnamespace::anativecontext::softrenderer_initialize(ctx);
                    break;
#endif
#ifdef ALMOND_USING_SDL
                case ContextType::SDL:
                    std::cerr << "[Init] Initializing SDL context for hwnd=" << hwnd << "\n";
                    almondnamespace::sdlcontext::sdl_initialize(ctx);
                    break;
#endif
#ifdef ALMOND_USING_SFML
                case ContextType::SFML:
                    std::cerr << "[Init] Initializing SFML context for hwnd=" << hwnd << "\n";
                    almondnamespace::sfmlcontext::sfml_initialize(ctx);
                    break;
#endif
#ifdef ALMOND_USING_RAYLIB
                case ContextType::RayLib:
                    std::cerr << "[Init] Initializing RayLib context for hwnd=" << hwnd << "\n";
                    almondnamespace::raylibcontext::raylib_initialize(ctx);
                    break;
#endif
                default:
                    break;
                }
            }
            };

#ifdef ALMOND_USING_RAYLIB
        make_backend_windows(ContextType::RayLib, RayLibWinCount);
#endif
#ifdef ALMOND_USING_SDL
        make_backend_windows(ContextType::SDL, SDLWinCount);
#endif
#ifdef ALMOND_USING_SFML
        make_backend_windows(ContextType::SFML, SFMLWinCount);
#endif
#ifdef ALMOND_USING_OPENGL
        make_backend_windows(ContextType::OpenGL, OpenGLWinCount);
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
        make_backend_windows(ContextType::Software, SoftwareWinCount);
#endif

        ArrangeDockedWindowsGrid();
        return !g_backends.empty();
    }

    //void almondnamespace::core::MultiContextManager::AddWindow(HWND hwnd, HDC hdc, HGLRC glContext,
    //    bool usesSharedContext, ResizeCallback onResize) {
    //    windows.emplace_back(hwnd, hdc, glContext, usesSharedContext);
    //    windows.back().onResize = std::move(onResize);
    //    if (!hdc) MakeDockable(hwnd, parent);
    //}

    // ======================================================
    // MultiContextManager : Implementations
    // ======================================================
    void MultiContextManager::AddWindow(
        HWND hwnd,
        HWND parent,
        HDC hdc,
        HGLRC glContext,
        bool usesSharedContext,
        ResizeCallback onResize,
        ContextType type)
    {
        if (!hwnd) return;

        if (!hdc) hdc = GetDC(hwnd);

        // Only create GL context if this window is OpenGL
        if (type == ContextType::OpenGL) {
#ifdef ALMOND_USING_OPENGL
            if (!glContext) {
                SetupPixelFormat(hdc);
                glContext = CreateSharedGLContext(hdc);

                static bool gladInitialized = false;
                if (!gladInitialized) {
                    wglMakeCurrent(hdc, glContext);
                    gladInitialized = gladLoadGL();
                    wglMakeCurrent(nullptr, nullptr);
                }
            }
#endif
        }

        // Make window dockable if a parent is provided
        if (parent) MakeDockable(hwnd, parent);

        // --- Create Context object ---
        auto ctx = std::make_shared<core::Context>();
        ctx->hwnd = hwnd;
        ctx->hdc = hdc;
        ctx->hglrc = glContext;
        ctx->type = type;
        //ctx->running = true;

        // Prepare WindowData (unique_ptr)
        auto winPtr = std::make_unique<WindowData>(hwnd, hdc, glContext, usesSharedContext, type);
        winPtr->onResize = std::move(onResize);
        winPtr->context = ctx;  // <-- hook it up
        ctx->windowData = winPtr.get(); // set the back-pointer immediately

        // Keep raw pointer for thread use
        WindowData* rawWinPtr = winPtr.get();

        {
            std::scoped_lock lock(windowsMutex);
            windows.emplace_back(std::move(winPtr));
        }

        // Launch render thread if needed
        if (!gThreads.contains(hwnd) && rawWinPtr) {
            gThreads[hwnd] = std::thread([this, rawWinPtr]() {
                RenderLoop(*rawWinPtr);
                });
        }

        ArrangeDockedWindowsGrid();
    }


    void MultiContextManager::StartRenderThreads() {
        std::scoped_lock lock(windowsMutex); // lock while grabbing pointers
        for (const auto& w : windows) {
            if (!gThreads.contains(w->hwnd)) {
#if ALMOND_SINGLE_PARENT
                // Only spawn threads for windows that exist and are valid
                gThreads[w->hwnd] = std::thread([this, hwnd = w->hwnd]() {
                    auto it = std::find_if(windows.begin(), windows.end(),
                        [hwnd](const std::unique_ptr<WindowData>& w) { return w->hwnd == hwnd; });
                    if (it != windows.end()) RenderLoop(**it);
                    });
#else
                // Fully independent: spawn thread for every window
                gThreads[w->hwnd] = std::thread([this, hwnd = w->hwnd]() {
                    auto it = std::find_if(windows.begin(), windows.end(),
                        [hwnd](const std::unique_ptr<WindowData>& w) { return w->hwnd == hwnd; });
                    if (it != windows.end()) RenderLoop(**it);
                    });
#endif
            }
        }
    }

    void MultiContextManager::RemoveWindow(HWND hwnd)
    {
        std::unique_ptr<WindowData> removedWindow;

        {
            std::scoped_lock lock(windowsMutex);

            // Find the window
            auto it = std::find_if(
                windows.begin(), windows.end(),
                [hwnd](const std::unique_ptr<WindowData>& w) {
                    return w && w->hwnd == hwnd;
                });

            if (it == windows.end())
                return;

            // Mark the window as no longer running
            (*it)->running = false;

            // Take ownership of the unique_ptr out of the vector
            removedWindow = std::move(*it);
            windows.erase(it);
        }

        // Ensure the render thread is stopped before destroying the context
        if (gThreads.contains(hwnd)) {
            if (gThreads[hwnd].joinable()) {
                gThreads[hwnd].join();
            }
            gThreads.erase(hwnd);
        }

        // Clean up OpenGL / DC (safe now, since thread is joined)
        if (removedWindow) {
            if (removedWindow->glContext) {
                wglMakeCurrent(nullptr, nullptr);
                wglDeleteContext(removedWindow->glContext);
            }
            if (removedWindow->hdc && removedWindow->hwnd) {
                ReleaseDC(removedWindow->hwnd, removedWindow->hdc);
            }
            // unique_ptr destructor cleans up WindowData
        }
    }

    // ---------------- Docking Layout ----------------

    // Original version (simple equal grid):
    // void almondnamespace::core::MultiContextManager::ArrangeDockedWindowsGrid() {
    //     if (!parent || windows.empty()) return;
    //     RECT rc; GetClientRect(parent, &rc);
    //     int cols = 1, rows = 1;
    //     while (cols * rows < (int)windows.size())
    //         (cols <= rows ? ++cols : ++rows);
    //     int cw = rc.right / cols, ch = rc.bottom / rows;
    //     for (size_t i = 0; i < windows.size(); ++i) {
    //         int c = int(i) % cols, r = int(i) / cols;
    //         SetWindowPos(windows[i]->hwnd, nullptr, c * cw, r * ch, cw, ch,
    //             SWP_NOZORDER | SWP_NOACTIVATE);
    //         if (windows[i]->onResize) windows[i]->onResize(cw, ch);
    //     }
    // }

    // Alternate version (resize parent to fit grid):
    // void almondnamespace::core::MultiContextManager::ArrangeDockedWindowsGrid() {
    //     if (!parent || windows.empty()) return;
    //
    //     constexpr int minCellWidth = 400;
    //     constexpr int minCellHeight = 300;
    //
    //     int cols = 1, rows = 1;
    //     while (cols * rows < (int)windows.size())one error is vector subscript out of range
    //         (cols <= rows ? ++cols : ++rows);
    //
    //     int totalWidth = cols * minCellWidth;
    //     int totalHeight = rows * minCellHeight;
    //
    //     SetWindowPos(parent, nullptr, 0, 0, totalWidth, totalHeight,
    //         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    //
    //     RECT rc; GetClientRect(parent, &rc);
    //     int cw = rc.right / cols;
    //     int ch = rc.bottom / rows;
    //
    //     for (size_t i = 0; i < windows.size(); ++i) {
    //         int c = int(i) % cols;
    //         int r = int(i) / cols;
    //         SetWindowPos(windows[i]->hwnd, nullptr, c * cw, r * ch, cw, ch,
    //             SWP_NOZORDER | SWP_NOACTIVATE);
    //         if (windows[i]->onResize) windows[i]->onResize(cw, ch);
    //     }
    // }

    // Final active version (your current one):
    void MultiContextManager::ArrangeDockedWindowsGrid() {
        if (!parent || windows.empty()) return;

        // Compute grid
        int total = static_cast<int>(windows.size());
        int cols = 1, rows = 1;
        while (cols * rows < total) (cols <= rows ? ++cols : ++rows);

        // Parent client area
        RECT rcClient{};
        GetClientRect(parent, &rcClient);
        int clientW = rcClient.right - rcClient.left;
        int clientH = rcClient.bottom - rcClient.top;

        int cw = std::max(1, clientW / cols);
        int ch = std::max(1, clientH / rows);

        // Place each child in its cell
        for (size_t i = 0; i < windows.size(); ++i) {
            int c = static_cast<int>(i) % cols;
            int r = static_cast<int>(i) / cols;

            WindowData& win = *windows[i];
            SetWindowPos(win.hwnd, nullptr, c * cw, r * ch, cw, ch,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_SHOWWINDOW);

            //win.width = cw;
            //win.height = ch;
            if (win.onResize) win.onResize(cw, ch);
        }
    }

    void MultiContextManager::StopAll() {
        running = false;

		{ // mutex scope
            std::scoped_lock lock(windowsMutex);
            for (auto& w : windows) {
                if (w) w->running = false;
            }
        }

        for (auto& [hwnd, th] : gThreads) {
            if (th.joinable()) th.join();
        }
        gThreads.clear();
    }

    void MultiContextManager::RenderLoop(WindowData& win) {
        // Make context aware of its owning window
        if (win.context) {
            win.context->windowData = &win;
        }

        // Ensure per-thread initialization flags are outside switch
        static thread_local bool glInit = false;
        static thread_local bool swInit = false;
        static thread_local bool sdlInit = false;
        static thread_local bool sfmlInit = false;
        static thread_local bool rayInit = false;

        switch (win.type) {
        case ContextType::OpenGL:
#ifdef ALMOND_USING_OPENGL
            if (!wglMakeCurrent(win.hdc, win.glContext)) return;

            if (!glInit && win.context) {
                std::cerr << "[OpenGL] Initializing VAO/VBO/EBO for hwnd=" << win.hwnd << "\n";
                almondnamespace::openglcontext::opengl_initialize(
                    win.context,      // shared_ptr<Context>
                    win.hwnd,         // HWND
                    win.width,
                    win.height,
                    win.onResize
                );
                glInit = true;
            }

            while (running && win.running) {
                if (win.context) {
                    if (!almondnamespace::openglcontext::opengl_process(*win.context,
                        win.commandQueue)) {
                        std::cerr << "[OpenGL] process returned false\n";
                        break;
                    }
                  //  std::cout << "[OpenGL] Processed context for hwnd=" << win.hwnd << "\n";
                }
                else {
                    glClearColor(1.f, 0.f, 1.f, 1.f); // magenta fallback
                    glClear(GL_COLOR_BUFFER_BIT);
                    SwapBuffers(win.hdc);
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
            wglMakeCurrent(nullptr, nullptr);
#endif
            break;

        case ContextType::Software:
#ifdef ALMOND_USING_SOFTWARE_RENDERER
            if (!swInit && win.context) {
                std::cerr << "[Software] Initializing renderer for hwnd=" << win.hwnd << "\n";
                almondnamespace::anativecontext::softrenderer_initialize(win.context);
                swInit = true;
            }

            while (running && win.running) {
                if (win.context) {
                    if (!almondnamespace::anativecontext::softrenderer_process(*win.context,
                        win.commandQueue)) {
                        std::cerr << "[Software] process returned false\n";
                        break;
                    }
                   // std::cout << "[Software] Processed context for hwnd=" << win.hwnd << "\n";
                }
                else {
                    std::this_thread::sleep_for(std::chrono::milliseconds(16));
                }
            }
#endif
            break;

        case ContextType::SDL:
#ifdef ALMOND_USING_SDL
            if (!sdlInit && win.context) {
                std::cerr << "[SDL] Initializing context for hwnd=" << win.hwnd << "\n";
                almondnamespace::sdlcontext::sdl_initialize(win.context);
                sdlInit = true;
            }

            while (running && win.running) {
                if (win.context) {
                    if (!almondnamespace::sdlcontext::sdl_process(*win.context,
                        win.commandQueue)) {
                        std::cerr << "[SDL] process returned false\n";
                        break;
                    }
                  //  std::cout << "[SDL] Processed context for hwnd=" << win.hwnd << "\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
#endif
            break;

        case ContextType::SFML:
#ifdef ALMOND_USING_SFML
            if (!sfmlInit && win.context) {
                std::cerr << "[SFML] Initializing context for hwnd=" << win.hwnd << "\n";
                almondnamespace::sfmlcontext::sfml_initialize(win.context);
                sfmlInit = true;
            }

            while (running && win.running) {
                if (win.context) {
                    if (!almondnamespace::sfmlcontext::sfml_process(*win.context,
                        win.commandQueue)) {
                        std::cerr << "[SFML] process returned false\n";
                        break;
                    }
                //    std::cout << "[SFML] Processed context for hwnd=" << win.hwnd << "\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
#endif
            break;

        case ContextType::RayLib:
#ifdef ALMOND_USING_RAYLIB
            if (!rayInit && win.context) {
                std::cerr << "[RayLib] Initializing context for hwnd=" << win.hwnd << "\n";
                almondnamespace::raylibcontext::raylib_initialize(win.context);
                rayInit = true;
            }

            while (running && win.running) {
                if (win.context) {
                    if (!almondnamespace::raylibcontext::raylib_process(*win.context,
                        win.commandQueue)) {
                        std::cerr << "[RayLib] process returned false\n";
                        break;
                    }
                 //   std::cout << "[RayLib] Processed context for hwnd=" << win.hwnd << "\n";
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(16));
            }
#endif
            break;

        case ContextType::Custom:
            // TODO: custom backend loop
            break;

        case ContextType::Noop:
        default:
            break;
        }
    }


    void MultiContextManager::HandleDropFiles(HWND hwnd, HDROP hDrop) {
        UINT count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
        for (UINT i = 0; i < count; ++i) {
            wchar_t path[MAX_PATH]{};
            DragQueryFile(hDrop, i, path, MAX_PATH);
            std::wcout << L"[Drop] " << path << L"\n";
        }
    }

    LRESULT CALLBACK almondnamespace::core::MultiContextManager::ParentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (msg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA,
                reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }
        auto* mgr = reinterpret_cast<MultiContextManager*>(
            GetWindowLongPtr(hwnd, GWLP_USERDATA));
        if (!mgr) return DefWindowProc(hwnd, msg, wParam, lParam);

        switch (msg) {
            //case WM_SIZE: mgr->ArrangeDockedWindowsGrid(); return 0;
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps); return 0;
        }
        case WM_DROPFILES:
            mgr->HandleDropFiles(hwnd, reinterpret_cast<HDROP>(wParam));
            DragFinish(reinterpret_cast<HDROP>(wParam));
            return 0;
        case WM_CLOSE: DestroyWindow(hwnd); return 0;
        case WM_DESTROY:
            if (hwnd == mgr->GetParentWindow()) {
                PostQuitMessage(0);
            }
            return 0;

        default: return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }

    LRESULT CALLBACK MultiContextManager::ChildProc(HWND hwnd, UINT msg,
        WPARAM wParam, LPARAM lParam) {
        static DragState& drag = gDragState;
        if (msg == WM_NCCREATE) {
            auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
            return TRUE;
        }

        switch (msg) {
        case WM_LBUTTONDOWN: {
            SetCapture(hwnd);
            drag.dragging = true;
            drag.draggedWindow = hwnd;
            drag.originalParent = GetParent(hwnd);
            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ClientToScreen(hwnd, &pt);
            drag.lastMousePos = pt;
            return 0;
        }
        case WM_MOUSEMOVE: {
            if (!drag.dragging || drag.draggedWindow != hwnd)
                return DefWindowProc(hwnd, msg, wParam, lParam);

            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            ClientToScreen(hwnd, &pt);
            int dx = pt.x - drag.lastMousePos.x;
            int dy = pt.y - drag.lastMousePos.y;
            drag.lastMousePos = pt;

            RECT wndRect; GetWindowRect(hwnd, &wndRect);
            int newX = wndRect.left + dx;
            int newY = wndRect.top + dy;

            if (drag.originalParent) {
                RECT prc; GetClientRect(drag.originalParent, &prc);
                POINT tl{ 0, 0 }; ClientToScreen(drag.originalParent, &tl);
                OffsetRect(&prc, tl.x, tl.y);

                bool inside =
                    newX >= prc.left && newY >= prc.top &&
                    (newX + (wndRect.right - wndRect.left)) <= prc.right &&
                    (newY + (wndRect.bottom - wndRect.top)) <= prc.bottom;

                if (inside) {
                    if (GetParent(hwnd) != drag.originalParent) {
                        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
                        style &= ~(WS_POPUP | WS_OVERLAPPEDWINDOW);
                        style |= WS_CHILD;
                        SetWindowLongPtr(hwnd, GWL_STYLE, style);
                        SetParent(hwnd, drag.originalParent);

                        POINT cp{ newX, newY };
                        ScreenToClient(drag.originalParent, &cp);
                        SetWindowPos(hwnd, nullptr, cp.x, cp.y,
                            wndRect.right - wndRect.left,
                            wndRect.bottom - wndRect.top,
                            SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                    }
                    else {
                        POINT cp{ newX, newY };
                        ScreenToClient(drag.originalParent, &cp);
                        SetWindowPos(hwnd, nullptr, cp.x, cp.y,
                            0, 0, SWP_NOZORDER | SWP_NOSIZE |
                            SWP_NOACTIVATE);
                    }
                }
                else {
                    if (GetParent(hwnd) == drag.originalParent) {
                        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
                        style &= ~WS_CHILD;
                        style |= WS_OVERLAPPEDWINDOW | WS_VISIBLE;
                        SetWindowLongPtr(hwnd, GWL_STYLE, style);
                        SetParent(hwnd, nullptr);
                        SetWindowPos(hwnd, nullptr, newX, newY,
                            wndRect.right - wndRect.left,
                            wndRect.bottom - wndRect.top,
                            SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
                    }
                    else {
                        SetWindowPos(hwnd, nullptr, newX, newY,
                            0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
                    }
                }
            }
            else {
                SetWindowPos(hwnd, nullptr, newX, newY,
                    0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
            }
            return 0;
        }
        case WM_LBUTTONUP: {
            if (drag.dragging && drag.draggedWindow == hwnd) {
                ReleaseCapture();
                drag.dragging = false;
                drag.draggedWindow = nullptr;
                drag.originalParent = nullptr;
            }
            return 0;
        }
        case WM_DROPFILES: {
            if (HWND p = GetParent(hwnd))
                SendMessage(p, WM_DROPFILES, wParam, lParam);
            DragFinish(reinterpret_cast<HDROP>(wParam));
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
            EndPaint(hwnd, &ps);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
        }
    }
} // namespace almondnamespace::core

