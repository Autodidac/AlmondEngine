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
#include "ampmcboundedqueue.hpp"   // Lock‑free MPMCQueue<T>
#include "anet.hpp"                // for poll()

#include <span>
#include <atomic>
#include <chrono>
#include <fstream>
#include <coroutine>
#include <filesystem>
#include <functional>
#include <semaphore> // for cleaner future improvement
#include <thread>
#include <vector>

namespace almondnamespace 
{
    // —————————————————————————————————————————————————————————————————
    // Coroutine Task (public coroutine handle type)
    // —————————————————————————————————————————————————————————————————
    struct Task {
        struct promise_type {
            Task get_return_object() {
                return Task{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_always initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() noexcept {}
            void unhandled_exception() { std::terminate(); }
        };

        using handle_t = std::coroutine_handle<promise_type>;
        handle_t h;

        explicit Task(handle_t h_) noexcept : h(h_) {}
        Task(Task&& o) noexcept : h(o.h) { o.h = nullptr; }
        ~Task() { if (h) h.destroy(); }
        Task(const Task&) = delete;
        Task& operator=(const Task&) = delete;
    };

    // —————————————————————————————————————————————————————————————————
    // Internal job queue + worker pool
    // —————————————————————————————————————————————————————————————————
    static MPMCQueue<std::function<void()>> g_jobQueue{ 1024 };
    static std::vector<std::thread>         g_workers;
    static std::atomic<bool>                g_running{ false };

    inline void scheduler_start(int threadCount) {
        g_running = true;
        for (int i = 0; i < threadCount; ++i) {
            g_workers.emplace_back([] {
                std::function<void()> job;
                while (g_running) {
                    if (g_jobQueue.dequeue(job)) {
                        job();
                    }
                    else {
                        std::this_thread::yield();
                    }
                }
                // Drain remaining jobs
                while (g_jobQueue.dequeue(job)) {
                    job();
                }
                });
        }
    }

    inline void scheduler_stop() {
        g_running = false;
        for (auto& t : g_workers) {
            if (t.joinable()) t.join();
        }
        g_workers.clear();
    }

    inline void scheduler_enqueue(std::function<void()> job) {
        while (!g_jobQueue.enqueue(std::move(job))) {
            std::this_thread::yield();
        }
    }

    // —————————————————————————————————————————————————————————————————
    // Awaitables (no heap, no classes, clean C++20)
    // —————————————————————————————————————————————————————————————————

    // LoadAssetAwaitable — runs blocking disk I/O on a worker
    struct LoadAssetAwaitable {
        std::string path;

        bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) const noexcept {
            scheduler_enqueue([h, path = path]() {
                std::ifstream in(path, std::ios::binary);
                std::vector<std::byte> buf;
                if (in) {
                    in.seekg(0, std::ios::end);
                    buf.resize(static_cast<size_t>(in.tellg()));
                    in.seekg(0);
                    in.read(reinterpret_cast<char*>(buf.data()), buf.size());
                }
                h.resume(); // fire coroutine again
                });
        }

        std::vector<std::byte> await_resume() const noexcept {
            return {}; // optionally store/retrieve result externally
        }
    };

    // ReceiveNetworkAwaitable — polls network input then resumes
    struct ReceiveNetworkAwaitable {
        bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) const noexcept {
            scheduler_enqueue([h]() {
                net::poll(); // from anet.hpp
                h.resume();
                });
        }

        std::vector<std::byte> await_resume() const noexcept {
            return {}; // optionally return network message
        }
    };

    // NextFrame — requeues coroutine for next engine tick
    struct NextFrame {
        bool await_ready() const noexcept { return false; }

        void await_suspend(std::coroutine_handle<> h) const noexcept {
            scheduler_enqueue([h]() { h.resume(); });
        }

        void await_resume() const noexcept {}
    };

} // namespace almondnamespace
