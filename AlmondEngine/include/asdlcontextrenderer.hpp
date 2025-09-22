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
 // asdlcontextrenderer.hpp
#pragma once

#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SDL)

#include "asdlcontext.hpp"
#include "aspritehandle.hpp"
#include "aatlastexture.hpp"
//#include "asdltextures.hpp"  // your SDL texture uploads / helpers

#include <span>
#include <iostream>

namespace almondnamespace::sdlcontext
{
    using almondnamespace::sdlcontext::state::s_sdlstate;

    struct RendererContext
    {
        enum class RenderMode {
            SingleTexture,
            TextureAtlas
        };
        RenderMode mode = RenderMode::TextureAtlas;

         SDL_Renderer* renderer {nullptr };  // SDL3 explicit renderer
        //SDL_Surface* renderer{ nullptr };
    };

    inline RendererContext sdl_renderer{};

	// Helper function to check and log SDL errors
    inline void check_sdl_error(const char* location)
    {
        const char* err = SDL_GetError();
        if (err && *err) {
            std::cerr << "[SDL ERROR] " << location << ": " << err << "\n";
            SDL_ClearError();
        }
    }

    inline void init_renderer(SDL_Renderer* renderer)
    {
        if (!renderer) {
            throw std::runtime_error("SDL_Renderer is null");
        }
        sdl_renderer.renderer = renderer;
        // Set default draw color to black
        SDL_SetRenderDrawColor(sdl_renderer.renderer, 0, 0, 0, 255);
        // Check for SDL errors
        check_sdl_error("init_renderer");
	}

    inline void begin_frame()
    {
        SDL_SetRenderDrawColor(sdl_renderer.renderer, 0, 0, 0, 255);
        SDL_RenderClear(sdl_renderer.renderer);
    }

    inline void end_frame()
    {
        SDL_RenderPresent(sdl_renderer.renderer);
    }

} // namespace almondnamespace::sdl
#endif