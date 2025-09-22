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

#include "aengineconfig.hpp"

#include <array>
#include <vector>

#if defined(ALMOND_USING_VULKAN)

namespace almondnamespace::vulkan {

    struct Vertex {
        alignas(8)  glm::vec2 pos;       // 8-byte aligned (vec2)
        alignas(16) glm::vec3 color;     // 16-byte aligned (vec3)
        alignas(8)  glm::vec2 texCoord;  // 8-byte aligned (vec2)

        static VkVertexInputBindingDescription get_binding_description() {
            return {
                .binding = 0,
                .stride = sizeof(Vertex),
                .inputRate = VK_VERTEX_INPUT_RATE_VERTEX
            };
        }

        static std::array<VkVertexInputAttributeDescription, 3> get_attribute_descriptions() {
            return { {
                    // Position (vec2)
                    {
                        .location = 0,
                        .binding = 0,
                        .format = VK_FORMAT_R32G32_SFLOAT,
                        .offset = offsetof(Vertex, pos)
                    },
                // Color (vec3)
                {
                    .location = 1,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32B32_SFLOAT,
                    .offset = offsetof(Vertex, color)
                },
                // Texture Coordinates (vec2)
                {
                    .location = 2,
                    .binding = 0,
                    .format = VK_FORMAT_R32G32_SFLOAT,
                    .offset = offsetof(Vertex, texCoord)
                }
            } };
        }
    };

    inline std::vector<Vertex> getQuadVertices() {
        return {
            // Positions (Vulkan NDC), Colors, Texture Coordinates
            {{-0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}}, // Bottom-left
            {{ 0.5f, -0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}, // Bottom-right
            {{ 0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}}, // Top-right
            {{-0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}}  // Top-left
        };
    }

    inline std::vector<uint32_t> getQuadIndices() {
        return { 0, 1, 2, 2, 3, 0 }; // CCW winding
    }
}
#endif