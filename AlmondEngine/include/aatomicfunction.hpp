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
// aatomicfunction.hpp
#pragma once

#include <functional>
#include <atomic>
#include <utility>
#include <stdexcept>
#include <memory>

namespace almondnamespace::core {

    // Thread-safe, reusable, header-only function wrapper
    template <typename Signature>
    struct AlmondAtomicFunction {
        using Func = std::function<Signature>;
        std::atomic<std::shared_ptr<Func>> ptr{ nullptr };

        AlmondAtomicFunction() noexcept = default;

        // Construct/assign from any callable (lambda, free function, std::function)
        template<typename F>
        AlmondAtomicFunction(F&& f) noexcept {
            store(std::forward<F>(f));
        }

        template<typename F>
        AlmondAtomicFunction& operator=(F&& f) noexcept {
            store(std::forward<F>(f));
            return *this;
        }

        // Store a new callable
        template<typename F>
        void store(F&& f) noexcept {
            auto newf = std::make_shared<Func>(std::forward<F>(f));
            ptr.store(newf, std::memory_order_release);
        }

        // Check if a function is stored
        bool empty() const noexcept {
            return !ptr.load(std::memory_order_acquire);
        }

        // Call operator
        template<typename... Args>
        auto operator()(Args&&... args) const -> decltype(std::declval<Func>()(std::forward<Args>(args)...)) {
            auto f = ptr.load(std::memory_order_acquire);
            if (!f) throw std::bad_function_call();
            return (*f)(std::forward<Args>(args)...);
        }

        // Implicit conversion to bool
        explicit operator bool() const noexcept { return !empty(); }
    };

} // namespace almondnamespace::core
