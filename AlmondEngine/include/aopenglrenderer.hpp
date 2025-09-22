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
 // aopenglrenderer.hpp
#pragma once

#include "aengineconfig.hpp"
//#include "aopenglcontext.hpp"
#include "aatlastexture.hpp"
#include "aspritehandle.hpp"
#include "aopengltextures.hpp" // AtlasGPU + gpu_atlases + ensure_uploaded
#include "aopenglquad.hpp"
#include "aopenglstate.hpp"

#include <span>
#include <iostream>

#if defined (ALMOND_USING_OPENGL)
namespace almondnamespace::openglrenderer
{
	//using almondnamespace::openglcontext::state::s_openglstate;

	struct RendererContext
	{
		enum class RenderMode {
			SingleTexture,
			TextureAtlas
		};
		RenderMode mode = RenderMode::TextureAtlas;
	};

	inline void check_gl_error(const char* location) {
		GLenum err = glGetError();
		if (err != GL_NO_ERROR) {
			std::cerr << "[GL ERROR] " << location << " error: 0x" << std::hex << err << std::dec << "\n";
		}
	}

	inline bool is_handle_live(const SpriteHandle& handle) noexcept {
		return handle.is_valid();
	}

    inline void debug_gl_state(GLuint shader, GLuint expectedVAO, GLuint expectedTex,
        GLint uUVRegionLoc, GLint uTransformLoc) noexcept
    {
        GLint boundVAO = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &boundVAO);
        std::cerr << "[Debug] Bound VAO: " << boundVAO << ", Expected VAO: " << expectedVAO << "\n";

        GLint activeTexUnit = 0;
        glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexUnit);
        std::cerr << "[Debug] Active Texture Unit: 0x" << std::hex << activeTexUnit << std::dec << "\n";

        GLint boundTex = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &boundTex);
        std::cerr << "[Debug] Bound Texture: " << boundTex << ", Expected Texture: " << expectedTex << "\n";

        if (uUVRegionLoc >= 0)
        {
            GLfloat uvRegion[4] = {};
            glGetUniformfv(shader, uUVRegionLoc, uvRegion);
            std::cerr << "[Debug] Uniform uUVRegion: ("
                << uvRegion[0] << ", " << uvRegion[1] << ", "
                << uvRegion[2] << ", " << uvRegion[3] << ")\n";
        }

        if (uTransformLoc >= 0)
        {
            GLfloat transform[4] = {};
            glGetUniformfv(shader, uTransformLoc, transform);
            std::cerr << "[Debug] Uniform uTransform: ("
                << transform[0] << ", " << transform[1] << ", "
                << transform[2] << ", " << transform[3] << ")\n";
        }
    }

    inline void draw_debug_outline() noexcept
    {
        glUseProgram(s_openglstate.shader);

        if (s_openglstate.uUVRegionLoc >= 0)
            glUniform4f(s_openglstate.uUVRegionLoc, 0, 0, 1, 1);

        if (s_openglstate.uTransformLoc >= 0)
            glUniform4f(s_openglstate.uTransformLoc, -1, -1, 2, 2);

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(s_openglstate.vao);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_BLEND);

        glLineWidth(2.0f);

        glDrawArrays(GL_LINE_LOOP, 0, 4);

        glBindVertexArray(0);
        glLineWidth(1.0f);
    }

//    // Diagnostic draw_sprite version
    inline void draw_sprite(SpriteHandle handle, std::span<const TextureAtlas* const> atlases,
        float x, float y, int width, int height) noexcept
    {
        if (!is_handle_live(handle)) {
            std::cerr << "[DrawSprite] Invalid sprite handle.\n";
            return;
        }

        const int w = width;
        const int h = height;
        if (w == 0 || h == 0) {
            std::cerr << "[DrawSprite] ERROR: Window dimensions are zero.\n";
            return;
        }

#if defined(DEBUG_TEXTURE_RENDERING)
        std::cerr << "[DrawSprite] Inputs: x=" << x << ", y=" << y << ", w=" << width << ", h=" << height << '\n';
#endif

        const int atlasIdx = int(handle.atlasIndex);
        const int localIdx = int(handle.localIndex);

        if (atlasIdx < 0 || atlasIdx >= int(atlases.size())) {
            std::cerr << "[DrawSprite] Atlas index out of bounds: " << atlasIdx << '\n';
            return;
        }
        const TextureAtlas* atlas = atlases[atlasIdx];
        if (!atlas) {
            std::cerr << "[DrawSprite] Null atlas pointer at index: " << atlasIdx << '\n';
            return;
        }
        if (localIdx < 0 || localIdx >= int(atlas->entries.size())) {
            std::cerr << "[DrawSprite] Sprite index out of bounds: " << localIdx << '\n';
            return;
        }

        std::cerr << "[DrawSprite] Using atlas index = " << atlasIdx
            << ", atlas name = '" << atlas->name << "'"
            << ", local sprite index = " << localIdx
            << ", sprite name = '" << atlas->entries[localIdx].name << "'\n";

        std::cerr << "[DrawSprite] Pointer key: " << atlas
            << ", Atlas name: " << atlas->name << "\n";

        //ensure_uploaded(*atlas);

        auto it = opengltextures::opengl_gpu_atlases.find(atlas);
        if (it == opengltextures::opengl_gpu_atlases.end()) {
            std::cerr << "[DrawSprite] GPU texture not found for atlas '" << atlas->name << "'\n";
            return;
        }
        GLuint tex = it->second.textureHandle;
        if (!tex) {
            std::cerr << "[DrawSprite] GPU texture handle is 0 for atlas '" << atlas->name << "'\n";
            return;
        }


        glUseProgram(openglcontext::s_openglstate.shader);
        glBindVertexArray(openglcontext::s_openglstate.vao);

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex);

        const auto& entry = atlas->entries[localIdx];

        // Flip V coords for OpenGL (bottom-left origin)
        float u0 = entry.region.u1;
        float u1 = entry.region.u2;
        float v0 = 1.0f - entry.region.v1; 
        float v1 = 1.0f - entry.region.v2; 

        float du = u1 - u0;
        float dv = v1 - v0;
        std::cerr << "[DrawSprite] UVs: u0=" << u0 << ", u1=" << u1 << ", du=" << du << "\n";
        std::cerr << "[DrawSprite] UVs: v0=" << v0 << ", v1=" << v1 << ", dv=" << dv << "\n";
        if (openglcontext::s_openglstate.uUVRegionLoc >= 0)
            glUniform4f(openglcontext::s_openglstate.uUVRegionLoc, u0, v0, du, dv);

        // Flip Y pixel coordinate *before* normalization
        float flippedY = h - (y + height * 0.5f);

        // Convert to NDC [-1, 1], center coords
        float ndc_x = ((x + width * 0.5f) / float(w)) * 2.f - 1.f;
        float ndc_y = (flippedY / float(h)) * 2.f - 1.f;
        float ndc_w = (width / float(w)) * 2.f;
        float ndc_h = (height / float(h)) * 2.f;

#if defined(DEBUG_TEXTURE_RENDERING_VERY_VERBOSE)
        std::cerr << "[DrawSprite] Atlas entries count: " << atlas->entries.size() << "\n";
        std::cerr << "[DrawSprite] Requested sprite index: " << localIdx << "\n";

        if (y < 0) {
            std::cerr << "[Warning] Negative y coordinate: " << y << '\n';
        }

        std::cerr << "[DrawSprite] Using region for '" << entry.name << "': "
            << "u1=" << entry.region.u1 << ", v1=" << entry.region.v1
            << ", u2=" << entry.region.u2 << ", v2=" << entry.region.v2
            << ", x=" << entry.region.x << ", y=" << entry.region.y
            << ", w=" << entry.region.width << ", h=" << entry.region.height << '\n';
#endif
        if (openglcontext::s_openglstate.uTransformLoc >= 0)
            glUniform4f(openglcontext::s_openglstate.uTransformLoc, ndc_x, ndc_y, ndc_w, ndc_h);

        //debug_gl_state(s_state.shader, s_state.vao, tex, s_state.uUVRegionLoc, s_state.uTransformLoc);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        //draw_debug_outline();

        // unbind VAO
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glDisable(GL_BLEND);

#if defined(DEBUG_TEXTURE_RENDERING_VERY_VERBOSE)
        std::cerr << "[DrawSprite] Atlas '" << atlas->name << "' Sprite '" << entry.name
            << "' AtlasIdx=" << atlasIdx << " SpriteIdx=" << localIdx << '\n';
#endif
    }


    inline void draw_quad(const openglcontext::Quad& quad, GLuint texture) {
        glUseProgram(s_openglstate.shader);

        // Set UV region for full quad (if needed)
        if (s_openglstate.uUVRegionLoc >= 0)
            glUniform4f(s_openglstate.uUVRegionLoc, 0.f, 0.f, 1.f, 1.f);

        // Bind texture explicitly
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        // Set sampler uniform once if not already done
        // glUniform1i(s_state.uTextureLoc, 0);

        glBindVertexArray(quad.vao());

        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawElements(GL_TRIANGLES, quad.index_count(), GL_UNSIGNED_INT, nullptr);

        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);

        glDisable(GL_BLEND);
    }


	inline void begin_frame() {
		// Set a clear color explicitly, e.g. blue
		glClearColor(0.f, 0.f, 1.f, 1.f);
		glViewport(0, 0, core::cli::window_width, core::cli::window_height);
		glClear(GL_COLOR_BUFFER_BIT);
	}

	inline void end_frame() {
		//present();
	}

} // namespace almondnamespace::opengl
#endif // ALMOND_USING_OPENGL