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
 // acontextstate.hpp
#pragma once  

#include "aplatform.hpp"  
#include "aengineconfig.hpp"  
#include "acontextwindow.hpp" // Include the definition of WindowContext  

#ifdef ALMOND_USING_SDL
#include "asdlstate.hpp"
#endif
#ifdef ALMOND_USING_SFML
#include "asfmlstate.hpp"
#endif
#ifdef ALMOND_USING_RAYLIB
#include "araylibstate.hpp"
#endif
#ifdef ALMOND_USING_OPENGL
#include "aopenglstate.hpp"
#endif
#ifdef ALMOND_USING_VULKAN
#include "avulkanstate.hpp"
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
#include "asoftrenderer_state.hpp"
#endif

#include <memory>  
#include <atomic>  
#include <cstdint>  

// Forward declarations for subsystems, no ownership illusions
namespace almondnamespace::contextwindow { struct WindowContext; }  
namespace almondnamespace::input { struct Input; }  
//namespace almondnamespace::vr { struct VRSystem; }  
//namespace almondnamespace::renderer { struct Renderer; }  
//namespace almondnamespace::audio { struct AudioSystem; }  

// Cross-platform ContextState encapsulates the state of the engine context
namespace almondnamespace::state  
{  

#ifdef ALMOND_USING_SDL

    inline sdlcontext::state::SDL3State s_sdlstate{};

#endif
#ifdef ALMOND_USING_SFML

    inline sfmlcontext::state::SFML3State s_sfmlstate{};

#endif
#ifdef ALMOND_USING_RAYLIB

    inline raylibcontext::RaylibState s_raylibstate{};

#endif
#ifdef ALMOND_USING_OPENGL

    inline openglcontext::OpenGL4State s_openglstate{};

#endif
#ifdef ALMOND_USING_VULKAN

    inline vulkancontext::State s_vulkanstate{};

#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER

    inline anativecontext::SoftRendState s_softrendererstate{};
#endif

    struct ContextState final // ContextState encapsulates the state of the context
    {  
        // Subsystems: injected, no ownership illusions.  
        std::shared_ptr<contextwindow::WindowContext> window;  
        std::shared_ptr<input::Input> input;  
        //std::shared_ptr<vr::VRSystem> vr;  
        //std::shared_ptr<renderer::Renderer> renderer;  
        //std::shared_ptr<audio::AudioSystem> audio;  

        // Engine control  
        std::atomic<bool> isRunning{ true };  
        uint64_t frameCount = 0;  

        // Constructor  
        ContextState(  
            std::shared_ptr<contextwindow::WindowContext> win,  
            std::shared_ptr<input::Input> in  
            //std::shared_ptr<vr::VRSystem> vrSystem = nullptr,  
            //std::shared_ptr<renderer::Renderer> render = nullptr,  
            //std::shared_ptr<audio::AudioSystem> audioSys = nullptr  
        )  
            : window(std::move(win)),  
            input(std::move(in))  
            //vr(std::move(vrSystem)),  
            //renderer(std::move(render)),  
            //audio(std::move(audioSys))  
        {  
        }  

        //void update()  
        //{  
        //    if (window) window->pump_events();  
        //    if (input) input->poll(*window);  
        //    if (vr) vr->update();  
        //    if (audio) audio->update();  

        //    ++frameCount;  
        //}  

        void stop() noexcept { isRunning = false; }  
    };  
}
