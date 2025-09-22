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
 // arenderer.hpp
#pragma once

#include "aplatform.hpp"
#include "aspriteregistry.hpp"
#include "aatlastexture.hpp"

#ifdef ALMOND_USING_VULKAN
#include "avulkanrenderer.hpp"
#endif
#ifdef ALMOND_USING_OPENGL
#include "aopenglrenderer.hpp"
#endif

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <string>
#include <span>

namespace almondnamespace
{
    struct CommandBuffer {
        std::vector<std::string> commands;

        void add_command(const std::string& command) {
            commands.push_back(command);
        }

        void execute() const {
            for (const auto& cmd : commands) {
                std::cout << "Executing: " << cmd << '\n';
            }
        }
    };

    struct Renderer {
        std::vector<std::shared_ptr<TextureAtlas>> atlases;
        std::shared_ptr<SpriteRegistry> spriteRegistry;
        void* platformContext = nullptr;

    private:
        std::vector<const TextureAtlas*> atlasPtrsCache;

    public:
        Renderer(std::vector<std::shared_ptr<TextureAtlas>> atlasList,
            std::shared_ptr<SpriteRegistry> registry,
            void* context)
            : atlases(std::move(atlasList))
            , spriteRegistry(std::move(registry))
            , platformContext(context)
        {
            if (atlases.empty() || !spriteRegistry) {
                throw std::invalid_argument("Renderer: atlases and spriteRegistry must not be null or empty.");
            }

            atlasPtrsCache.reserve(atlases.size());
            for (const auto& atlas : atlases) {
                atlasPtrsCache.push_back(atlas.get());
            }
        }

        [[nodiscard]] bool is_initialized() const noexcept {
            return !atlases.empty() && spriteRegistry != nullptr;
        }

        bool set_shader_pipeline(const std::string& vertPath, const std::string& fragPath) const
        {
            platform_set_shader_pipeline(vertPath, fragPath);
            std::cerr << "[Renderer] set_shader_pipeline is not yet implemented.\n";
            return false;
        }

        void draw_sprite(const std::string& spriteName, float x, float y, float width, float height, CommandBuffer& cmd) const
        {
            if (!is_initialized()) {
                std::cerr << "[Renderer] draw_sprite called before initialization.\n";
                return;
            }

            auto spriteOpt = spriteRegistry->get(spriteName);
            if (!spriteOpt) {
                std::cerr << "[Renderer] Sprite '" << spriteName << "' not found.\n";
                return;
            }

            const auto& [handle, u0, v0, u1, v1, pivotX, pivotY] = *spriteOpt;

            if (!handle.is_valid()) {
                std::cerr << "[Renderer] Invalid handle for sprite '" << spriteName << "'.\n";
                return;
            }

            std::span<const TextureAtlas* const> atlasSpan{ atlasPtrsCache.data(), atlasPtrsCache.size() };

            cmd.add_command("Draw sprite: " + spriteName);

            platform_draw_sprite(handle, atlasSpan, x, y, width, height);
        }

        void draw_sprite(SpriteHandle handle, 
            float x, float y, float width, float height, CommandBuffer& cmd) const
        {
            if (!is_initialized()) {
                std::cerr << "[Renderer] draw_sprite called before initialization.\n";
                return;
            }

            size_t atlasIdx = (handle.id >> 12) & 0xFFF;
            if (atlasIdx >= atlases.size()) {
                std::cerr << "[Renderer] Invalid atlas index in draw_sprite.\n";
                return;
            }

            std::span<const TextureAtlas* const> atlasSpan{ atlasPtrsCache.data(), atlasPtrsCache.size() };

            cmd.add_command("Draw sprite with handle " + std::to_string(handle.id));

            platform_draw_sprite(handle, atlasSpan, x, y, width, height);
        }

    private:
        void platform_draw_sprite(SpriteHandle handle, std::span<const TextureAtlas* const> atlases,
            float x, float y, float width, float height) const
        {
#ifdef ALMOND_USING_VULKAN
            if (platformContext) {
                vulkan::draw_sprite(handle, atlases, x, y, width, height);
            }
#elif defined(ALMOND_USING_OPENGL)
            if (platformContext) {
                opengltextures::draw_sprite(handle, atlases, x, y, width, height);
            }
#else
            std::cerr << "[Renderer] No platform renderer selected.\n";
#endif
        }

        void platform_set_shader_pipeline(const std::string& vertPath, const std::string& fragPath) const
        {
#ifdef ALMOND_USING_VULKAN
            if (platformContext) {
                vulkan::set_shader_pipeline(vertPath, fragPath);
            }
#elif defined(ALMOND_USING_OPENGL)
            if (platformContext) {
                // opengl::set_shader_pipeline(vertPath, fragPath); // TODO: implement
            }
#else
            std::cerr << "[Renderer] No platform renderer selected.\n";
#endif
        }
    };

} // namespace almondnamespace

