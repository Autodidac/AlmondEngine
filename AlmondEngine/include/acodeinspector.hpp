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

#undef min

#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string_view>
#include <algorithm>

namespace almondnamespace::codeinspector {

    struct InspectionResult {
        std::filesystem::path filePath;
        std::vector<std::string> issues;
    };

    struct LineStats {
        size_t total = 0;
        size_t blank = 0;
        size_t comments = 0;
        size_t code = 0;

        void accumulate(const LineStats& other) {
            total += other.total;
            blank += other.blank;
            comments += other.comments;
            code += other.code;
        }

        std::vector<std::string> format_summary() const {
            return {
                "Total lines: " + std::to_string(total),
                "Blank lines: " + std::to_string(blank),
                "Comment lines: " + std::to_string(comments),
                "Code lines: " + std::to_string(code)
            };
        }
    };

    inline std::pair<bool, std::string> has_header_protection(const std::vector<std::string>& lines) {
        constexpr size_t max_search_lines = 30;
        bool has_pragma_once = false;
        bool has_ifndef_guard = false;

        for (size_t i = 0; i < std::min(lines.size(), max_search_lines); ++i) {
            std::string_view line = lines[i];

            if (line.find("#pragma once") != std::string_view::npos) {
                has_pragma_once = true;
            }

            if (line.find("#ifndef") != std::string_view::npos || line.find("#if !defined") != std::string_view::npos) {
                has_ifndef_guard = true;
            }

            if (has_pragma_once && has_ifndef_guard) {
                return { true, "Conflicting header guards: both #pragma once and #ifndef found" };
            }
        }

        if (has_pragma_once || has_ifndef_guard) {
            return { true, "" };
        }

        return { false, "Missing header guard" };
    }

    inline bool uses_almond_namespace(const std::vector<std::string>& lines) {
        constexpr std::string_view ns = "namespace almondnamespace";
        return std::any_of(lines.begin(), lines.end(),
            [&](const std::string& line) { return line.find(ns) != std::string::npos; });
    }

    inline std::vector<std::string> check_trailing_whitespace(const std::vector<std::string>& lines) {
        std::vector<std::string> issues;
        for (size_t i = 0; i < lines.size(); ++i) {
            const auto& line = lines[i];
            if (!line.empty()) {
                size_t last_non_ws = line.find_last_not_of(" \t\r\n");
                if (last_non_ws != std::string::npos && last_non_ws != line.size() - 1)
                    issues.emplace_back("Trailing whitespace on line " + std::to_string(i + 1));
            }
        }
        return issues;
    }

    inline LineStats analyze_line_stats(const std::vector<std::string>& lines) {
        LineStats stats;
        stats.total = lines.size();

        bool in_block_comment = false;

        for (const auto& line : lines) {
            std::string_view trimmed = line;
            // Trim leading whitespace
            size_t start = trimmed.find_first_not_of(" \t");
            if (start == std::string_view::npos) {
                ++stats.blank;
                continue;
            }
            trimmed.remove_prefix(start);

            if (in_block_comment) {
                ++stats.comments;
                if (trimmed.find("*/") != std::string_view::npos) {
                    in_block_comment = false;
                }
            }
            else if (trimmed.starts_with("//")) {
                ++stats.comments;
            }
            else if (trimmed.starts_with("/*")) {
                ++stats.comments;
                if (trimmed.find("*/") == std::string_view::npos) {
                    in_block_comment = true;
                }
            }
            else {
                ++stats.code;
            }
        }

        return stats;
    }

    inline std::vector<std::string> read_file_lines(const std::filesystem::path& path) {
        std::ifstream file(path);
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(file, line)) {
            lines.push_back(std::move(line));
        }
        return lines;
    }

    inline std::vector<InspectionResult> inspect_directory(const std::filesystem::path& directory, LineStats& totalStatsOut) {
        std::vector<InspectionResult> results;
        totalStatsOut = {}; // reset totals

        for (const auto& entry : std::filesystem::recursive_directory_iterator(directory)) {
            if (!entry.is_regular_file()) continue;

            const auto& path = entry.path();

            // Skip unwanted folders
            if (path.string().find("vcpkg_installed") != std::string::npos)
                continue;

            if (path.extension() != ".hpp" && path.extension() != ".cpp" &&
                path.extension() != ".inl" && path.extension() != ".h")
                continue;

            auto lines = read_file_lines(path);
            if (lines.empty()) continue;

            InspectionResult result;
            result.filePath = path;

            // Header guard check
            auto [hasGuard, guardError] = has_header_protection(lines);
            if (!hasGuard) {
                result.issues.emplace_back("Missing #pragma once or include guard");
            }
            else if (!guardError.empty()) {
                result.issues.emplace_back(guardError);
            }

            // Namespace check
            if (!uses_almond_namespace(lines)) {
                result.issues.emplace_back("Does not use 'almondnamespace' namespace");
            }

            // Trailing whitespace
            auto trailingIssues = check_trailing_whitespace(lines);
            result.issues.insert(result.issues.end(), trailingIssues.begin(), trailingIssues.end());

            // Line stats
            auto stats = analyze_line_stats(lines);
            totalStatsOut.accumulate(stats);

            // Append per file stats summary
            auto summary = stats.format_summary();
            for (auto& line : summary) {
                result.issues.emplace_back(line);
            }

            results.push_back(std::move(result));
        }

        return results;
    }


} // namespace almondnamespace::codeinspector
