#pragma once

#ifdef _WIN32
#include <cstdint>
#endif
#ifdef __linux__
#include <cstddef>
#endif

namespace almond {
    // Define fixed-width integer types for cross-platform compatibility
    using int8   = int8_t;
    using uint8  = uint8_t;
    using int16  = int16_t;
    using uint16 = uint16_t;
    using int32  = int32_t;
    using uint32 = uint32_t;
    using int64  = int64_t;
    using uint64 = uint64_t;

    // Pointer-sized integer types for 64-bit compatibility
    using intptr  = std::intptr_t;
    using uintptr = std::uintptr_t;

    // Cross-platform helper for byte-swapping (endianness)
    [[nodiscard]] constexpr uint32 swap_endian(uint32 value) noexcept {
        return (value << 24) | ((value << 8) & 0x00FF0000) | ((value >> 8) & 0x0000FF00) | (value >> 24);
    }
}  // namespace almond
