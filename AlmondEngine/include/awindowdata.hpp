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
 // awindowdata.hpp
#pragma once

#include <windows.h>
#include <memory>
#include <functional>

#include "acontexttype.hpp"  // for ContextType
#include "acommandqueue.hpp" // for CommandQueue

namespace almondnamespace::core 
{
    struct Context; // forward declaration
    class MultiContextManager; // Forward declare MultiContextManager for ResizeCallback

    struct WindowData
    {
        HWND hwnd{};
        HDC  hdc{};
        HGLRC glContext{};
        bool running = true;
        bool clicked = false;
        bool usesSharedContext = false;
        std::shared_ptr<Context> context;
        almondnamespace::core::ContextType type { almondnamespace::core::ContextType::OpenGL };

        int width = 0;
        int height = 0;

        using ResizeCallback = std::function<void(int, int)>;
        ResizeCallback onResize;

        CommandQueue commandQueue;

        WindowData() = default;
        WindowData(HWND h, HDC dc, HGLRC ctx, bool shared, almondnamespace::core::ContextType t)
            : hwnd(h), hdc(dc), glContext(ctx), usesSharedContext(shared), type(t) {
        }

        // Non-copyable, non-movable
        WindowData(const WindowData&) = delete;
        WindowData& operator=(const WindowData&) = delete;
        WindowData(WindowData&&) = delete;
        WindowData& operator=(WindowData&&) = delete;

        // Forwarding convenience
        void EnqueueCommand(std::function<void()> cmd) {
            commandQueue.enqueue(std::move(cmd));
        }

        bool DrainCommands() {
            return commandQueue.drain();
        }
    };

} // namespace almondshell::core
