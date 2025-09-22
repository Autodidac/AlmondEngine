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
 // ataskgraphwithdot.hpp
#pragma once

#include "ampmcboundedqueue.hpp"
#include "aenginesystems.hpp" // Reuse almondnamespace::Task

#include <atomic>
#include <coroutine>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>
#include <semaphore>

namespace almondnamespace 
{
    namespace taskgraph 
    {
        struct Node {
            Task Task_;
            std::atomic<int> PrereqCount{ 0 };
            std::vector<Node*> Dependents;
            std::string Label;

            explicit Node(Task&& t) : Task_(std::move(t)) {}
        };

        using NodePtr = std::unique_ptr<Node>;

        class TaskGraph {
        public:
            explicit TaskGraph(size_t workerCount)
                : Queue_(1024), Running_(true), WorkSem_(0) {
                for (size_t i = 0; i < workerCount; ++i)
                    Workers_.emplace_back(&TaskGraph::WorkerLoop, this);
            }

            ~TaskGraph() {
                Running_ = false;
                for (size_t i = 0; i < Workers_.size(); ++i)
                    WorkSem_.release();
                for (auto& t : Workers_)
                    if (t.joinable())
                        t.join();
            }

            void AddNode(NodePtr node) {
                Nodes_.push_back(std::move(node));
            }

            void AddDependency(Node& a, Node& b) {
                b.PrereqCount.fetch_add(1, std::memory_order_relaxed);
                a.Dependents.push_back(&b);
            }

            void Execute() {
                for (auto& n : Nodes_) {
                    if (n->PrereqCount.load(std::memory_order_acquire) == 0) {
                        Queue_.enqueue(n.get());
                        WorkSem_.release();
                    }
                }
            }

            void WaitAll() {
                while (true) {
                    bool busy = !Queue_.empty();
                    for (auto& n : Nodes_) {
                        if (n->PrereqCount.load(std::memory_order_acquire) >= 0) {
                            busy = true;
                            break;
                        }
                    }
                    if (!busy)
                        break;
                    std::this_thread::yield();
                }
            }

            void DumpDot(const std::string& path = "graph.dot") {
                std::ofstream out(path);
                out << "digraph G{";
                    for (auto& n : Nodes_) {
                        auto id = reinterpret_cast<std::uintptr_t>(n.get());
                        out << "N" << id << "[label="" << n->Label << ""];";
                    }
                for (auto& n : Nodes_) {
                    auto s = reinterpret_cast<std::uintptr_t>(n.get());
                    for (auto* d : n->Dependents) {
                        auto t = reinterpret_cast<std::uintptr_t>(d);
                        out << "N" << s << "->N" << t << ";";
                    }
                }
                out << "}";
            }

        private:
            void WorkerLoop() {
                Node* n = nullptr;
                while (Running_) {
                    WorkSem_.acquire();
                    if (!Running_) break;
                    if (!Queue_.dequeue(n)) continue;

                    if (!n || !n->Task_.h) {
#ifndef NDEBUG
                        std::cerr << "[TaskGraph] WARNING: null coroutine handle, skipping";
#endif
                            continue;
                    }

                    n->Task_.h.resume();
                    for (auto* d : n->Dependents) {
                        if (d->PrereqCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                            Queue_.enqueue(d);
                            WorkSem_.release();
                        }
                    }
                    n->PrereqCount.store(-1, std::memory_order_release);
                }

                while (Queue_.dequeue(n)) {
                    if (!n || !n->Task_.h) continue;
                    n->Task_.h.resume();
                    for (auto d : n->Dependents) {
                        if (d->PrereqCount.fetch_sub(1, std::memory_order_acq_rel) == 1) {
                            Queue_.enqueue(d);
                            WorkSem_.release();
                        }
                    }
                    n->PrereqCount.store(-1, std::memory_order_release);
                }
            }

            MPMCQueue<Node*> Queue_;
            std::vector<std::thread> Workers_;
            std::atomic<bool> Running_;
            std::counting_semaphore<> WorkSem_;
            std::vector<NodePtr> Nodes_;
        };

    } // namespace taskgraph
} // namespace almondnamespace
