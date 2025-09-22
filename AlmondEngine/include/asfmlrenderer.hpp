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
 // asfmlrenderer.hpp
#pragma once

#include "aengineconfig.hpp"

#if defined(ALMOND_USING_SFML)

#include "aspritehandle.hpp"
#include "aatlastexture.hpp"
#include "asfmlstate.hpp"  // you’ll need a wrapper for sf::RenderWindow etc.
#include "asfmltextures.hpp"

#include <iostream>
#include <span>

//#include <SFML/Graphics.hpp>
//#include <SFML/Window.hpp> // for sf::RenderWindow, sf::Event, etc.
//#include <SFML/System.hpp> // for sf::Vector2f, sf::IntRect, etc.

namespace almondnamespace::sfmlcontext
{
    //using almondnamespace::sfmlcontext::state::s_state;

    struct RendererContext
    {
        enum class RenderMode
        {
            SingleTexture,
            TextureAtlas
        };

        RenderMode mode = RenderMode::TextureAtlas;
    };

    inline RendererContext sfml_renderer{};

    // ──────────────────────────────────────────────
    // Begin frame
    // ──────────────────────────────────────────────
   // inline void begin_frame()
   // {
        //if (!sfml_state.window)
        //{
        //    throw std::runtime_error("[SFML] begin_frame: Render window is null.");
        //}
        //sfml_state.window->clear(sf::Color::White);
   // }

    // ──────────────────────────────────────────────
    // End frame
    // ──────────────────────────────────────────────
    //inline void end_frame()
   // {
 /*       if (!sfml_state.window)
        {
            throw std::runtime_error("[SFML] end_frame: Render window is null.");
        }
        sfml_state.window->display();*/
  //  }




} // namespace almondnamespace::sfmlcontext

#endif // ALMOND_USING_SFML
