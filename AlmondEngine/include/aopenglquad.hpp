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
// aopenglquad.hpp
#pragma once

#include "aengineconfig.hpp"

#include <glad/glad.h>
#include <stdexcept>
#include <vector>

#if defined(ALMOND_USING_OPENGL)

namespace almondnamespace::openglcontext {

    class Quad {
        GLuint vao_ = 0;
        GLuint vbo_ = 0;
        GLuint ebo_ = 0;

        constexpr static float defaultVertices[16] = {
            // pos.x, pos.y,  u, v
             0.f, 0.f,  0.f, 0.f,
             1.f, 0.f,  1.f, 0.f,
             1.f, 1.f,  1.f, 1.f,
             0.f, 1.f,  0.f, 1.f
        };

        constexpr static unsigned int defaultIndices[6] = {
            0,1,2, 2,3,0
        };

    public:
        Quad() {
            // Generate and bind VAO
            glGenVertexArrays(1, &vao_);
            glBindVertexArray(vao_);

            // Generate and bind VBO
            glGenBuffers(1, &vbo_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glBufferData(GL_ARRAY_BUFFER, sizeof(defaultVertices), defaultVertices, GL_STATIC_DRAW);

            // Setup vertex attributes: pos (location=0), uv (location=1)
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

            glEnableVertexAttribArray(1);
            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

            // Generate and bind EBO
            glGenBuffers(1, &ebo_);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(defaultIndices), defaultIndices, GL_STATIC_DRAW);

            // Unbind VAO (don't unbind EBO while VAO is bound)
            glBindVertexArray(0);

            // Cleanup bindings
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }

        ~Quad() {
            if (ebo_) glDeleteBuffers(1, &ebo_);
            if (vbo_) glDeleteBuffers(1, &vbo_);
            if (vao_) glDeleteVertexArrays(1, &vao_);
        }

        // Disable copy
        Quad(const Quad&) = delete;
        Quad& operator=(const Quad&) = delete;

        // Allow move
        Quad(Quad&& other) noexcept
            : vao_(other.vao_), vbo_(other.vbo_), ebo_(other.ebo_) {
            other.vao_ = 0;
            other.vbo_ = 0;
            other.ebo_ = 0;
        }
        Quad& operator=(Quad&& other) noexcept {
            if (this != &other) {
                if (vao_) glDeleteVertexArrays(1, &vao_);
                if (vbo_) glDeleteBuffers(1, &vbo_);
                if (ebo_) glDeleteBuffers(1, &ebo_);

                vao_ = other.vao_;
                vbo_ = other.vbo_;
                ebo_ = other.ebo_;

                other.vao_ = 0;
                other.vbo_ = 0;
                other.ebo_ = 0;
            }
            return *this;
        }

        GLuint vao() const noexcept { return vao_; }
        GLsizei index_count() const noexcept { return 6; }
    };

} // namespace almondnamespace::opengl
#endif
