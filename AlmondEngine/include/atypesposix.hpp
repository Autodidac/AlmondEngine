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

#include "atypes.hpp"  // Ensures almond types are defined, like uint32, uint16, uint64, etc.

#ifdef __linux__

namespace almondnamespace
{
    // Ensure 1-byte packing for consistent layout across platforms
#pragma pack(push, 1)

// Define Handle as a generic 64-bit pointer type
    using Handle = void*;

    // Cross-platform DWORD and WORD definitions using almond types
    using DWORD = uint32;
    using WORD = uint16;

    struct MyPackedStruct {
        DWORD id;        // 32-bit identifier for uniform data layout
        WORD type;       // 16-bit type indicator for compact storage
        uint32 data;     // 32-bit data field

        uint64 largeData; // 64-bit data for compatibility with larger values in 64-bit environments
    };

#pragma pack(pop)  // Restore default packing after struct definition
}  // namespace almond
#endif