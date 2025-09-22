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
 // aversion.hpp - Almond Engine version information
#pragma once

#include <iostream>
#include <cstdio>

namespace almondnamespace {

    // Version information as constexpr for compile-time evaluation
    constexpr int major = 0;
    constexpr int minor = 42;
    constexpr int revision = 1;

    static char version_string[32] = "";
    static char name_string[16] = "Almond Engine";
    
    // Use inline to ensure that each definition is treated uniquely in different translation units
    inline int GetMajor() { return major; }
    inline int GetMinor() { return minor; }
    inline int GetRevision() { return revision; }

    inline const char* GetEngineName()
    {
        return name_string;
    }

    inline const char* GetEngineVersion()
    {
        std::snprintf(version_string, sizeof(version_string), "%d.%d.%d", major, minor, revision);
        return version_string;
    }

} // namespace almond
