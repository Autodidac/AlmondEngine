#pragma once

#include "framework.h"

#include <cstdint>
//#include <Windows.h>  // For Windows-specific HANDLE type

namespace almond {
    // Fixed-width integer types
    using uint16 = uint16_t;
    using uint32 = uint32_t;
    using uint64 = uint64_t;

    // Define platform-specific Handle type for Windows
    using Handle = HANDLE;
    

    // Windows DWORD and WORD definitions adapted for 64-bit compatibility
    using DWORD = uint32;
    using WORD  = uint16;

    // Structure with 64-bit compatibility and packed alignment for data transfer
    #pragma pack(push, 1)
    struct MyPackedStruct {
        DWORD id;     // 32-bit identifier
        WORD type;    // 16-bit type indicator
        uint32 data;  // 32-bit data field

        uint64 largeData; // Example 64-bit field for 64-bit data compatibility
    };
    #pragma pack(pop)  // Restore default packing alignment
}  // namespace almond
