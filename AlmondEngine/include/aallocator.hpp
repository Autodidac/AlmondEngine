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
// almond_allocator.hpp ‑ functional, header‑only C++20
// -----------------------------------------------------
//  ▸ linear_arena  : bump‑pointer scratch allocator
//  ▸ block_pool    : fixed‑block freelist allocator
//
//  All interfaces are free functions in almondnamespace::mem.
//  Plug into std containers via <memory_resource>.
#pragma once

#include <cstddef>
#include <cstdint>
#include <memory_resource>
#include <new>
#include <type_traits>
#include <vector>

namespace almondnamespace::mem {

    // ─────────────────────────────────────────────────────────────────────────────
    // helpers
    // ─────────────────────────────────────────────────────────────────────────────
    template<std::size_t N> struct bytes_ { static constexpr std::size_t value = N; };
    template<std::size_t M> struct kilobytes_ { static constexpr std::size_t value = M * 1024; };
    template<std::size_t G> struct megabytes_ { static constexpr std::size_t value = G * 1024 * 1024; };

    template<class T>
    concept TriviallyDestructible = std::is_trivially_destructible_v<T>;

    // ─────────────────────────────────────────────────────────────────────────────
    // 1. linear_arena : bump pointer, reset en masse
    // ─────────────────────────────────────────────────────────────────────────────
    class linear_arena final : public std::pmr::memory_resource {
    public:
        linear_arena(std::byte* buffer, std::size_t bytes) noexcept
            : begin_{ buffer }, end_{ buffer + bytes }, curr_{ buffer } {
        }

        /// reset the arena (does NOT run dtors)
        void clear() noexcept { curr_ = begin_; }

        std::size_t capacity() const noexcept { return end_ - begin_; }
        std::size_t used()     const noexcept { return curr_ - begin_; }

    private:
        // std::pmr::memory_resource interface
        void* do_allocate(std::size_t n, std::size_t align) override {
            auto p = reinterpret_cast<std::uintptr_t>(curr_);
            auto adj = (align - (p % align)) % align;
            if (p + adj + n > reinterpret_cast<std::uintptr_t>(end_))
                throw std::bad_alloc{};
            curr_ = reinterpret_cast<std::byte*>(p + adj + n);
            return reinterpret_cast<void*>(p + adj);
        }
        void  do_deallocate(void*, std::size_t, std::size_t) noexcept override {}
        bool  do_is_equal(const std::pmr::memory_resource& o) const noexcept override {
            return this == &o;
        }

        std::byte* begin_;
        std::byte* end_;
        std::byte* curr_;
    };

    inline linear_arena& frame_arena() {
        static std::byte storage[megabytes_<4>::value];   // 4 MiB scratch
        static linear_arena arena{ storage, sizeof(storage) };
        return arena;
    }

    // ─────────────────────────────────────────────────────────────────────────────
    // 2. block_pool : fixed‑size freelist for T
    // ─────────────────────────────────────────────────────────────────────────────
    template<typename T, std::size_t N>
        requires TriviallyDestructible<T>
    class block_pool {
    public:
        constexpr block_pool() noexcept {
            std::byte* head = &storage_[0];
            for (std::size_t i = 0; i < N - 1; ++i) {
                *reinterpret_cast<std::byte**>(head) = head + block_size;
                head += block_size;
            }
            *reinterpret_cast<std::byte**>(head) = nullptr;
            freelist_ = &storage_[0];
        }

        [[nodiscard]] T* allocate() {
            if (!freelist_) throw std::bad_alloc{};
            std::byte* p = freelist_;
            freelist_ = *reinterpret_cast<std::byte**>(freelist_);
            return new (p) T;                       // placement‑new
        }

        void deallocate(T* obj) noexcept {
            obj->~T();                              // trivial, but keeps API honest
            std::byte* p = reinterpret_cast<std::byte*>(obj);
            *reinterpret_cast<std::byte**>(p) = freelist_;
            freelist_ = p;
        }

    private:
        static constexpr std::size_t block_size =
            (sizeof(T) + alignof(T) - 1) / alignof(T) * alignof(T);

        alignas(alignof(T)) std::byte storage_[block_size * N];
        std::byte* freelist_;
    };

    // ─────────────────────────────────────────────────────────────────────────────
    // usage helpers
    // ─────────────────────────────────────────────────────────────────────────────
    template<typename T, typename... Args>
    [[nodiscard]] T* make_frame(Args&&... args) {
        auto& arena = frame_arena();
        auto mem = arena.allocate(sizeof(T), alignof(T));
        return new (mem) T(std::forward<Args>(args)...);
    }
