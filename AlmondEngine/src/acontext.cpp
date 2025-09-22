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
 // acontext.cpp
#include "pch.h"

#include "aplatform.hpp"
#include "aengineconfig.hpp"
#include "acontext.hpp"
#include "acontextwindow.hpp"
#include "acontextmultiplexer.hpp"

#ifdef ALMOND_USING_OPENGL
#include "aopenglcontext.hpp"
#include "aopenglrenderer.hpp"
#include "aopengltextures.hpp"
#endif
#ifdef ALMOND_USING_VULKAN
#include "avulkancontext.hpp"
#include "avulkanrenderer.hpp"
#include "avulkantextures.hpp"
#endif
#ifdef ALMOND_USING_DIRECTX
#include "adirectxcontext.hpp"
#include "adirectxrenderer.hpp"
#include "adirectxtextures.hpp"
#endif
#ifdef ALMOND_USING_SDL
#include "asdlcontext.hpp"
#include "asdlcontextrenderer.hpp"
#include "asdltextures.hpp"
#endif
#ifdef ALMOND_USING_SFML
#include "asfmlcontext.hpp"
#include "asfmlrenderer.hpp"
#include "asfmltextures.hpp"
#endif
#ifdef ALMOND_USING_RAYLIB
#include "araylibcontext.hpp"
#include "araylibrenderer.hpp"
#include "araylibtextures.hpp"
#endif
#ifdef ALMOND_USING_CUSTOM
#include "acustomcontext.hpp"
#include "acustomrenderer.hpp"
#include "acustomtextures.hpp"
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
#include "asoftrenderer_context.hpp"
#include "asoftrenderer_renderer.hpp"
#include "asoftrenderer_textures.hpp"
#endif

#include "aimageloader.hpp"
#include "aatlastexture.hpp"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include <queue>

namespace almondnamespace::core {

    // ─── Global backends ──────────────────────────────────────
    std::map<ContextType, BackendState> g_backends{};

    // ─── AddContextForBackend ─────────────────────────────────
    void AddContextForBackend(ContextType type, std::shared_ptr<Context> context) {
        auto& backendState = g_backends[type];

        if (!backendState.master)
            backendState.master = std::move(context);
        else
            backendState.duplicates.emplace_back(std::move(context));

        // allocate backend-specific data if not already set
        if (!backendState.data) {
            switch (type) {
#ifdef ALMOND_USING_OPENGL
            case ContextType::OpenGL:
                backendState.data = {
                    new opengltextures::BackendData(),
                    [](void* p) { delete static_cast<opengltextures::BackendData*>(p); }
                };
                break;
#endif
#ifdef ALMOND_USING_SOFTWARE_RENDERER
            case ContextType::Software:
                backendState.data = {
                    new anativecontext::BackendData(),
                    [](void* p) { delete static_cast<anativecontext::BackendData*>(p); }
                };
                break;
#endif
            default:
                break;
            }
        }
    }

    // ─── Context::process_safe ────────────────────────────────
    bool Context::process_safe(Context& ctx, CommandQueue& queue) {
        if (!process) return false;
        try {
            return process(ctx, queue);
        }
        catch (const std::exception& e) {
            std::cerr << "[Context] Exception in process: " << e.what() << "\n";
            return false;
        }
        catch (...) {
            std::cerr << "[Context] Unknown exception in process\n";
            return false;
        }
    }

    // ─── Atlas Thunks ─────────────────────────────────────────
    inline uint32_t AddTextureThunk(TextureAtlas& atlas, std::string name, const ImageData& img, ContextType type) {
        switch (type) {
#ifdef ALMOND_USING_OPENGL
        case ContextType::OpenGL: return opengltextures::atlas_add_texture(atlas, name, img);
#endif
#ifdef ALMOND_USING_SDL
        case ContextType::SDL:  return sdlcontext::atlas_add_texture(atlas, name, img);
#endif
#ifdef ALMOND_USING_SFML
        case ContextType::SFML:  return sfmlcontext::atlas_add_texture(atlas, name, img);
#endif
#ifdef ALMOND_USING_RAYLIB
        case ContextType::RayLib: return raylibtextures::atlas_add_texture(atlas, name, img);
#endif
#ifdef ALMOND_USING_VULKAN
        case ContextType::Vulkan: return vulkancontext::atlas_add_texture(atlas, name, img);
#endif
#ifdef ALMOND_USING_DIRECTX
        case ContextType::DirectX: return directxcontext::atlas_add_texture(atlas, name, img);
#endif
        default: (void)atlas; (void)name; (void)img; return 0;
        }
    }

    inline uint32_t AddAtlasThunk(TextureAtlas atlas, ContextType type) {
        switch (type) {
#ifdef ALMOND_USING_OPENGL
        case ContextType::OpenGL: return opengltextures::load_atlas(atlas, 0);
#endif
#ifdef ALMOND_USING_SDL
        case ContextType::SDL:  return sdlcontext::load_atlas(atlas, 0);
#endif
#ifdef ALMOND_USING_SFML
        case ContextType::SFML: return sfmlcontext::load_atlas(atlas, 0);
#endif
#ifdef ALMOND_USING_RAYLIB
        case ContextType::RayLib: return raylibtextures::load_atlas(atlas, 0);
#endif
#ifdef ALMOND_USING_VULKAN
        case ContextType::Vulkan: return vulkancontext::load_atlas(atlas, 0);
#endif
#ifdef ALMOND_USING_DIRECTX
        case ContextType::DirectX: return directxcontext::load_atlas(atlas, 0);
#endif
        default: (void)atlas; return 0;
        }
    }

    // ─── Backend stubs (minimal no-op defaults) ──────────────
#ifdef ALMOND_USING_OPENGL
    inline void opengl_initialize() {}
    inline void opengl_cleanup() {}
    bool opengl_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void opengl_clear() {}
    inline void opengl_present() {}
    inline int  opengl_get_width() { return 1280; }
    inline int  opengl_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_SDL
    inline void sdl_initialize() {}
    inline void sdl_cleanup() {}
    bool sdl_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void sdl_clear() {}
    inline void sdl_present() {}
    inline int  sdl_get_width() { return 1280; }
    inline int  sdl_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_SFML
    inline void sfml_initialize() {}
    inline void sfml_cleanup() {}
    bool sfml_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void sfml_clear() {}
    inline void sfml_present() {}
    inline int  sfml_get_width() { return 1280; }
    inline int  sfml_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_RAYLIB
    inline void raylib_initialize() {}
    inline void raylib_cleanup() {}
    bool raylib_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void raylib_clear() {}
    inline void raylib_present() {}
    inline int  raylib_get_width() { return 1280; }
    inline int  raylib_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_VULKAN
    inline void vulkan_initialize() {}
    inline void vulkan_cleanup() {}
    bool vulkan_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void vulkan_clear() {}
    inline void vulkan_present() {}
    inline int  vulkan_get_width() { return 1280; }
    inline int  vulkan_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_DIRECTX
    inline void directx_initialize() {}
    inline void directx_cleanup() {}
    bool directx_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void directx_clear() {}
    inline void directx_present() {}
    inline int  directx_get_width() { return 1280; }
    inline int  directx_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_CUSTOM
    inline void custom_initialize() {}
    inline void custom_cleanup() {}
    bool custom_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void custom_clear() {}
    inline void custom_present() {}
    inline int  custom_get_width() { return 1280; }
    inline int  custom_get_height() { return 720; }
#endif

#ifdef ALMOND_USING_SOFTWARE_RENDERER
    inline void softrenderer_initialize() {}
    inline void softrenderer_cleanup() {}
    bool softrenderer_process(Context& ctx, CommandQueue& queue) { queue.drain(); return true; }
    inline void softrenderer_clear() {}
    inline void softrenderer_present() {}
    inline int  softrenderer_get_width() { return 800; }
    inline int  softrenderer_get_height() { return 600; }
#endif


    // ─── Init all backends ───────────────────────────────────
    void InitializeAllContexts() {

#if defined(ALMOND_USING_OPENGL)
        auto openglContext = std::make_shared<Context>();
        openglContext->initialize = opengl_initialize;
        openglContext->cleanup = opengl_cleanup;
        openglContext->process = opengl_process;
        openglContext->clear = opengl_clear;
        openglContext->present = opengl_present;
        openglContext->get_width = opengl_get_width;
        openglContext->get_height = opengl_get_height;

        openglContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        openglContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        openglContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        openglContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        openglContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        openglContext->registry_get = [](const char*) { return 0; };
        openglContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        openglContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::OpenGL);
            };
        openglContext->add_atlas = [&](const TextureAtlas& a) {
            return AddAtlasThunk(a, ContextType::OpenGL);
            };

        openglContext->add_model = [](const char*, const char*) { return 0; };

        openglContext->backendName = "OpenGL";
        openglContext->type = ContextType::OpenGL;
        AddContextForBackend(ContextType::OpenGL, openglContext);
#endif

#if defined(ALMOND_USING_SDL)
        auto sdlContext = std::make_shared<Context>();
        sdlContext->initialize = sdl_initialize;
        sdlContext->cleanup = sdl_cleanup;
        sdlContext->process = sdl_process;
        sdlContext->clear = sdl_clear;
        sdlContext->present = sdl_present;
        sdlContext->get_width = sdl_get_width;
        sdlContext->get_height = sdl_get_height;

        sdlContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        sdlContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        sdlContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        sdlContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        sdlContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        sdlContext->registry_get = [](const char*) { return 0; };
        sdlContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        sdlContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::SDL);
            };
        sdlContext->add_atlas = [](const TextureAtlas&) { return 1; };
        sdlContext->add_model = [](const char*, const char*) { return 0; };

        sdlContext->backendName = "SDL";
        sdlContext->type = ContextType::SDL;
        AddContextForBackend(ContextType::SDL, sdlContext);
#endif

#if defined(ALMOND_USING_SFML)
        auto sfmlContext = std::make_shared<Context>();
        sfmlContext->initialize = sfml_initialize;
        sfmlContext->cleanup = sfml_cleanup;
        sfmlContext->process = sfml_process;
        sfmlContext->clear = sfml_clear;
        sfmlContext->present = sfml_present;
        sfmlContext->get_width = sfml_get_width;
        sfmlContext->get_height = sfml_get_height;

        sfmlContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        sfmlContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        sfmlContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        sfmlContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        sfmlContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        sfmlContext->registry_get = [](const char*) { return 0; };
        sfmlContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        sfmlContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::SFML);
            };
        sfmlContext->add_atlas = [](const TextureAtlas&) { return 1; };
        sfmlContext->add_model = [](const char*, const char*) { return 0; };

        sfmlContext->backendName = "SFML";
        sfmlContext->type = ContextType::SFML;
        AddContextForBackend(ContextType::SFML, sfmlContext);
#endif

#if defined(ALMOND_USING_RAYLIB)
        auto raylibContext = std::make_shared<Context>();
        raylibContext->initialize = raylib_initialize;
        raylibContext->cleanup = raylib_cleanup;
        raylibContext->process = raylib_process;
        raylibContext->clear = raylib_clear;
        raylibContext->present = raylib_present;
        raylibContext->get_width = raylib_get_width;
        raylibContext->get_height = raylib_get_height;

        raylibContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        raylibContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        raylibContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        raylibContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        raylibContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        raylibContext->registry_get = [](const char*) { return 0; };
        raylibContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        raylibContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::RayLib);
            };
        raylibContext->add_atlas = [](const TextureAtlas&) { return 1; };
        raylibContext->add_model = [](const char*, const char*) { return 0; };

        raylibContext->backendName = "RayLib";
        raylibContext->type = ContextType::RayLib;
        AddContextForBackend(ContextType::RayLib, raylibContext);
#endif

#if defined(ALMOND_USING_VULKAN)
        auto vulkanContext = std::make_shared<Context>();
        vulkanContext->initialize = vulkan_initialize;
        vulkanContext->cleanup = vulkan_cleanup;
        vulkanContext->process = vulkan_process;
        vulkanContext->clear = vulkan_clear;
        vulkanContext->present = vulkan_present;
        vulkanContext->get_width = vulkan_get_width;
        vulkanContext->get_height = vulkan_get_height;

        vulkanContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        vulkanContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        vulkanContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        vulkanContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        vulkanContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        vulkanContext->registry_get = [](const char*) { return 0; };
        vulkanContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        vulkanContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::Vulkan);
            };
        vulkanContext->add_atlas = [](const TextureAtlas&) { return 1; };
        vulkanContext->add_model = [](const char*, const char*) { return 0; };

        vulkanContext->backendName = "Vulkan";
        vulkanContext->type = ContextType::Vulkan;
        AddContextForBackend(ContextType::Vulkan, vulkanContext);
#endif

#if defined(ALMOND_USING_DIRECTX)
        auto directxContext = std::make_shared<Context>();
        directxContext->initialize = directx_initialize;
        directxContext->cleanup = directx_cleanup;
        directxContext->process = directx_process;
        directxContext->clear = directx_clear;
        directxContext->present = directx_present;
        directxContext->get_width = directx_get_width;
        directxContext->get_height = directx_get_height;

        directxContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        directxContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        directxContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        directxContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        directxContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        directxContext->registry_get = [](const char*) { return 0; };
        directxContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        directxContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::DirectX);
            };
        directxContext->add_atlas = [](const TextureAtlas&) { return 1; };
        directxContext->add_model = [](const char*, const char*) { return 0; };

        directxContext->backendName = "DirectX";
        directxContext->type = ContextType::DirectX;
        AddContextForBackend(ContextType::DirectX, directxContext);
#endif

#if defined(ALMOND_USING_CUSTOM)
        auto customContext = std::make_shared<Context>();
        customContext->initialize = custom_initialize;
        customContext->cleanup = custom_cleanup;
        customContext->process = custom_process;
        customContext->clear = custom_clear;
        customContext->present = custom_present;
        customContext->get_width = custom_get_width;
        customContext->get_height = custom_get_height;

        customContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        customContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        customContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        customContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        customContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        customContext->registry_get = [](const char*) { return 0; };
        customContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        customContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::Custom);
            };
        customContext->add_atlas = [](const TextureAtlas&) { return 1; };
        customContext->add_model = [](const char*, const char*) { return 0; };

        customContext->backendName = "Custom";
        customContext->type = ContextType::Custom;
        AddContextForBackend(ContextType::Custom, customContext);
#endif

#if defined(ALMOND_USING_SOFTWARE_RENDERER)
        auto softwareContext = std::make_shared<Context>();
        softwareContext->initialize = softrenderer_initialize;
        softwareContext->cleanup = softrenderer_cleanup;
        softwareContext->process = softrenderer_process;
        softwareContext->clear = softrenderer_clear;
        softwareContext->present = softrenderer_present;
        softwareContext->get_width = softrenderer_get_width;
        softwareContext->get_height = softrenderer_get_height;

        softwareContext->is_key_held = [](input::Key k) { return input::is_key_held(k); };
        softwareContext->is_key_down = [](input::Key k) { return input::is_key_down(k); };
        softwareContext->get_mouse_position = [](int& x, int& y) { x = input::mouseX; y = input::mouseY; };
        softwareContext->is_mouse_button_held = [](input::MouseButton b) { return input::is_mouse_button_held(b); };
        softwareContext->is_mouse_button_down = [](input::MouseButton b) { return input::is_mouse_button_down(b); };

        softwareContext->registry_get = [](const char*) { return 0; };
        softwareContext->draw_sprite = [](SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float) {};

        softwareContext->add_texture = [&](TextureAtlas& a, const std::string& n, const ImageData& i) {
            return AddTextureThunk(a, n, i, ContextType::Software);
            };
        softwareContext->add_atlas = [](const TextureAtlas&) { return 1; };
        softwareContext->add_model = [](const char*, const char*) { return 0; };

        softwareContext->backendName = "Software";
        softwareContext->type = ContextType::Software;
        AddContextForBackend(ContextType::Software, softwareContext);
#endif
    }

} // namespace almondnamespace::core
