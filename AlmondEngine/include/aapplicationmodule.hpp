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
#pragma once

#include <vector>
#include <cstdint>

namespace almondnamespace {
    // ─── Contract ────────────────────────────────────────────────────────────────
    struct application_module {
        void (*init)    () noexcept = nullptr;
        void (*update)  (float dt)  noexcept = nullptr;
        void (*shutdown)() noexcept = nullptr;
    };

    // ─── Internal registry (header‑only, hidden from public API) ─────────────────
    namespace detail {
        inline auto& get_modules() {
            static std::vector<application_module*> list;
            return list;
        }
    }

    // ─── Registrar helper (automatic push into the list) ─────────────────────────
    struct _module_registrar {
        explicit _module_registrar(application_module* m) noexcept {
            detail::get_modules().push_back(m);
        }
    };

} // namespace almondnamespace

// ─── Sugar: declare an application module in one shot ───────────────────────
#define ALMOND_APPLICATION_MODULE(NAME)                                      \
    static void NAME##_init    () noexcept;                                  \
    static void NAME##_update  (float) noexcept;                             \
    static void NAME##_shutdown() noexcept;                                  \
    static almondnamespace::application_module NAME##_desc {                 \
        &NAME##_init, &NAME##_update, &NAME##_shutdown                       \
    };                                                                       \
    static almondnamespace::_module_registrar NAME##_auto { &NAME##_desc };  \
    static void NAME##_init() noexcept                                       \