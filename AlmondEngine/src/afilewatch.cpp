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
#include "pch.h"

#include "afilewatch.hpp"

#include <array>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace almondnamespace 
{
    static std::uint64_t compute_file_hash(const std::string& path) {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            return 0;
        }

        constexpr std::uint64_t kOffsetBasis = 1469598103934665603ull;
        constexpr std::uint64_t kPrime = 1099511628211ull;

        std::uint64_t hash = kOffsetBasis;
        std::array<char, 4096> buffer{};

        while (file) {
            file.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
            const std::streamsize readCount = file.gcount();
            for (std::streamsize i = 0; i < readCount; ++i) {
                hash ^= static_cast<unsigned char>(buffer[static_cast<std::size_t>(i)]);
                hash *= kPrime;
            }
        }

        return hash;
    }

    std::vector<file_state> get_file_states(const std::string& folder) {
        std::vector<file_state> result;

        std::error_code ec;
        if (!fs::exists(folder, ec) || ec) {
            return result;
        }

        for (const auto& entry : fs::directory_iterator(folder, ec)) {
            if (ec) {
                break;
            }

            if (entry.is_regular_file() && entry.path().extension() == ".cpp") {
                file_state fs;
                fs.path = entry.path().string();

                auto last_write = fs::last_write_time(entry, ec);
                if (ec) {
                    ec.clear();
                    continue;
                }

                fs.last_write_time = last_write.time_since_epoch().count();
                fs.hash = compute_file_hash(fs.path);
                fs.dirty = false;
                result.push_back(std::move(fs));
            }
        }
        return result;
    }

    void scan_and_mark_changes(std::vector<file_state>& files) {
        for (auto& f : files) {
            std::error_code ec;
            auto write_time = fs::last_write_time(f.path, ec);
            if (ec) {
                f.last_write_time = 0;
                f.hash = 0;
                f.dirty = true;
                continue;
            }

            auto new_time = write_time.time_since_epoch().count();
            if (new_time != f.last_write_time) {
                f.last_write_time = new_time;

                const auto new_hash = compute_file_hash(f.path);
                if (new_hash == 0) {
                    f.hash = 0;
                    f.dirty = true;
                    continue;
                }

                if (new_hash != f.hash) {
                    f.hash = new_hash;
                    f.dirty = true;
                }
                else {
                    f.dirty = false;
                }
            }
            else {
                f.dirty = false;
            }
        }
    }
}
