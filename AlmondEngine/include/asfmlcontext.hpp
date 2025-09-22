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
 // asfmlcontext.hpp
#pragma once

#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SFML)

#include "aplatform.hpp"
#include "asfmlstate.hpp"

#include "acontext.hpp"
#include "acontextwindow.hpp"
#include "acommandline.hpp"

#include <iostream>
#include <string>
#include <utility>

//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp>

namespace almondnamespace::sfmlcontext
{
    using namespace almondnamespace::contextwindow;
    using namespace almondnamespace::core;
    //inline sfmlcontext::state::SFML3State s_state;
/*    inline void initialize()
    {

        HWND hwnd = almondnamespace::contextwindow::WindowContext::getWindowHandle();
        if (!hwnd)
            throw std::runtime_error("No valid HWND in WindowContext!");

        // Create the SFML RenderWindow that attaches to the native HWND.
        // Don't create a NEW window — wrap the existing one.
        s_state.window.sfml_window = new sf::RenderWindow(static_cast<sf::WindowHandle>(hwnd));

        s_state.window.sfml_window->setVerticalSyncEnabled(true);
    }
*/

    struct SFMLState {
        std::unique_ptr<sf::RenderWindow> window;
        HWND parent = nullptr;
        HWND hwnd = nullptr;
        HDC hdc = nullptr;
        HGLRC glContext = nullptr; // External OpenGL context (e.g., from Raylib)
        unsigned int width = 400;
        unsigned int height = 300;
        bool running = false;
        std::function<void(int, int)> onResize;
    };
    inline SFMLState sfmlcontext;

    inline bool sfml_initialize(std::shared_ptr<core::Context> ctx, HWND parentWnd = nullptr, unsigned int w = 400, unsigned int h = 300, std::function<void(int, int)> onResize = nullptr)
    {
//        //constexpr unsigned int width = 1280;
//        //constexpr unsigned int height = 720;
//        //const char* title = "Almond Engine SFML Window";
//        //sf::Vector2u screenSize = { width, height };
//        //sf::VideoMode mode(screenSize, 32);
//
//        almondnamespace::contextwindow::WindowData* winCtx = contextwindow::WindowData::get_global_instance();
//
//        //if (s_sfmlstate.window.sfml_window)
//        //{
//        //    std::cerr << "[SFML] Warning: Window already initialized.\n";
//        //    return;
//        //}
//        //
//        //s_sfmlstate.window.sfml_window = new sf::RenderWindow(winCtx->getChildHandle());// mode, std::string{ title });
//
//
//
//
////            auto* winCtx = almondnamespace::contextwindow::WindowContext::get_global_instance();
////HWND hwndChild = winCtx->getParentHandle();
////if (!hwndChild) throw std::runtime_error("No valid child HWND!");
////
////
////RECT rc; GetClientRect(hwndChild, &rc);
////std::cout << "Child size: " << (rc.right - rc.left) << "x" << (rc.bottom - rc.top) << std::endl;
////
////LONG_PTR style = GetWindowLongPtr(hwndChild, GWL_STYLE);
////std::cout << "Child style: 0x" << std::hex << style << std::endl;
////
////char className[256] = {};
////GetClassNameA(hwndChild, className, 255);
////std::cout << "Child class: " << className << std::endl;
////
////s_sfmlstate.window.sfml_window = new sf::RenderWindow(hwndChild); 
////winCtx->setChildHandle(s_sfmlstate.window.sfml_window->getNativeHandle());
//
//
//        //HWND parentHwnd = CreateWindowEx(
//        //    0, L"MyParentClass", L"Parent Window",
//        //    WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, core::cli::window_width, core::cli::window_height,
//        //    nullptr, nullptr, hInstance, nullptr);
//
//        //ShowWindow(parentHwnd, nCmdShow);
//        //UpdateWindow(parentHwnd);
//
//		// Create the SFML RenderWindow that attaches to the native HWND.
//
//        unsigned int width = static_cast<unsigned int>(core::cli::window_width);
//        unsigned int height = static_cast<unsigned int>(core::cli::window_height);
//        //unsigned int width = 800;
//        //unsigned int height = 600;
//
//        const char* title = "Almond Engine SFML Window";
//        sf::Vector2u screenSize = { width,height };
//        sf::VideoMode mode(screenSize, 32);
//
//        sf::RenderWindow sfmlWindow(mode, title);
//
//        // Grab the SFML window HWND
//        HWND sfmlHwnd = reinterpret_cast<HWND>(sfmlWindow.getNativeHandle());
//        s_sfmlstate.window.hwndChild = sfmlHwnd; // if you have a member for HWND
//
//        HWND parentHwnd = winCtx->getParentHandle();
//
//        // "Dock" the SFML window as a child of the parent
//        SetParent(sfmlHwnd, parentHwnd);
//
//        // Change window style to child
//        LONG_PTR style = GetWindowLongPtr(sfmlHwnd, GWL_STYLE);
//        style &= ~WS_POPUP;
//        style |= WS_CHILD | WS_VISIBLE;
//        SetWindowLongPtr(sfmlHwnd, GWL_STYLE, style);
//
//        // Position and size inside parent
//        SetWindowPos(sfmlHwnd, nullptr, 0, 0, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
//
//        // Draw something
//        sf::CircleShape shape(100.f);
//        shape.setFillColor(sf::Color::Green);
//
//
//        MSG msg;
//        bool running = true;
//        while (running)
//        {
//            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
//            {
//                if (msg.message == WM_QUIT)
//                    running = false;
//                TranslateMessage(&msg);
//                DispatchMessage(&msg);
//            }
//
//            // SFML rendering
//            if (sfmlWindow.isOpen())
//            {
//                sfmlWindow.clear(sf::Color::Red);
//                sfmlWindow.draw(shape);
//                sfmlWindow.display();
//            }
//        }
//
//#if defined(_WIN32)
//      //  if (winCtx)
//      //  {
//            //HWND hwnd = s_state.window.sfml_window->getNativeHandle();
//            //if (!hwnd)
//            //    throw std::runtime_error("[SFML] Failed to grab native window handle.");
//            //ctx->setParentHandle(hwnd);
//          //  HWND hwnd = static_cast<HWND>(s_sfmlstate.window.sfml_window->getNativeHandle());
//          //  winCtx->setChildHandle(hwnd); // Set this if you want to dock SFML window into a parent window
//
//      //  }
//#endif
//
//        //s_sfmlstate.window.sfml_window->setVerticalSyncEnabled(true);
//        return true;

        ctx->onResize = std::move(onResize);
        sfmlcontext.width = w;
        sfmlcontext.height = h;
        sfmlcontext.parent = parentWnd;

        // Force SFML to use core OpenGL (no software fallback)
        sf::ContextSettings settings;
        settings.majorVersion = 3;
        settings.minorVersion = 3;
        settings.attributeFlags = sf::ContextSettings::Core;


        sf::Vector2u screenSize = { sfmlcontext.width, sfmlcontext.height };
        sf::VideoMode mode(screenSize, 32);
        sfmlcontext.window = std::make_unique<sf::RenderWindow>(mode, "SFML Window");

        if (!sfmlcontext.window || !sfmlcontext.window->isOpen()) {
            std::cerr << "[SFML] Failed to create SFML window\n";
            return false;
        }

        sfmlcontext.hwnd = sfmlcontext.window->getNativeHandle();
        sfmlcontext.hdc = GetDC(sfmlcontext.hwnd);

        // SFML MUST have created its context already at this point
        sfmlcontext.glContext = wglGetCurrentContext();
        if (!sfmlcontext.glContext) {
            std::cerr << "[SFML] Failed to get OpenGL context\n";
            return false;
        }

        // Set parent if requested (for embedding)
        if (sfmlcontext.parent) {
            SetParent(sfmlcontext.hwnd, sfmlcontext.parent);

            LONG_PTR style = GetWindowLongPtr(sfmlcontext.hwnd, GWL_STYLE);
            style &= ~WS_OVERLAPPEDWINDOW;
            style |= WS_CHILD | WS_VISIBLE;
            SetWindowLongPtr(sfmlcontext.hwnd, GWL_STYLE, style);

            SetWindowPos(sfmlcontext.hwnd, nullptr, 0, 0, sfmlcontext.width, sfmlcontext.height,
                SWP_NOZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
        }
        std::cout << "[SFML] HWND: " << sfmlcontext.hwnd << "\n";
        std::cout << "[SFML] HDC: " << sfmlcontext.hdc << "\n";
        std::cout << "[SFML] HGLRC: " << sfmlcontext.glContext << "\n";

        sfmlcontext.running = true;
        return true;
    }

    inline void sfml_begin_frame()
    {
        if (!s_sfmlstate.window.sfml_window)
            throw std::runtime_error("[SFML] Window not initialized.");

        if (s_sfmlstate.window.sfml_window->isOpen())
            s_sfmlstate.window.sfml_window->clear(sf::Color::Black);
    }

    inline void sfml_end_frame()
    {
        if (!s_sfmlstate.window.sfml_window)
            throw std::runtime_error("[SFML] Window not initialized.");

        if (s_sfmlstate.window.sfml_window->isOpen())
            s_sfmlstate.window.sfml_window->display();
    }

    inline bool sfml_should_close()
    {
        if (!s_sfmlstate.window.sfml_window)
            throw std::runtime_error("[SFML] Window not initialized.");

        return !s_sfmlstate.window.sfml_window->isOpen();
    }

    inline void sfml_clear() { sfml_begin_frame(); }
    inline void sfml_present() { sfml_end_frame(); }

    inline bool sfml_process(core::Context& ctx, core::CommandQueue& queue)
    {
        //if (!s_sfmlstate.window.sfml_window)
        //    throw std::runtime_error("[SFML] Window not initialized.");

        //while (const std::optional<sf::Event> event = s_sfmlstate.window.sfml_window->pollEvent())
        //{
        //    if (event->is<sf::Event::Closed>())
        //    {
        //        s_sfmlstate.window.sfml_window->close();
        //        return false;
        //    }
        //    sfml_clear();
        //    sfml_present();
        //}
        //return true;


        if (!sfmlcontext.running || !sfmlcontext.window || !sfmlcontext.window->isOpen()) return 1;

        // ⚠️ This makes the GL context active for SFML’s thread
        if (!wglMakeCurrent(sfmlcontext.hdc, sfmlcontext.glContext)) {
            std::cerr << "[SFMLRender] Failed to activate GL context\n";
            sfmlcontext.running = false;
            return false;
        }

        bool isActiveSFMLcontextwindow = sfmlcontext.window->setActive(); // Activates SFML's render context wrapper

        if (auto event = sfmlcontext.window->pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                sfmlcontext.window->close();
                sfmlcontext.running = false;
            }
        }
        static auto* bgTimer = almondnamespace::time::getTimer("menu", "bg_color");
        if (!bgTimer)
            bgTimer = &almondnamespace::time::createNamedTimer("menu", "bg_color");

        double t = almondnamespace::time::elapsed(*bgTimer); // seconds

        // smooth rainbow effect
        unsigned char r = static_cast<unsigned char>((0.5 + 0.5 * std::sin(t * 1.0)) * 255);
        unsigned char g = static_cast<unsigned char>((0.5 + 0.5 * std::sin(t * 0.7 + 2.0)) * 255);
        unsigned char b = static_cast<unsigned char>((0.5 + 0.5 * std::sin(t * 1.3 + 4.0)) * 255);

        sfmlcontext.window->clear(sf::Color(r, g, b));

        //sfmlcontext.window->clear(sf::Color::Green);
        sfmlcontext.window->display();
        return true;
    }

    inline std::pair<int, int> get_window_size_wh() noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return { 0, 0 };

        auto size = s_sfmlstate.window.sfml_window->getSize();
        return { static_cast<int>(size.x), static_cast<int>(size.y) };
    }

    inline std::pair<int, int> get_window_position_xy() noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return { 0, 0 };

        auto pos = s_sfmlstate.window.sfml_window->getPosition();
        return { pos.x, pos.y };
    }

    inline void set_window_position(int x, int y) noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return;

        s_sfmlstate.window.sfml_window->setPosition(sf::Vector2i(x, y));
    }

    inline void set_window_size(int width, int height) noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return;

        s_sfmlstate.window.sfml_window->setSize(sf::Vector2u(static_cast<unsigned>(width), static_cast<unsigned>(height)));
    }

    inline void set_window_icon(const std::string& iconPath) noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return;

        //sf::Image icon;
        //if (icon.loadFromFile(iconPath))
        //    s_state.window.sfml_window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
        //else
        //    std::cerr << "[SFML_SetWindowIcon] Failed to load icon from: " << iconPath << '\n';
    }

    inline int sfml_get_width() noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return 0;

        return static_cast<int>(s_sfmlstate.window.sfml_window->getSize().x);
    }

    inline int sfml_get_height() noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return 0;

        return static_cast<int>(s_sfmlstate.window.sfml_window->getSize().y);
    }

    inline void sfml_set_window_title(const std::string& title) noexcept
    {
        if (!s_sfmlstate.window.sfml_window)
            return;

        s_sfmlstate.window.sfml_window->setTitle(title);
    }

    inline void sfml_cleanup(std::shared_ptr<almondnamespace::core::Context>& ctx)
    {
        //if (s_sfmlstate.window.sfml_window)
        //{
        //    if (s_sfmlstate.window.sfml_window->isOpen())
        //        s_sfmlstate.window.sfml_window->close();

        //    delete s_sfmlstate.window.sfml_window;
        //    s_sfmlstate.window.sfml_window = nullptr;
        //}

        if (sfmlcontext.running && sfmlcontext.window) {
            sfmlcontext.window->close();
            sfmlcontext.window.reset();
            sfmlcontext.running = false;
        }
    }

    inline bool SFMLIsRunning(std::shared_ptr<core::Context> ctx)
    {
        return sfmlcontext.running;
    }
}

#endif // ALMOND_USING_SFML
