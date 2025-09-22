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
 // acontextstatemanager.hpp
#pragma once

#include "aplatform.hpp"      // must always come first
#include "acontextstate.hpp"  // ContextState definition

#include <memory>
#include <stdexcept>

namespace almondnamespace::state
{
    struct ContextState; // Forward declaration
}

namespace almondnamespace
{
    class ContextManager
    {
    public:
        static void setContext(std::shared_ptr<state::ContextState> ctx)
        {
            instance() = std::move(ctx);
        }

        static state::ContextState& getContext()
        {
            auto& inst = instance();
            if (!inst)
                throw std::runtime_error("No active ContextState set");
            return *inst;
        }

        static void resetContext()
        {
            instance().reset();
        }

        static bool hasContext()
        {
            return static_cast<bool>(instance());
        }

    private:
        static std::shared_ptr<state::ContextState>& instance()
        {
            static std::shared_ptr<state::ContextState> s_instance;
            return s_instance;
        }
    };
}
