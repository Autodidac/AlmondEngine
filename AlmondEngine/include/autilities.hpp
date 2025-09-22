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

#include "aplatform.hpp"

#include <iostream>
#include <utility>
#include <type_traits>

namespace almondnamespace::utilities
{
#ifdef _WIN32 // Windows-specific

#include <consoleapi3.h>
#include "aframework.hpp"

    [[nodiscard]] inline constexpr bool isConsoleApplication() noexcept
    {
        return GetConsoleWindow() != nullptr;
    }

#else // Unix-like

#include <unistd.h>
#include <stdio.h>

    [[nodiscard]] inline constexpr bool isConsoleApplication() noexcept
    {
        return isatty(fileno(stdout)) != 0;
    }

#endif

    // Macro retry-once for no-return calls, scoped safely
#define NO_RETURN_RETRY_ONCE(call)                  \
    do {                                            \
        try {                                       \
            call;                                   \
        } catch (...) {                             \
            std::cerr << "[Retry] Retrying: " #call "\n"; \
            call;                                   \
        }                                           \
    } while (0)

    // Retry wrapper for callables with return values
    template<typename Func>
    auto make_retry_once(Func&& f)
    {
        return [f = std::forward<Func>(f)](auto&&... args) -> decltype(auto)
            {
                try
                {
                    return f(std::forward<decltype(args)>(args)...);
                }
                catch (...)
                {
                    std::cerr << "[Retry] First attempt failed, retrying once...\n";
                    return f(std::forward<decltype(args)>(args)...);
                }
            };
    }
} // namespace almondnamespace::utilities

////// Usage examples: //////

        //1. Using the macro for calls that don’t return anything(void functions) :
        //#include "aalmondretry.hpp"

        //    void risky_operation()
        //{
        //    static int attempts = 0;
        //    if (++attempts == 1) {
        //        throw std::runtime_error("Simulated failure");
        //    }
        //    std::cout << "Operation succeeded on attempt " << attempts << "\n";
        //}

        //int main()
        //{
        //    NO_RETURN_RETRY_ONCE(risky_operation());

        //    // Output:
        //    // [Retry] Retrying: risky_operation();
        //    // Operation succeeded on attempt 2
        //}



        //2. Using the function wrapper for functions with return values:
        //cpp
        //    Copy
        //    Edit
        //#include "aalmondretry.hpp"

        //    int risky_calculation(int x)
        //{
        //    static int attempts = 0;
        //    if (++attempts == 1) {
        //        throw std::runtime_error("First attempt failed");
        //    }
        //    return x * 2;
        //}

        //int main()
        //{
        //    auto safe_calculation = almondnamespace::utilities::make_retry_once(risky_calculation);

        //    int result = safe_calculation(21);
        //    std::cout << "Result: " << result << "\n";

        //    // Output:
        //    // [Retry] First attempt failed, retrying once...
        //    // Result: 42
        //}