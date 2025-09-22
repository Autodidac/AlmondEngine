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
 // acontextmultiplexer.hpp
#pragma once

#include "aplatform.hpp"        // Must always come first for platform defines
#include "aengineconfig.hpp"   // All ENGINE-specific includes

#include "acontext.hpp"       // Context, ContextType
#include "acontexttype.hpp"
#include "aopenglcontext.hpp" // RendererContext, RenderMode
#include "asdlcontext.hpp"    // SDLContext
#include "asfmlcontext.hpp"   // SFMLContext
#include "araylibcontext.hpp" // RaylibContext
#include "asoftrenderer_context.hpp" // SoftwareContext

//#include <windows.h>
//#include <windowsx.h>
//#include <shellapi.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include <atomic>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>
#include <unordered_map>

//#include <glad/glad.h>

//#define ALMOND_SINGLE_PARENT 1  // 0 = multiple top-level windows, 1 = single parent + children
#define ALMOND_SHARED_CONTEXT 1  // Force shared OpenGL context

#if !defined(ALMOND_SINGLE_PARENT) || (ALMOND_SINGLE_PARENT == 0)
#undef ALMOND_SHARED_CONTEXT
#define ALMOND_SHARED_CONTEXT 0
#else
#define ALMOND_USING_DOCKING 1  // Enable docking features
#define ALMOND_SHARED_CONTEXT 1 // Enable shared OpenGL context
#endif
namespace almondnamespace::core
{
    // -----------------------------------------------------------------
    // Global thread table (key = HWND)
    // -----------------------------------------------------------------
    static std::unordered_map<HWND, std::thread> gThreads;

    // -----------------------------------------------------------------
    // Drag state (for child window docking/movement)
    // -----------------------------------------------------------------
    struct DragState {
        bool dragging = false;
        POINT lastMousePos{};
        HWND draggedWindow = nullptr;
        HWND originalParent = nullptr;
    };
    static DragState gDragState;

    // ======================================================
    // MultiContextManager : Main orchestrator
    // ======================================================
    struct WindowData; // Forward declaration
    struct CommandQueue; // Forward declaration
    class MultiContextManager
    {
    public:
        // ---- Lifecycle ----
        static void ShowConsole();
        bool Initialize(HINSTANCE hInst,
            int RayLibWinCount = 0,
            int SDLWinCount = 0,
            int SFMLWinCount = 0,
            int OpenGLWinCount = 0,
            int SoftwareWinCount = 0,
            bool parented = ALMOND_SINGLE_PARENT == 1);

        void StopAll();
        bool IsRunning() const noexcept;
        void StopRunning() noexcept;

        // ---- Window / Context Management ----
        using ResizeCallback = std::function<void(int, int)>;
        void AddWindow(HWND hwnd, HWND parent, HDC hdc, HGLRC glContext,
            bool usesSharedContext,
            ResizeCallback onResize,
            ContextType type);
        void RemoveWindow(HWND hwnd);
        void ArrangeDockedWindowsGrid();
        void StartRenderThreads();

        HWND GetParentWindow() const { return parent; }
        const std::vector<std::unique_ptr<WindowData>>& GetWindows() const { return windows; }

        // ---- Render Commands ----
        using RenderCommand = std::function<void()>;
        void EnqueueRenderCommand(HWND hwnd, MultiContextManager::RenderCommand cmd);

        // ---- Context API ----
        static void SetCurrent(std::shared_ptr<core::Context> ctx);
        static std::shared_ptr<core::Context> GetCurrent();

        // ---- Windows Message Handling ----
        static LRESULT CALLBACK ParentProc(HWND, UINT, WPARAM, LPARAM);
        static LRESULT CALLBACK ChildProc(HWND, UINT, WPARAM, LPARAM);
        void HandleDropFiles(HWND, HDROP);

        void InitializeAllContexts();
        static ATOM RegisterParentClass(HINSTANCE, LPCWSTR);
        static ATOM RegisterChildClass(HINSTANCE, LPCWSTR);

        WindowData* findWindowByHWND(HWND hwnd);
        const WindowData* findWindowByHWND(HWND hwnd) const;
    private:
        // ---- Internal State ----
        std::vector<std::unique_ptr<WindowData>> windows;
        std::atomic<bool> running{ true };
        mutable std::mutex windowsMutex;


        HGLRC sharedContext = nullptr;
        HWND  parent = nullptr;

        // ---- Internals ----
        void RenderLoop(WindowData& win);
        void SetupPixelFormat(HDC hdc);
        HGLRC CreateSharedGLContext(HDC hdc);
        //bool ProcessBackend(ContextType type);
        int get_title_bar_thickness(const HWND window_handle);

        // ---- Static Shared ----
        inline static std::shared_ptr<core::Context> currentContext;
    };

    // ======================================================
    // WindowData : Per-context state holder
    // ======================================================
    //struct WindowData
    //{
    //    HWND hwnd{};
    //    HDC  hdc{};
    //    HGLRC glContext{};
    //    bool running = true;
    //    bool clicked = false;
    //    bool usesSharedContext = false;
    //    std::shared_ptr<core::Context> context;
    //    ContextType type{ ContextType::OpenGL };

    //    int width = 0;   
    //    int height = 0;  

    //    MultiContextManager::ResizeCallback onResize;
    //    CommandQueue commandQueue;

    //    WindowData() = default;
    //    WindowData(HWND h, HDC dc, HGLRC ctx, bool shared, ContextType t)
    //        : hwnd(h), hdc(dc), glContext(ctx), usesSharedContext(shared), type(t) {
    //    }

    //    // Non-copyable, non-movable
    //    WindowData(const WindowData&) = delete;
    //    WindowData& operator=(const WindowData&) = delete;
    //    WindowData(WindowData&&) = delete;
    //    WindowData& operator=(WindowData&&) = delete;

    //    // Forwarding convenience
    //    void EnqueueCommand(MultiContextManager::RenderCommand cmd) {
    //        commandQueue.enqueue(std::move(cmd));
    //    }

    //    bool DrainCommands() {
    //        return commandQueue.drain();
    //    }
    //};
}
//    almondnamespace::core::WindowData* almondnamespace::core::MultiContextManager::findWindowByHWND(HWND hwnd) {
//        std::scoped_lock lock(windowsMutex);
//        auto it = std::find_if(windows.begin(), windows.end(),
//            [hwnd](const std::unique_ptr<WindowData>& w) {
//                return w && w->hwnd == hwnd;
//            });
//        return (it != windows.end()) ? it->get() : nullptr;
//    }
//
//    const almondnamespace::core::WindowData* almondnamespace::core::MultiContextManager::findWindowByHWND(HWND hwnd) const {
//        std::scoped_lock lock(windowsMutex);
//        auto it = std::find_if(windows.begin(), windows.end(),
//            [hwnd](const std::unique_ptr<WindowData>& w) {
//                return w && w->hwnd == hwnd;
//            });
//        return (it != windows.end()) ? it->get() : nullptr;
//    }
//
//    // ======================================================
//    // Inline Implementations
//    // ======================================================
//
//    // ---------------- MultiContextManager (static) ----------------
//    //inline std::shared_ptr<core::Context> MultiContextManager::currentContext = nullptr;
//
//    inline void almondnamespace::core::MultiContextManager::SetCurrent(std::shared_ptr<core::Context> ctx)
//    {
//        currentContext = std::move(ctx);
//    }
//
//    inline std::shared_ptr<almondnamespace::core::Context> almondnamespace::core::MultiContextManager::GetCurrent()
//    {
//        return currentContext;
//    }
//
//    // ---------------- MultiContextManager (public helpers) ----------------
//    inline bool almondnamespace::core::MultiContextManager::IsRunning() const noexcept {
//        return running.load(std::memory_order_acquire);
//    }
//
//    inline void almondnamespace::core::MultiContextManager::StopRunning() noexcept {
//        running.store(false, std::memory_order_release);
//    }
//
//    inline void almondnamespace::core::MultiContextManager::EnqueueRenderCommand(HWND hwnd, RenderCommand cmd) {
//        std::scoped_lock lock(windowsMutex);
//        auto it = std::find_if(windows.begin(), windows.end(),
//            [hwnd](const std::unique_ptr<WindowData>& w) { return w->hwnd == hwnd; });
//        if (it != windows.end()) {
//            (*it)->EnqueueCommand(std::move(cmd));
//        }
//    }
//
//    // -----------------------------------------------------------------
//    // Implementation
//    // -----------------------------------------------------------------
//    void almondnamespace::core::MultiContextManager::ShowConsole()
//    {
//        if (AllocConsole()) {
//            FILE* f;
//            freopen_s(&f, "CONOUT$", "w", stdout);
//            freopen_s(&f, "CONIN$", "r", stdin);
//            freopen_s(&f, "CONOUT$", "w", stderr);
//            std::ios::sync_with_stdio(true);
//        }
//    }
//
//    ATOM almondnamespace::core::MultiContextManager::RegisterParentClass(HINSTANCE hInst, LPCWSTR name)
//    {
//        WNDCLASS wc{};
//        wc.lpfnWndProc = ParentProc;
//        wc.hInstance = hInst;
//        wc.lpszClassName = name;
//        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
//        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//        return RegisterClass(&wc);
//    }
//
//    ATOM almondnamespace::core::MultiContextManager::RegisterChildClass(HINSTANCE hInst, LPCWSTR name)
//    {
//        WNDCLASS wc{};
//        wc.lpfnWndProc = ChildProc;
//        wc.hInstance = hInst;
//        wc.lpszClassName = name;
//        wc.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
//        wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
//        wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
//        return RegisterClass(&wc);
//    }
//
//    void almondnamespace::core::MultiContextManager::SetupPixelFormat(HDC hdc)
//    {
//        PIXELFORMATDESCRIPTOR pfd{};
//        pfd.nSize = sizeof(pfd);
//        pfd.nVersion = 1;
//        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//        pfd.iPixelType = PFD_TYPE_RGBA;
//        pfd.cColorBits = 32;
//        pfd.cDepthBits = 24;
//        pfd.iLayerType = PFD_MAIN_PLANE;
//
//        int pf = ChoosePixelFormat(hdc, &pfd);
//        if (pf == 0 || !SetPixelFormat(hdc, pf, &pfd))
//            throw std::runtime_error("Failed to set pixel format");
//    }
//
//    HGLRC almondnamespace::core::MultiContextManager::CreateSharedGLContext(HDC hdc)
//    {
//        SetupPixelFormat(hdc);
//        HGLRC ctx = wglCreateContext(hdc);
//        if (!ctx) throw std::runtime_error("Failed to create OpenGL context");
//        if (sharedContext && !wglShareLists(sharedContext, ctx)) {
//            wglDeleteContext(ctx);
//            throw std::runtime_error("Failed to share GL context");
//        }
//        return ctx;
//    }
//
//    //bool almondnamespace::core::MultiContextManager::ProcessBackend(ContextType type) {
//    //    auto it = g_backends.find(type);
//    //    if (it == g_backends.end()) return false;
//
//    //    auto& state = it->second;
//    //    bool anyRunning = false;
//
//    //    if (state.master && state.master->process) {
//    //        anyRunning |= state.master->process_safe(*state.master);
//    //    }
//
//    //    for (auto& dup : state.duplicates) {
//    //        if (dup && dup->process) {
//    //            anyRunning |= dup->process_safe(*dup);
//    //        }
//    //    }
//
//    //    return anyRunning;
//    //}
//
//    inline int almondnamespace::core::MultiContextManager::get_title_bar_thickness(const HWND window_handle)
//    {
//        RECT window_rectangle{}, client_rectangle{};
//        GetWindowRect(window_handle, &window_rectangle);
//        GetClientRect(window_handle, &client_rectangle);
//
//        int totalWidth = window_rectangle.right - window_rectangle.left;
//        int totalHeight = window_rectangle.bottom - window_rectangle.top;
//        int clientWidth = client_rectangle.right - client_rectangle.left;
//        int clientHeight = client_rectangle.bottom - client_rectangle.top;
//
//        int nonClientHeight = totalHeight - clientHeight; // title bar + borders (top+bottom)
//        int nonClientWidth = totalWidth - clientWidth;   // vertical borders (left+right)
//
//        int borderThickness = nonClientWidth / 2;          // assume symmetrical
//        int titleBarHeight = nonClientHeight - borderThickness * 2;
//
//#if defined(DEBUG_WINDOW_VERBOSE)
//        std::cout << "Window size:  " << totalWidth << "x" << totalHeight << " px\n";
//        std::cout << "Client size:  " << clientWidth << "x" << clientHeight << " px\n";
//        std::cout << "Non-client H: " << nonClientHeight
//            << " (title bar + top/bottom borders)\n";
//        std::cout << "Non-client W: " << nonClientWidth
//            << " (left+right borders total)\n";
//        std::cout << "Border thickness: " << borderThickness << " px each side\n";
//        std::cout << "Title bar height: " << titleBarHeight << " px\n";
//#endif
//        return titleBarHeight;
//    }
//
//    // ----------  MultiContext Manager Initialize  -------------------------------------------------------
//    bool almondnamespace::core::MultiContextManager::Initialize(HINSTANCE hInst, int RayLibWinCount, int SDLWinCount,
//        int SFMLWinCount, int OpenGLWinCount, int SoftwareWinCount, bool parented)
//    {
//        if (OpenGLWinCount <= 0 && SDLWinCount <= 0 && SFMLWinCount <= 0 && RayLibWinCount <= 0 && SoftwareWinCount <= 0) return false;  // Nothing to do
//
//        RegisterParentClass(hInst, L"AlmondParent");
//        RegisterChildClass(hInst, L"AlmondChild");
//
//        int wintitle_offsetx = 22;
//        int wintitle_offsety = -49;
//
//        // Parent window
//        if (parented) {
//
//            parent = CreateWindowEx(0, L"AlmondParent", L"Almond Docking",
//                WS_OVERLAPPEDWINDOW | WS_VISIBLE,
//                CW_USEDEFAULT, CW_USEDEFAULT, (cli::window_width + wintitle_offsetx), (cli::window_height + wintitle_offsety),
//                nullptr, nullptr, hInst, this);
//            if (!parent) return false;
//
//            DragAcceptFiles(parent, TRUE);
//            get_title_bar_thickness(parent);
//        }
//        else {
//            parent = nullptr;
//        }
//
//        std::cout << "wintitle_offsetx: "
//            << wintitle_offsetx << ", wintitle_offsety: " << wintitle_offsety << '\n';
//
//        //// ===== Dummy OpenGL context for shared initialization =====
//        HWND dummy = CreateWindowEx(WS_EX_TOOLWINDOW, L"AlmondChild", L"Dummy",
//            WS_POPUP, 0, 0, 1, 1,
//            nullptr, nullptr, hInst, nullptr);
//        if (!dummy) return false;
//        HDC dummyDC = GetDC(dummy);
//        SetupPixelFormat(dummyDC);
//        sharedContext = wglCreateContext(dummyDC); // what dummy actually does, create a shared context
//        if (!sharedContext) {
//            ReleaseDC(dummy, dummyDC);
//            DestroyWindow(dummy);
//            throw std::runtime_error("Failed to create shared OpenGL context");
//        }
//        wglMakeCurrent(dummyDC, sharedContext);
//        static bool gladInitialized = false;
//        if (!gladInitialized) {
//            gladInitialized = gladLoadGL();
//        }
//        wglMakeCurrent(nullptr, nullptr);
//        ReleaseDC(dummy, dummyDC);
//        DestroyWindow(dummy);
//
//        auto initialize_backend = [&](ContextType type, int count, bool(*backendInitialize)(std::shared_ptr<Context>, HWND, int, int)) {
//            if (count <= 0) return;
//            HWND hwnd;
//            for (int i = 0; i < count; ++i) {
//                hwnd = CreateWindowEx(0, L"AlmondChild", L"Decoy Window - This stops SDL climbing to parent position",
//                    WS_CHILD | WS_VISIBLE,
//                    0, 0, (cli::window_width + wintitle_offsetx), (cli::window_height + wintitle_offsety),
//                    parent ? parent : nullptr,
//                    nullptr, hInst, nullptr);
//                if (!hwnd) continue;
//                if (i == 0)
//                {
//                    if (type == ContextType::OpenGL)
//                    {
//                        HDC hdc = GetDC(hwnd);
//                        HGLRC ctx = CreateSharedGLContext(hdc);
//                        windows.emplace_back(std::make_unique<WindowData>(
//                            hwnd, hdc, ctx, true, ContextType::OpenGL));
//                    }
//                }
//                else
//                {
//                    windows.emplace_back(std::make_unique<WindowData>(
//                        hwnd, nullptr, nullptr, false, ContextType::OpenGL));
//
//                }
//            }
//
//            BackendState& state = g_backends[type];
//
//            //// If master is null, create a fresh Context
//            //if (!state.master)
//            //    state.master = std::make_shared<Context>();
//
//            //// Create duplicates
//            //for (int i = 1; i < count; ++i) {
//            //    auto dup = std::make_shared<Context>(*state.master);
//            //    state.duplicates.push_back(dup);
//            //}
//
//            // Example: offset each window by 50 pixels
//            int xOffset = (cli::window_width + wintitle_offsetx);
//            int yOffset = (cli::window_height + wintitle_offsety);
//
//            auto add_ctx = [&](auto& ctx) -> bool {
//                if (!ctx) return false;
//                bool ok = backendInitialize(ctx, parent, xOffset, yOffset);
//                if (ok) {
//                    ctx->hwnd = hwnd;
//                    ctx->width = xOffset;
//                    ctx->height = yOffset;
//                    ctx->type = type;
//
//#ifdef ALMOND_USING_RAYLIB
//                    if (type == ContextType::RayLib)
//                    {
//                        almondnamespace::raylibcontext::raylib_initialize(ctx, parent, xOffset, yOffset);
//                    }
//#endif
//
//#ifdef ALMOND_USING_SDL
//                    if (type == ContextType::SDL)
//                    {
//                        almondnamespace::sdlcontext::sdl_initialize(ctx, parent, xOffset, yOffset);
//                    }
//#endif
//
//#ifdef ALMOND_USING_SFML
//                    if (type == ContextType::SFML)
//                    {
//                        almondnamespace::sfmlcontext::sfml_initialize(ctx, parent, xOffset, yOffset);
//                    }
//#endif
//
//#ifdef ALMOND_USING_SOFTWARE_RENDERER
//                    if (type == ContextType::Software)
//                    {
//                        almondnamespace::anativecontext::softrenderer_initialize(ctx, parent, xOffset, yOffset);
//                    }
//#endif
//#ifdef ALMOND_USING_OPENGL
//                    if (type == ContextType::OpenGL)
//                    {
//                        almondnamespace::openglcontext::opengl_initialize(ctx, parent, xOffset, yOffset);
//                    }
//#endif
//
//                    AddWindow(ctx->hwnd, parent, nullptr, nullptr, false, [ctx](int w, int h) {
//                        ctx->width = w;
//                        ctx->height = h;
//                        // SetWindowPos(ctx->hwnd, nullptr, xOffset, yOffset, w, h,
//                        //     SWP_NOZORDER | SWP_SHOWWINDOW);
//                        //if (ctx->onResize) ctx->onResize(w, h);
//
//                        }, type);
//                }
//                return ok;
//                };
//
//            //add_ctx(state.master);
//            //RECT rc;
//            //GetClientRect(state.master->hwnd, &rc);
//            //if (state.master->onResize)
//            //    state.master->onResize(rc.right - rc.left, rc.bottom - rc.top);
//
//            //for (auto& dup : state.duplicates) {
//            //    add_ctx(dup);
//            //    RECT rcDup;
//            //    GetClientRect(dup->hwnd, &rcDup);
//            //    if (dup->onResize)
//            //        dup->onResize(rcDup.right - rcDup.left, rcDup.bottom - rcDup.top);
//            //}
//
//
//            // Initialize master and duplicates
//            add_ctx(state.master);
//            for (auto& dup : state.duplicates)
//                add_ctx(dup);
//            };
//
//#ifdef ALMOND_USING_RAYLIB
//        initialize_backend(ContextType::RayLib, RayLibWinCount,
//            [](std::shared_ptr<Context> ctx, HWND parent, int w, int h) {
//                return true;
//            });
//#endif
//
//#ifdef ALMOND_USING_SDL
//        initialize_backend(ContextType::SDL, SDLWinCount,
//            [](std::shared_ptr<Context> ctx, HWND parent, int w, int h) {
//                return true;
//            });
//#endif
//
//#ifdef ALMOND_USING_SFML
//        initialize_backend(ContextType::SFML, SFMLWinCount,
//            [](std::shared_ptr<Context> ctx, HWND parent, int w, int h) {
//                return true;
//            });
//#endif
//#ifdef ALMOND_USING_OPENGL
//        initialize_backend(ContextType::OpenGL, OpenGLWinCount,
//            [](std::shared_ptr<Context> ctx, HWND parent, int w, int h) {
//                return true;
//            });
//#endif
//
//#ifdef ALMOND_USING_SOFTWARE_RENDERER
//        initialize_backend(ContextType::Software, SoftwareWinCount,
//            [](std::shared_ptr<Context> ctx, HWND parent, int w, int h) {
//                return true;
//            });
//#endif
//        // Arrange windows in grid
//        //ArrangeDockedWindowsGrid();
//
//        return !g_backends.empty();
//    }
//
//    //void almondnamespace::core::MultiContextManager::AddWindow(HWND hwnd, HDC hdc, HGLRC glContext,
//    //    bool usesSharedContext, ResizeCallback onResize) {
//    //    windows.emplace_back(hwnd, hdc, glContext, usesSharedContext);
//    //    windows.back().onResize = std::move(onResize);
//    //    if (!hdc) MakeDockable(hwnd, parent);
//    //}
//
//    // -----------------------------------------------------------------
//    // Forward-declare docking helper
//    // -----------------------------------------------------------------
//    void MakeDockable(HWND hwnd, HWND parent);
//
//    // ======================================================
//    // MultiContextManager : Implementations
//    // ======================================================
//    void almondnamespace::core::MultiContextManager::AddWindow(
//        HWND hwnd,
//        HWND parent,
//        HDC hdc,
//        HGLRC glContext,
//        bool usesSharedContext,
//        ResizeCallback onResize,
//        ContextType type) // <-- explicit parent
//    {
//        if (!hwnd) return;
//
//        if (!hdc) hdc = GetDC(hwnd);
//
//        // Only create GL context if this window is OpenGL
//        if (type == ContextType::OpenGL) {
//#ifdef ALMOND_USING_OPENGL
//            if (!glContext) {
//                SetupPixelFormat(hdc);
//                glContext = CreateSharedGLContext(hdc);
//
//                static bool gladInitialized = false;
//                if (!gladInitialized) {
//                    wglMakeCurrent(hdc, glContext);
//                    gladInitialized = gladLoadGL();
//                    wglMakeCurrent(nullptr, nullptr);
//                }
//            }
//#endif
//        }
//
//        // Make window dockable if a parent is provided
//        if (parent) MakeDockable(hwnd, parent);
//
//        // Prepare WindowData (unique_ptr)
//        auto winPtr = std::make_unique<WindowData>(hwnd, hdc, glContext, usesSharedContext, type);
//        winPtr->onResize = std::move(onResize);
//
//        // Keep raw pointer for thread use
//        WindowData* rawWinPtr = winPtr.get();
//
//        // Insert into vector safely
//        {
//            std::scoped_lock lock(windowsMutex);
//            windows.emplace_back(std::move(winPtr));
//        }
//
//        // Launch render thread if needed
//        if (!gThreads.contains(hwnd) && rawWinPtr) {
//            gThreads[hwnd] = std::thread([this, rawWinPtr]() {
//                RenderLoop(*rawWinPtr);
//                });
//        }
//
//        // Re-arrange docked windows
//        ArrangeDockedWindowsGrid();
//    }
//
//
//    void almondnamespace::core::MultiContextManager::StartRenderThreads() {
//        std::scoped_lock lock(windowsMutex); // lock while grabbing pointers
//        for (const auto& w : windows) {
//            if (!gThreads.contains(w->hwnd)) {
//#if ALMOND_SINGLE_PARENT
//                // Only spawn threads for windows that exist and are valid
//                gThreads[w->hwnd] = std::thread([this, hwnd = w->hwnd]() {
//                    auto it = std::find_if(windows.begin(), windows.end(),
//                        [hwnd](const std::unique_ptr<WindowData>& w) { return w->hwnd == hwnd; });
//                    if (it != windows.end()) RenderLoop(**it);
//                    });
//#else
//                // Fully independent: spawn thread for every window
//                gThreads[w->hwnd] = std::thread([this, hwnd = w->hwnd]() {
//                    auto it = std::find_if(windows.begin(), windows.end(),
//                        [hwnd](const std::unique_ptr<WindowData>& w) { return w->hwnd == hwnd; });
//                    if (it != windows.end()) RenderLoop(**it);
//                    });
//#endif
//            }
//        }
//    }
//
//    void almondnamespace::core::MultiContextManager::RemoveWindow(HWND hwnd)
//    {
//        std::unique_ptr<WindowData> removedWindow;
//
//        {
//            std::scoped_lock lock(windowsMutex);
//
//            // Find the window
//            auto it = std::find_if(
//                windows.begin(), windows.end(),
//                [hwnd](const std::unique_ptr<WindowData>& w) {
//                    return w && w->hwnd == hwnd;
//                });
//
//            if (it == windows.end())
//                return;
//
//            // Mark the window as no longer running
//            (*it)->running = false;
//
//            // Take ownership of the unique_ptr out of the vector
//            removedWindow = std::move(*it);
//            windows.erase(it);
//        }
//
//        // Ensure the render thread is stopped before destroying the context
//        if (gThreads.contains(hwnd)) {
//            if (gThreads[hwnd].joinable()) {
//                gThreads[hwnd].join();
//            }
//            gThreads.erase(hwnd);
//        }
//
//        // Clean up OpenGL / DC (safe now, since thread is joined)
//        if (removedWindow) {
//            if (removedWindow->glContext) {
//                wglMakeCurrent(nullptr, nullptr);
//                wglDeleteContext(removedWindow->glContext);
//            }
//            if (removedWindow->hdc && removedWindow->hwnd) {
//                ReleaseDC(removedWindow->hwnd, removedWindow->hdc);
//            }
//            // unique_ptr destructor cleans up WindowData
//        }
//    }
//
//    // ---------------- Docking Layout ----------------
//
//    // Original version (simple equal grid):
//    // void almondnamespace::core::MultiContextManager::ArrangeDockedWindowsGrid() {
//    //     if (!parent || windows.empty()) return;
//    //     RECT rc; GetClientRect(parent, &rc);
//    //     int cols = 1, rows = 1;
//    //     while (cols * rows < (int)windows.size())
//    //         (cols <= rows ? ++cols : ++rows);
//    //     int cw = rc.right / cols, ch = rc.bottom / rows;
//    //     for (size_t i = 0; i < windows.size(); ++i) {
//    //         int c = int(i) % cols, r = int(i) / cols;
//    //         SetWindowPos(windows[i]->hwnd, nullptr, c * cw, r * ch, cw, ch,
//    //             SWP_NOZORDER | SWP_NOACTIVATE);
//    //         if (windows[i]->onResize) windows[i]->onResize(cw, ch);
//    //     }
//    // }
//
//    // Alternate version (resize parent to fit grid):
//    // void almondnamespace::core::MultiContextManager::ArrangeDockedWindowsGrid() {
//    //     if (!parent || windows.empty()) return;
//    //
//    //     constexpr int minCellWidth = 400;
//    //     constexpr int minCellHeight = 300;
//    //
//    //     int cols = 1, rows = 1;
//    //     while (cols * rows < (int)windows.size())
//    //         (cols <= rows ? ++cols : ++rows);
//    //
//    //     int totalWidth = cols * minCellWidth;
//    //     int totalHeight = rows * minCellHeight;
//    //
//    //     SetWindowPos(parent, nullptr, 0, 0, totalWidth, totalHeight,
//    //         SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
//    //
//    //     RECT rc; GetClientRect(parent, &rc);
//    //     int cw = rc.right / cols;
//    //     int ch = rc.bottom / rows;
//    //
//    //     for (size_t i = 0; i < windows.size(); ++i) {
//    //         int c = int(i) % cols;
//    //         int r = int(i) / cols;
//    //         SetWindowPos(windows[i]->hwnd, nullptr, c * cw, r * ch, cw, ch,
//    //             SWP_NOZORDER | SWP_NOACTIVATE);
//    //         if (windows[i]->onResize) windows[i]->onResize(cw, ch);
//    //     }
//    // }
//
//    // Final active version (your current one):
//    void almondnamespace::core::MultiContextManager::ArrangeDockedWindowsGrid() {
//        if (!parent || windows.empty()) return;
//
//        // Compute grid size
//        int total = (int)windows.size();
//        int cols = 1, rows = 1;
//        while (cols * rows < total) (cols <= rows ? ++cols : ++rows);
//
//        constexpr int minCellWidth = 400;
//        constexpr int minCellHeight = 300;
//
//        // Desired client area for parent
//        int clientWidth = cols * minCellWidth;
//        int clientHeight = rows * minCellHeight;
//
//        // Adjust parent outer size
//        RECT rc = { 0, 0, clientWidth, clientHeight };
//        DWORD style = GetWindowLong(parent, GWL_STYLE);
//        AdjustWindowRect(&rc, style, FALSE);
//        int parentWidth = rc.right - rc.left;
//        int parentHeight = rc.bottom - rc.top;
//
//        // Resize parent window
//        SetWindowPos(parent, nullptr, 0, 0, parentWidth, parentHeight,
//            SWP_NOZORDER | SWP_SHOWWINDOW);
//
//        // Get client area again
//        RECT rcClient;
//        GetClientRect(parent, &rcClient);
//        int cw = rcClient.right / cols;
//        int ch = rcClient.bottom / rows;
//
//        // Arrange children
//        for (size_t i = 0; i < windows.size(); ++i) {
//            int c = (int)i % cols;
//            int r = (int)i / cols;
//            WindowData& win = *windows[i];
//
//            SetWindowPos(win.hwnd, nullptr, c * cw, r * ch, cw, ch,
//                SWP_NOZORDER | SWP_SHOWWINDOW);
//
//            if (win.onResize) win.onResize(cw, ch);
//        }
//    }
//
//    void almondnamespace::core::MultiContextManager::StopAll() {
//        running = false;
//
//        {
//            std::scoped_lock lock(windowsMutex);
//            for (auto& w : windows) {
//                if (w) w->running = false;
//            }
//        }
//
//        for (auto& [hwnd, th] : gThreads) {
//            if (th.joinable()) th.join();
//        }
//        gThreads.clear();
//    }
//
//    void almondnamespace::core::MultiContextManager::RenderLoop(WindowData& win) {
//        switch (win.type) {
//        case ContextType::OpenGL:
//#ifdef ALMOND_USING_OPENGL
//            if (!wglMakeCurrent(win.hdc, win.glContext)) return;
//            while (running && win.running) {
//                glClearColor(win.clicked ? 1.f : 0.f, 0.f, win.clicked ? 0.f : 1.f, 1.f);
//                glClear(GL_COLOR_BUFFER_BIT);
//
//                win.DrainCommands();
//
//                SwapBuffers(win.hdc);
//                std::this_thread::sleep_for(std::chrono::milliseconds(16));
//            }
//            wglMakeCurrent(nullptr, nullptr);
//#endif
//            break;
//
//        case ContextType::Software:
//            while (running && win.running) {
//                win.DrainCommands();
//                std::this_thread::sleep_for(std::chrono::milliseconds(16));
//            }
//            break;
//        }
//    }
//
//    void almondnamespace::core::MultiContextManager::HandleDropFiles(HWND hwnd, HDROP hDrop) {
//        UINT count = DragQueryFile(hDrop, 0xFFFFFFFF, nullptr, 0);
//        for (UINT i = 0; i < count; ++i) {
//            wchar_t path[MAX_PATH]{};
//            DragQueryFile(hDrop, i, path, MAX_PATH);
//            std::wcout << L"[Drop] " << path << L"\n";
//        }
//    }
//
//    LRESULT CALLBACK almondnamespace::core::MultiContextManager::ParentProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//        if (msg == WM_NCCREATE) {
//            auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
//            SetWindowLongPtr(hwnd, GWLP_USERDATA,
//                reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
//            return TRUE;
//        }
//        auto* mgr = reinterpret_cast<MultiContextManager*>(
//            GetWindowLongPtr(hwnd, GWLP_USERDATA));
//        if (!mgr) return DefWindowProc(hwnd, msg, wParam, lParam);
//
//        switch (msg) {
//            //case WM_SIZE: mgr->ArrangeDockedWindowsGrid(); return 0;
//        case WM_PAINT: {
//            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
//            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
//            EndPaint(hwnd, &ps); return 0;
//        }
//        case WM_DROPFILES:
//            mgr->HandleDropFiles(hwnd, reinterpret_cast<HDROP>(wParam));
//            DragFinish(reinterpret_cast<HDROP>(wParam));
//            return 0;
//        case WM_CLOSE: DestroyWindow(hwnd); return 0;
//        case WM_DESTROY:
//            if (hwnd == mgr->GetParentWindow()) {
//                PostQuitMessage(0);
//            }
//            return 0;
//
//        default: return DefWindowProc(hwnd, msg, wParam, lParam);
//        }
//    }
//
//    LRESULT CALLBACK almondnamespace::core::MultiContextManager::ChildProc(HWND hwnd, UINT msg,
//        WPARAM wParam, LPARAM lParam) {
//        static DragState& drag = gDragState;
//        if (msg == WM_NCCREATE) {
//            auto* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
//            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
//            return TRUE;
//        }
//
//        switch (msg) {
//        case WM_LBUTTONDOWN: {
//            SetCapture(hwnd);
//            drag.dragging = true;
//            drag.draggedWindow = hwnd;
//            drag.originalParent = GetParent(hwnd);
//            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//            ClientToScreen(hwnd, &pt);
//            drag.lastMousePos = pt;
//            return 0;
//        }
//        case WM_MOUSEMOVE: {
//            if (!drag.dragging || drag.draggedWindow != hwnd)
//                return DefWindowProc(hwnd, msg, wParam, lParam);
//
//            POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
//            ClientToScreen(hwnd, &pt);
//            int dx = pt.x - drag.lastMousePos.x;
//            int dy = pt.y - drag.lastMousePos.y;
//            drag.lastMousePos = pt;
//
//            RECT wndRect; GetWindowRect(hwnd, &wndRect);
//            int newX = wndRect.left + dx;
//            int newY = wndRect.top + dy;
//
//            if (drag.originalParent) {
//                RECT prc; GetClientRect(drag.originalParent, &prc);
//                POINT tl{ 0, 0 }; ClientToScreen(drag.originalParent, &tl);
//                OffsetRect(&prc, tl.x, tl.y);
//
//                bool inside =
//                    newX >= prc.left && newY >= prc.top &&
//                    (newX + (wndRect.right - wndRect.left)) <= prc.right &&
//                    (newY + (wndRect.bottom - wndRect.top)) <= prc.bottom;
//
//                if (inside) {
//                    if (GetParent(hwnd) != drag.originalParent) {
//                        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
//                        style &= ~(WS_POPUP | WS_OVERLAPPEDWINDOW);
//                        style |= WS_CHILD;
//                        SetWindowLongPtr(hwnd, GWL_STYLE, style);
//                        SetParent(hwnd, drag.originalParent);
//
//                        POINT cp{ newX, newY };
//                        ScreenToClient(drag.originalParent, &cp);
//                        SetWindowPos(hwnd, nullptr, cp.x, cp.y,
//                            wndRect.right - wndRect.left,
//                            wndRect.bottom - wndRect.top,
//                            SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
//                    }
//                    else {
//                        POINT cp{ newX, newY };
//                        ScreenToClient(drag.originalParent, &cp);
//                        SetWindowPos(hwnd, nullptr, cp.x, cp.y,
//                            0, 0, SWP_NOZORDER | SWP_NOSIZE |
//                            SWP_NOACTIVATE);
//                    }
//                }
//                else {
//                    if (GetParent(hwnd) == drag.originalParent) {
//                        LONG_PTR style = GetWindowLongPtr(hwnd, GWL_STYLE);
//                        style &= ~WS_CHILD;
//                        style |= WS_OVERLAPPEDWINDOW | WS_VISIBLE;
//                        SetWindowLongPtr(hwnd, GWL_STYLE, style);
//                        SetParent(hwnd, nullptr);
//                        SetWindowPos(hwnd, nullptr, newX, newY,
//                            wndRect.right - wndRect.left,
//                            wndRect.bottom - wndRect.top,
//                            SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
//                    }
//                    else {
//                        SetWindowPos(hwnd, nullptr, newX, newY,
//                            0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
//                    }
//                }
//            }
//            else {
//                SetWindowPos(hwnd, nullptr, newX, newY,
//                    0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
//            }
//            return 0;
//        }
//        case WM_LBUTTONUP: {
//            if (drag.dragging && drag.draggedWindow == hwnd) {
//                ReleaseCapture();
//                drag.dragging = false;
//                drag.draggedWindow = nullptr;
//                drag.originalParent = nullptr;
//            }
//            return 0;
//        }
//        case WM_DROPFILES: {
//            if (HWND p = GetParent(hwnd))
//                SendMessage(p, WM_DROPFILES, wParam, lParam);
//            DragFinish(reinterpret_cast<HDROP>(wParam));
//            return 0;
//        }
//        case WM_PAINT: {
//            PAINTSTRUCT ps; HDC hdc = BeginPaint(hwnd, &ps);
//            FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
//            EndPaint(hwnd, &ps);
//            return 0;
//        }
//        default:
//            return DefWindowProc(hwnd, msg, wParam, lParam);
//        }
//    }
//
//    // -----------------------------------------------------------------
//    // Helper definitions that must be visible to the linker
//    // -----------------------------------------------------------------
//
//    // -----------------------------------------------------------------
//    // Dockable child handling
//    // -----------------------------------------------------------------
//    struct SubCtx { HWND originalParent; };
//
//    LRESULT CALLBACK DockableProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp,
//        UINT_PTR, DWORD_PTR dw)
//    {
//#if ALMOND_SINGLE_PARENT
//        auto* ctx = reinterpret_cast<SubCtx*>(dw);
//#endif
//
//        switch (msg) {
//        case WM_CLOSE:
//#if ALMOND_SINGLE_PARENT
//            if (ctx && ctx->originalParent) {
//                if (hwnd == ctx->originalParent) {
//                    PostQuitMessage(0); // main parent closed -> shutdown engine
//                }
//                else {
//                    DestroyWindow(hwnd); // child closes itself
//                }
//            }
//            else {
//                DestroyWindow(hwnd); // no parent -> just close itself
//            }
//#else
//            DestroyWindow(hwnd); // always independent
//#endif
//            return 0;
//
//        case WM_LBUTTONDOWN:
//        case WM_MOUSEMOVE:
//        case WM_LBUTTONUP:
//            return almondnamespace::core::MultiContextManager::ChildProc(hwnd, msg, wp, lp);
//        }
//        return DefSubclassProc(hwnd, msg, wp, lp);
//    }
//
//    void MakeDockable(HWND hwnd, HWND parent) {
//#if ALMOND_SINGLE_PARENT
//        auto* ctx = new SubCtx{ parent };
//        SetWindowSubclass(hwnd, DockableProc, 1, reinterpret_cast<DWORD_PTR>(ctx));
//#endif
//    }
