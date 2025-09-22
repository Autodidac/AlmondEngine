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
// aopenglcontext.hpp
#pragma once

#include "aplatform.hpp"
#include "aengineconfig.hpp"    // brings in <windows.h>, <glad/glad.h>, etc.

#if defined(ALMOND_USING_OPENGL)

//#include "acontext.hpp"       // Context, ContextType
#include "acontextmultiplexer.hpp" // BackendMap, windows
#include "awindowdata.hpp"

//#include "acontextwindow.hpp"   // WindowContext::getWindowHandle()
#include "aplatformpump.hpp"    // platform::pump_events()
#include "aatlasmanager.hpp"
#include "arobusttime.hpp"      // RobustTime
#include "ainput.hpp"

//#include "aopenglstate.hpp"  // brings in the actual inline s_state
//#include "aopenglcontextinput.hpp"   // WndProcHook()
//#include "aopenglrenderer.hpp" // RendererContext, RenderMode
#include "aopengltextures.hpp" // ensure_uploaded, AtlasGPU, gpu_atlases 

//#include <array>
//#include <chrono>
//#include <format>
//#include <iostream>
//#include <stdexcept>
//#include <bitset>
//#include <mutex>
//#include <queue>
//#include <functional>
#include <format>
#include <iostream>
#include <stdexcept>
#include <functional>
#include <mutex>
#include <queue>

namespace almondnamespace::openglcontext
{
    // — Helpers
    inline void* LoadGLFunc(const char* name)
    {
        if (auto p = (void*)wglGetProcAddress(name)) return p;
        static HMODULE m = LoadLibraryA("opengl32.dll");
        return m ? (void*)GetProcAddress(m, name) : nullptr;
    }

    inline GLuint compileShader(GLenum type, const char* src)
    {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            char log[512]{};
            glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
            throw std::runtime_error(std::format("Shader compile error: {}\nSource:\n{}", log, src));
        }
        return shader;
    }

    inline GLuint linkProgram(const char* vsSrc, const char* fsSrc)
    {
        GLuint vertexShader = compileShader(GL_VERTEX_SHADER, vsSrc);
        GLuint fragmentShader = compileShader(GL_FRAGMENT_SHADER, fsSrc);
        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        GLint linked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, &linked);
        if (!linked) {
            char log[512]{};
            glGetProgramInfoLog(program, sizeof(log), nullptr, log);
            throw std::runtime_error(std::format("Program link error: {}", log));
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        return program;
    }

    // — Engine hooks
    inline bool opengl_initialize(std::shared_ptr<core::Context> ctx,
        HWND parentWnd = nullptr,
        unsigned int w = 800,
        unsigned int h = 600,
        std::function<void(int, int)> onResize = nullptr)
    {
        static bool initialized = false;
        if (initialized) return true;
        initialized = true;


        auto& backend = almondnamespace::opengltextures::get_opengl_backend();
        auto& glState = backend.glState;

        std::cerr << "[OpenGL Init] Starting initialization\n";

        // -----------------------------------------------------------------
        // Step 1: Resolve window/context handles
        // -----------------------------------------------------------------
        HWND   resolvedHwnd = nullptr;
        HDC    resolvedHdc = nullptr;
        HGLRC  resolvedHglrc = nullptr;

        if (ctx && ctx->windowData) {
            resolvedHwnd = ctx->windowData->hwnd;
            resolvedHdc = ctx->windowData->hdc;
            resolvedHglrc = ctx->windowData->glContext;
            std::cerr << "[OpenGL Init] Using ctx->windowData handles\n";
        }

        // Fallbacks: pull directly from current thread
        if (!resolvedHdc)   resolvedHdc = wglGetCurrentDC();
        if (!resolvedHglrc) resolvedHglrc = wglGetCurrentContext();

        // Parent HWND priority
        HWND resolvedParent = parentWnd ? parentWnd : resolvedHwnd;

        // Assign hwnd only if we actually resolved one
        if (resolvedHwnd) {
            glState.hwnd = resolvedHwnd;
            std::cerr << "[OpenGL Init] Using resolved hwnd=" << resolvedHwnd << "\n";
        }
        else if (parentWnd) {
            glState.hwnd = parentWnd;
            std::cerr << "[OpenGL Init] Using parent hwnd=" << parentWnd << "\n";
        }
        else {
            // Leave glState.hwnd unchanged if it was already set earlier
            std::cerr << "[OpenGL Init] WARNING: No hwnd resolved; keeping existing glState.hwnd="
                << glState.hwnd << "\n";
        }

        // Always update HDC / HGLRC from current thread
        glState.hdc = resolvedHdc;
        glState.hglrc = resolvedHglrc;
        glState.parent = resolvedParent;

        std::cerr << "[OpenGL Init] Final handles: hwnd=" << glState.hwnd
            << " hdc=" << glState.hdc
            << " hglrc=" << glState.hglrc << "\n";

        // -----------------------------------------------------------------
        // Step 2: Cache size and callback into Context
        // -----------------------------------------------------------------
        ctx->width = static_cast<int>(w);
        ctx->height = static_cast<int>(h);
        ctx->onResize = std::move(onResize);


        if (!glState.hwnd) {
            glState.hwnd = CreateWindowEx(
                0, L"AlmondChild", L"working",
                WS_CHILD | WS_VISIBLE | WS_BORDER,
                CW_USEDEFAULT, CW_USEDEFAULT, w, h,
                glState.parent, nullptr, nullptr, nullptr);
        }

        if (!glState.hwnd)
            throw std::runtime_error("[OpenGLContext] Failed to create child window");

        ShowWindow(glState.hwnd, SW_SHOW);

        // Device context
        glState.hdc = GetDC(glState.hwnd);
        if (!glState.hdc)
            throw std::runtime_error("GetDC failed");

        PIXELFORMATDESCRIPTOR pfd = { sizeof(pfd), 1,
            PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
            PFD_TYPE_RGBA, 32, 0,0,0,0,0,0,0,0,0,24,0,0,0,0,0,0,0 };

        int pf = ChoosePixelFormat(glState.hdc, &pfd);
        if (!pf) throw std::runtime_error("ChoosePixelFormat failed");
        if (!SetPixelFormat(glState.hdc, pf, &pfd))
            throw std::runtime_error("SetPixelFormat failed");

        // Temporary context to load extensions
        HGLRC tmpContext = wglCreateContext(glState.hdc);
        if (!tmpContext) throw std::runtime_error("wglCreateContext failed");
        if (!wglMakeCurrent(glState.hdc, tmpContext))
            throw std::runtime_error("wglMakeCurrent failed");

        auto wglCreateContextAttribsARB =
            (PFNWGLCREATECONTEXTATTRIBSARBPROC)LoadGLFunc("wglCreateContextAttribsARB");
        if (!wglCreateContextAttribsARB)
            throw std::runtime_error("Failed to load wglCreateContextAttribsARB");

        int attribs[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 6,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

        glState.hglrc = wglCreateContextAttribsARB(glState.hdc, nullptr, attribs);
        if (!glState.hglrc)
            throw std::runtime_error("Failed to create OpenGL 4.6 context");

        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(tmpContext);

        if (!wglMakeCurrent(glState.hdc, glState.hglrc))
            throw std::runtime_error("Failed to make OpenGL context current");

        if (!gladLoadGL())
            throw std::runtime_error("Failed to initialize GLAD");

        // Compile & link shaders (vertex and fragment)
        constexpr auto vs_source = R"(
        #version 460 core

        layout(location = 0) in vec2 aPos;       // [-0.5..0.5] quad coords
        layout(location = 1) in vec2 aTexCoord;  // [0..1] UV coords

        uniform vec4 uTransform; // xy = center in NDC, zw = size in NDC
        uniform vec4 uUVRegion;  // xy = UV offset, zw = UV size

        out vec2 vUV;

        void main() {
            vec2 pos = aPos * uTransform.zw + uTransform.xy;
            gl_Position = vec4(pos, 0.0, 1.0);
            vUV = uUVRegion.xy + vec2(aTexCoord.x, 1.0 - aTexCoord.y) * uUVRegion.zw;
        }
    )";

        constexpr auto fs_source = R"(
        #version 460 core
        in vec2 vUV;
        out vec4 outColor;

        uniform sampler2D uTexture;

        void main() {
            outColor = texture(uTexture, vUV);
        }
    )";


        // Compile shader program
        glState.shader = linkProgram(vs_source, fs_source);
        glState.uUVRegionLoc = glGetUniformLocation(glState.shader, "uUVRegion");
        glState.uTransformLoc = glGetUniformLocation(glState.shader, "uTransform");
        glState.uSamplerLoc = glGetUniformLocation(glState.shader, "uTexture");

        glUseProgram(glState.shader);
        if (glState.uSamplerLoc >= 0)
            glUniform1i(glState.uSamplerLoc, 0);
        glUseProgram(0);

        std::cerr << "[OpenGL Init] Shader program compiled/linked\n";

        // -----------------------------------------------------------------
        // Step 8: Quad VAO/VBO/EBO
        // -----------------------------------------------------------------
        glGenVertexArrays(1, &glState.vao);
        glGenBuffers(1, &glState.vbo);
        glGenBuffers(1, &glState.ebo);

        glBindVertexArray(glState.vao);

        constexpr float quadVerts[] = {
            -0.5f, -0.5f,    0.0f, 0.0f,
             0.5f, -0.5f,    1.0f, 0.0f,
             0.5f,  0.5f,    1.0f, 1.0f,
            -0.5f,  0.5f,    0.0f, 1.0f
        };

        glBindBuffer(GL_ARRAY_BUFFER, glState.vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVerts), quadVerts, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        constexpr unsigned int quadIndices[] = { 
            0, 1, 2,
            2, 3, 0 
        };
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glState.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quadIndices), quadIndices, GL_STATIC_DRAW);

        glBindVertexArray(0);

        std::cerr << "[OpenGL Init] Quad VAO/VBO/EBO created\n";

        // -----------------------------------------------------------------
        // Step 9: Hook texture uploads
        // -----------------------------------------------------------------
        atlasmanager::ensure_uploaded_backend = [](const TextureAtlas& atlas) {
            opengltextures::ensure_uploaded(atlas);
            };

        // -----------------------------------------------------------------
        // Step 10: Sync context with GL state
        // -----------------------------------------------------------------
        ctx->hwnd = glState.hwnd;
        ctx->hdc = glState.hdc;
        ctx->hglrc = glState.hglrc;

        std::cerr << "[OpenGL Init] Context sync complete\n";
        return true;
}

    inline void opengl_present() {
        auto& backend = opengltextures::get_opengl_backend();
        auto& glState = backend.glState;        // IMPORTANT: attach our static state
        SwapBuffers(glState.hdc);
    }

    //   inline void opengl_pump() { almondnamespace::platform::pump_events(); }
    inline int  opengl_get_width() { 
        RECT r; GetClientRect(s_openglstate.hwnd, &r); return r.right - r.left; 
    }
    inline int  opengl_get_height() { 
        RECT r; GetClientRect(s_openglstate.hwnd, &r); return r.bottom - r.top;
    }
    inline void opengl_clear(core::Context& ctx) {
        glViewport(0, 0, ctx.width, ctx.height);
        glClearColor(0.f, 0.f, 1.f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    // ------------------------------------------------------
    // Per-frame process: clear, run commands, swap
    // ------------------------------------------------------
    inline bool opengl_process(core::Context& ctx, core::CommandQueue& queue)
    {
        auto& backend = opengltextures::get_opengl_backend();
        auto& glState = backend.glState;

        if (!ctx.hdc || !ctx.hglrc) {
            std::cerr << "[OpenGL] Context not ready (hdc=" << ctx.hdc
                << ", hglrc=" << ctx.hglrc << ")\n";
            return false;
        }

        if (!wglMakeCurrent(ctx.hdc, ctx.hglrc)) return true;

        // ─── Time & events ──────────────────────────────────────────────────────
        //static auto lastReal = std::chrono::steady_clock::now();
        //auto nowReal = std::chrono::steady_clock::now();
        //s_clockSystem.advanceTime(nowReal - lastReal);
        //lastReal = nowReal;

        //platform::pump_events();

        //// ─── FPS logging ────────────────────────────────────────────────────────
        //++s_frameCount;
        //if (s_pollTimer.elapsed() >= 0.3) {
        //    s_pollTimer.restart();
        //}
        //if (s_fpsTimer.elapsed() >= 1.0) {
        //    std::cout << "FPS: " << s_frameCount << "\n";
        //    s_frameCount = 0;
        //    s_fpsTimer.restart();
        //}


        opengl_clear(ctx);

        // --- Debug sanity: check VAO/EBO existence before use ---
        if (glState.vao == 0 || glState.vbo == 0 || glState.ebo == 0) {
            std::cerr << "[OpenGL] VAO/VBO/EBO not initialized!\n";
            queue.drain();
            SwapBuffers(ctx.hdc);
            wglMakeCurrent(nullptr, nullptr);
            return true;
        }

        if (!atlasmanager::atlas_vector.empty()) {
            const auto* atlas = atlasmanager::atlas_vector[0];
            auto it = backend.gpu_atlases.find(atlas);
            if (it != backend.gpu_atlases.end()) {
                GLuint tex = it->second.textureHandle;

                if (tex == 0) {
                    std::cerr << "[OpenGL] Warning: texture handle is 0\n";
                }
                else {
                    glUseProgram(glState.shader);
                    glBindVertexArray(glState.vao);

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, tex);

                    if (glState.uTransformLoc >= 0)
                        glUniform4f(glState.uTransformLoc,
                            0.0f, 0.0f,   // center
                            2.0f, 2.0f);  // span whole screen

                    if (glState.uUVRegionLoc >= 0)
                        glUniform4f(glState.uUVRegionLoc,
                            0.0f, 0.0f,
                            1.0f, 1.0f);

                    // Double-check bound EBO before drawing
                    GLint boundEbo = 0;
                    glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &boundEbo);
                    if (boundEbo == 0) {
                        std::cerr << "[OpenGL] No element buffer bound!\n";
                    }
                    else {
                        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
                    }

                    // Unbind for hygiene
                    glBindVertexArray(0);
                    glBindTexture(GL_TEXTURE_2D, 0);
                    glUseProgram(0);
                }
            }
        }

        // --- Update input each frame ---
        almondnamespace::input::poll_input();

        // Pipe global input into context functions
        ctx.is_key_held = [](almondnamespace::input::Key key) -> bool {
            return almondnamespace::input::is_key_held(key);
            };
        ctx.is_key_down = [](almondnamespace::input::Key key) -> bool {
            return almondnamespace::input::is_key_down(key);
            };
        ctx.is_mouse_button_held = [](almondnamespace::input::MouseButton btn) -> bool {
            return almondnamespace::input::is_mouse_button_held(btn);
            };
        ctx.is_mouse_button_down = [](almondnamespace::input::MouseButton btn) -> bool {
            return almondnamespace::input::is_mouse_button_down(btn);
            };

        queue.drain();
        SwapBuffers(ctx.hdc);

        wglMakeCurrent(nullptr, nullptr);
        return true;
    }


    inline void opengl_cleanup(std::shared_ptr<core::Context>& ctx) {
        if (ctx->hglrc) {
            wglMakeCurrent(nullptr, nullptr);
            wglDeleteContext(ctx->hglrc);
            ctx->hglrc = nullptr;
        }
        if (ctx->hdc && ctx->hwnd) {
            ReleaseDC(ctx->hwnd, ctx->hdc);
            ctx->hdc = nullptr;
        }
    }

    // main context thread
    //inline bool opengl_process(core::Context& ctx) {
    //    MSG msg{};
    //    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
    //        if (msg.message == WM_QUIT) return false;
    //        TranslateMessage(&msg);
    //        DispatchMessage(&msg);
    //    }

    //    HDC hdc = s_openglstate.hdc;
    //    HGLRC hglrc = s_openglstate.hglrc;

    //    if (!hdc || !hglrc) return false;

    //    // Make context current on THIS thread only
    //    if (!wglMakeCurrent(hdc, hglrc)) {
    //        DWORD err = GetLastError();
    //        std::cerr << "[OPENGL] Failed to make context current, GetLastError = " << err << "\n";
    //        s_openglstate.running = false;
    //        return false;
    //    }

    //    glViewport(0, 0, s_openglstate.width, s_openglstate.height);
    //    glClearColor(0.f, 0.f, 1.f, 1.f);
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    // Execute queued render commands safely
    //    {
    //        std::scoped_lock lock(s_commandMutex);
    //        while (!s_renderCommands.empty()) {
    //            auto& cmd = s_renderCommands.front();
    //            cmd();
    //            s_renderCommands.pop();
    //        }
    //    }

    //    // Swap buffers for this window
    //    SwapBuffers(hdc);

    //    // Exit condition
    //    if (!s_openglstate.running || almondnamespace::input::is_key_down(almondnamespace::input::Key::Escape)) {
    //        s_openglstate.running = false;
    //        wglMakeCurrent(nullptr, nullptr);
    //        return false;
    //    }

    //    wglMakeCurrent(nullptr, nullptr); // always unbind
    //    return true;
    
    
    //    // ─── Time & events ──────────────────────────────────────────────────────
    //    static auto lastReal = std::chrono::steady_clock::now();
    //    auto nowReal = std::chrono::steady_clock::now();
    //    s_clockSystem.advanceTime(nowReal - lastReal);
    //    lastReal = nowReal;

    //    platform::pump_events();

    //    // ─── FPS logging ────────────────────────────────────────────────────────
    //    ++s_frameCount;
    //    if (s_pollTimer.elapsed() >= 0.3) {
    //        s_pollTimer.restart();
    //    }
    //    if (s_fpsTimer.elapsed() >= 1.0) {
    //        std::cout << "FPS: " << s_frameCount << "\n";
    //        s_frameCount = 0;
    //        s_fpsTimer.restart();
    //    }

    //    // ─── Clear & viewport ───────────────────────────────────────────────────
    //    RECT r;
    //    GetClientRect(s_hwnd, &r);
    //    glViewport(0, 0, r.right - r.left, r.bottom - r.top);
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    // ─── Draw “test_sprite” ─────────────────────────────────────────────────
    //    if (auto opt = s_registry.get("test_sprite")) {
    //        auto [globalSlot, u0, v0, u1, v1, px, py] = *opt;

    //        // decode atlas index and local slot
    //        int atlasIdx = globalSlot >> 16;
    //        int local = globalSlot & 0xFFFF;

    //        // fetch the correct Atlas (heap‐allocated)
    //        Atlas& A = *s_atlases.at(atlasIdx);

    //        // compute UV region
    //        float du = u1 - u0;
    //        float dv = v1 - v0;

    //        // bind shader + texture + draw
    //        glUseProgram(s_shader);
    //        glUniform4f(
    //            glGetUniformLocation(s_shader, "uRegion"),
    //            u0, v0, du, dv
    //        );
    //        glActiveTexture(GL_TEXTURE0);
    //        glBindTexture(GL_TEXTURE_2D, A.glHandle);
    //        glBindVertexArray(s_vao);
    //        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    //    }

    //    // ─── Present ────────────────────────────────────────────────────────────
    //    SwapBuffers(s_hdc);
   // }


    //inline bool process(Context& ctx) //override
    //{
    //    opengl_process(Context & ctx);
    //    return true;
    //}

	// Context state getters/setters
    inline bool OpenGLGetIsRunning(std::shared_ptr<core::Context> ctx) { return s_openglstate.running; }
    inline bool OpenGLSetIsRunning(std::shared_ptr<core::Context> ctx) { return s_openglstate.running = false; }

} // namespace almond::opengl

#endif // ALMOND_USING_OPENGL
