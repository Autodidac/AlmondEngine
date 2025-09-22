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

#ifdef ALMOND_USING_OPENGL
#ifdef ALMOND_USING_GLM

#include <vector>

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;
};

// Basic triangle vertex data (header-only)
inline std::vector<Vertex> getTriangleVertices() {
    return {
        {{0.0f,  0.5f}, {1.0f, 0.0f, 0.0f}},  // Top vertex (Red)
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},  // Right vertex (Green)
        {{-0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}}, // Left vertex (Blue)
    };
}

// Indices for drawing the triangle (header-only)
inline std::vector<uint32_t> getTriangleIndices() {
    return { 0, 1, 2 }; // Triangular face using the three vertices
}

#endif
#endif