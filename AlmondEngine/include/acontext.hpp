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
 // acontext.hpp
#pragma once

#include "aplatform.hpp"        // Must always come first for platform defines
#include "aengineconfig.hpp"    // All ENGINE-specific includes
#include "ainput.hpp"           // Keycodes and input handling
#include "aatlastexture.hpp"    // TextureAtlas type
#include "aspritehandle.hpp"    // SpriteHandle type
#include "aatomicfunction.hpp"  // AlmondAtomicFunction
#include "acommandqueue.hpp"    // CommandQueue
#include "awindowdata.hpp"      // WindowData

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <span>
#include <functional>
#include <cstdint>
#include <mutex>
#include <queue>

namespace almondnamespace::core
{
    struct WindowData; // forward declaration

    // ======================================================
    // Context: core per-backend state
    // Each backend (OpenGL, SDL, Raylib, etc.) wires its
    // rendering + input hooks here.
    // ======================================================
    struct Context {
        // --- Backend render hooks (fast raw function pointers) ---
        using InitializeFunc = void(*)();
        using CleanupFunc = void(*)();
        using ProcessFunc = bool(*)(Context&, CommandQueue&);
        using ClearFunc = void(*)();
        using PresentFunc = void(*)();
        using GetWidthFunc = int(*)();
        using GetHeightFunc = int(*)();
        using RegistryGetFunc = int(*)(const char*);
        using DrawSpriteFunc = void(*)(SpriteHandle, std::span<const TextureAtlas* const>, float, float, float, float);
        using AddTextureFunc = uint32_t(*)(TextureAtlas&, std::string, const ImageData&);
        using AddAtlasFunc = uint32_t(*)(const TextureAtlas&);
        using AddModelFunc = int(*)(const char*, const char*);

#if defined(_WIN32) || defined(WIN32)
        HWND hwnd = nullptr;
        HDC  hdc = nullptr;
        HGLRC hglrc = nullptr;
#else
        void* hwnd = nullptr;
        void* hdc = nullptr;
        void* hglrc = nullptr;
#endif
        WindowData* windowData = nullptr;

        int width = 400;
        int height = 300;
        ContextType type = ContextType::Custom;
        std::string backendName;

        // --- Backend function pointers ---
        InitializeFunc initialize = nullptr;
        CleanupFunc cleanup = nullptr;
        ProcessFunc process = nullptr;
        ClearFunc clear = nullptr;
        PresentFunc present = nullptr;
        GetWidthFunc get_width = nullptr;
        GetHeightFunc get_height = nullptr;
        RegistryGetFunc registry_get = nullptr;
        DrawSpriteFunc draw_sprite = nullptr;
        AddModelFunc add_model = nullptr;

        // --- Input hooks (std::function for flexibility) ---
        std::function<bool(input::Key)> is_key_held;
        std::function<bool(input::Key)> is_key_down;
        std::function<void(int&, int&)> get_mouse_position;
        std::function<bool(input::MouseButton)> is_mouse_button_held;
        std::function<bool(input::MouseButton)> is_mouse_button_down;

        // --- High-level callbacks ---
        AlmondAtomicFunction<uint32_t(TextureAtlas&, std::string, const ImageData&)> add_texture;
        AlmondAtomicFunction<uint32_t(const TextureAtlas&)> add_atlas;
        std::function<void(int, int)> onResize;

        Context() = default;

        Context(const std::string& name, ContextType t,
            InitializeFunc init, CleanupFunc cln, ProcessFunc proc,
            ClearFunc clr, PresentFunc pres, GetWidthFunc gw, GetHeightFunc gh,
            RegistryGetFunc rg, DrawSpriteFunc ds,
            AddTextureFunc at, AddAtlasFunc aa, AddModelFunc am)
            : width(400), height(300), type(t), backendName(name),
            initialize(init), cleanup(cln), process(proc), clear(clr),
            present(pres), get_width(gw), get_height(gh),
            registry_get(rg), draw_sprite(ds), add_model(am)
        {
            add_texture.store(at);
            add_atlas.store(aa);
        }

        // --- Safe wrappers ---
        inline void initialize_safe() const noexcept { if (initialize) initialize(); }
        inline void cleanup_safe()   const noexcept { if (cleanup) cleanup(); }
        bool process_safe(Context& ctx, CommandQueue& queue);

        inline void clear_safe(std::shared_ptr<Context>) const noexcept { if (clear) clear(); }
        inline void present_safe() const noexcept { if (present) present(); }

        inline int  get_width_safe()  const noexcept { return get_width ? get_width() : width; }
        inline int  get_height_safe() const noexcept { return get_height ? get_height() : height; }

        inline bool is_key_held_safe(input::Key k) const noexcept {
            return is_key_held ? is_key_held(k) : false;
        }
        inline bool is_key_down_safe(input::Key k) const noexcept {
            return is_key_down ? is_key_down(k) : false;
        }
        inline void get_mouse_position_safe(int& x, int& y) const noexcept {
            if (get_mouse_position) get_mouse_position(x, y); else { x = y = 0; }
        }
        inline bool is_mouse_button_held_safe(input::MouseButton b) const noexcept {
            return is_mouse_button_held ? is_mouse_button_held(b) : false;
        }
        inline bool is_mouse_button_down_safe(input::MouseButton b) const noexcept {
            return is_mouse_button_down ? is_mouse_button_down(b) : false;
        }

        inline int registry_get_safe(const char* key) const noexcept {
            return registry_get ? registry_get(key) : 0;
        }

        inline void draw_sprite_safe(SpriteHandle h,
            std::span<const TextureAtlas* const> atlases,
            float x, float y, float w, float hgt) const noexcept {
            if (draw_sprite) draw_sprite(h, atlases, x, y, w, hgt);
        }

        inline uint32_t add_texture_safe(TextureAtlas& atlas,
            std::string name,
            const ImageData& img) const noexcept {
            try { return add_texture(atlas, name, img); }
            catch (...) { return 0u; }
        }

        inline uint32_t add_atlas_safe(const TextureAtlas& atlas) const noexcept {
            try { return add_atlas(atlas); }
            catch (...) { return 0u; }
        }

        inline int add_model_safe(const char* name, const char* path) const noexcept {
            return add_model ? add_model(name, path) : -1;
        }
    };

    // ======================================================
    // BackendState: holds master + duplicates for a backend
    // ======================================================
    struct BackendState {
        std::shared_ptr<Context> master;
        std::vector<std::shared_ptr<Context>> duplicates;

        // opaque per-backend storage (cast in backends)
        std::unique_ptr<void, void(*)(void*)> data{ nullptr, [](void*) {} };
    };

    using BackendMap = std::map<ContextType, BackendState>;
    extern BackendMap g_backends;

    void InitializeAllContexts();
    void AddContextForBackend(ContextType type, std::shared_ptr<Context> context);
    bool ProcessAllContexts();

} // namespace almondnamespace::core
