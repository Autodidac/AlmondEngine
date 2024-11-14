#pragma once

#include "Types.h"  // Ensures almond types are defined, like uint32, uint16, uint64, etc.

#include <cstddef>

namespace almond {
    // Ensure 1-byte packing for consistent layout across platforms
    #pragma pack(push, 1)

    // Define Handle as a generic 64-bit pointer type
    using Handle = void*;

    // Cross-platform DWORD and WORD definitions using almond types
    using DWORD = uint32;
    using WORD  = uint16;

    struct MyPackedStruct {
        DWORD id;        // 32-bit identifier for uniform data layout
        WORD type;       // 16-bit type indicator for compact storage
        uint32 data;     // 32-bit data field

        uint64 largeData; // 64-bit data for compatibility with larger values in 64-bit environments
    };

    #pragma pack(pop)  // Restore default packing after struct definition
}  // namespace almond
