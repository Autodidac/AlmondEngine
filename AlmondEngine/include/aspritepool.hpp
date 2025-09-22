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
 // aspritepool.hpp
#pragma once

#include <vector>
#include <atomic>
#include <optional>
#include <stdexcept>
#include <coroutine>
#include <memory>
#include <iostream>

#include "aspritehandle.hpp"
#include "aenginesystems.hpp"   // almondnamespace::Task
#include "ataskgraphwithdot.hpp" // TaskGraph

namespace almondnamespace::spritepool 
{

    using almondnamespace::SpriteHandle;
    using almondnamespace::Task;
    using almondnamespace::taskgraph::Node;

    // Pool state
    inline std::vector<uint8_t> usedFlags;       // 0 = free, 1 = used
    inline std::vector<uint32_t> generations;    // generation counters per slot
    inline std::atomic<size_t> lastAllocIndex{ 0 };
    inline size_t capacity = 0;

    // Optional task graph for async flow
    inline taskgraph::TaskGraph* g_taskGraph = nullptr;

    // === Lifecycle ===
    inline void initialize(size_t cap) noexcept {
        capacity = cap;
        usedFlags.assign(capacity, 0);
        generations.assign(capacity, 0);
        lastAllocIndex.store(0, std::memory_order_relaxed);
#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
        std::cerr << "[SpritePool] Initialized with capacity " << capacity << "\n";
#endif
    }

    inline void clear() noexcept {
        usedFlags.clear();
        generations.clear();
        lastAllocIndex.store(0, std::memory_order_relaxed);
        capacity = 0;
        std::cerr << "[SpritePool] Cleared\n";
    }

    inline void reset() noexcept {
        if (capacity == 0) {
            std::cerr << "[SpritePool] Warning: reset called on uninitialized pool\n";
            return;
        }
        std::fill(usedFlags.begin(), usedFlags.end(), 0);
        std::fill(generations.begin(), generations.end(), 0);
        lastAllocIndex.store(0, std::memory_order_relaxed);
#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
        std::cerr << "[SpritePool] Reset to initial state\n";
#endif
    }

    inline void set_task_graph(taskgraph::TaskGraph* graph) noexcept {
        g_taskGraph = graph;
        std::cerr << "[SpritePool] Task graph set\n";
    }

    inline void validate_pool() noexcept {
        size_t freeCount = 0;
        for (size_t i = 0; i < capacity; ++i) {
            if (usedFlags[i] == 0) ++freeCount;
        }
        std::cerr << "[SpritePool] validate_pool: " << freeCount << " free slots out of " << capacity << "\n";
    }

    // === Core allocation logic ===
    inline bool try_acquire_flag(size_t idx) noexcept {
        std::atomic_ref<uint8_t> flag(usedFlags[idx]);
        uint8_t expected = 0;
        bool result = flag.compare_exchange_strong(expected, 1, std::memory_order_acquire);
        if (!result) {
            std::cerr << "[SpritePool] try_acquire_flag failed for idx " << idx << "\n";
        }
        return result;
    }

    inline std::optional<size_t> try_allocate_round_robin() noexcept {
        const size_t size = capacity;
        const size_t start = lastAllocIndex.load(std::memory_order_relaxed);
#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
        std::cerr << "[SpritePool] Allocator starts at index " << start << "\n";
#endif 

        for (size_t offset = 0; offset < size; ++offset) {
            size_t idx = (start + offset) % size;
            if (try_acquire_flag(idx)) {
                lastAllocIndex.store((idx + 1) % size, std::memory_order_relaxed);
#if defined(DEBUG_TEXTURE_RENDERING_VERBOSE)
                std::cerr << "[SpritePool] Allocator grabbed idx " << idx << "\n";
#endif
                return idx;
            }
        }

        std::cerr << "[SpritePool] Allocator exhausted all slots.\n";
        return std::nullopt;
    }

    // === NEW: Synchronous allocate ===

    inline SpriteHandle allocate() noexcept {
        auto idxOpt = try_allocate_round_robin();
        if (!idxOpt.has_value()) {
            std::cerr << "[SpritePool] allocate() failed: no slots left\n";
            return SpriteHandle::invalid();
        }
        uint32_t id = static_cast<uint32_t>(*idxOpt);
        return SpriteHandle{ id, generations[id] };
    }

    // === Async allocate ===

    struct AllocateAwaitable;

    inline Task spritepool_allocation_coroutine(AllocateAwaitable* self);

    struct AllocateAwaitable {
        std::optional<size_t> index;
        std::coroutine_handle<> awaitingCoroutine = nullptr;

        bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) noexcept {
            awaitingCoroutine = h;

            if (!g_taskGraph) {
                index = try_allocate_round_robin();
                if (awaitingCoroutine) awaitingCoroutine.resume();
                return;
            }

            Task task = spritepool_allocation_coroutine(this);
            auto node = std::make_unique<Node>(std::move(task));
            node->Label = "SpritePoolAllocate";
            g_taskGraph->AddNode(std::move(node));
            g_taskGraph->Execute();
        }

        SpriteHandle await_resume() noexcept {
            if (!index.has_value()) {
                std::cerr << "[SpritePool] Warning: allocation failed, no free sprites available\n";
                return SpriteHandle::invalid();
            }
            uint32_t id = static_cast<uint32_t>(*index);
            if (id >= generations.size()) {
                std::cerr << "[SpritePool] Warning: allocation failed, index out of bounds: " << id << "\n";
                return SpriteHandle::invalid();
            }
            return SpriteHandle{ id, generations[id] };
        }
    };

    inline Task spritepool_allocation_coroutine(AllocateAwaitable* self) {
        self->index = try_allocate_round_robin();
        if (self->awaitingCoroutine) self->awaitingCoroutine.resume();
        co_return;
    }

    inline AllocateAwaitable allocateAsync() noexcept {
        return {};
    }

    // === Free / check ===
    inline void free(const SpriteHandle& handle) noexcept {
        const size_t idx = static_cast<size_t>(handle.id) & 0xFFF;
        if (idx >= capacity) return;

        std::atomic_ref<uint8_t> flag(usedFlags[idx]);
        flag.store(0, std::memory_order_release);
        ++generations[idx];
    }

    inline bool is_alive(const SpriteHandle& handle) noexcept {
        const size_t idx = static_cast<size_t>(handle.id) & 0xFFF;
        if (idx >= capacity) return false;

        std::atomic_ref<uint8_t> flag(usedFlags[idx]);
        if (flag.load(std::memory_order_acquire) == 0) return false;

        return generations[idx] == handle.generation;
    }
} // namespace almondnamespace::spritepool
