// src/almond/platform/wasm_entry_point.cpp
#pragma once

//#include "pch.h"

#ifdef __EMSCRIPTEN__

#include "EntryPoint_Crossplatform.h"
#include <emscripten.h>
#include <html5.h>

namespace almond {

    class WasmEntryPoint : public EntryPointBase {
    public:
        void createEntryPoint(int width, int height, const char* title) override {
            // Initialize WebGL or HTML5 canvas entry point here
            emscripten_set_canvas_element_size("#canvas", width, height);
        }

        void show() override {
            emscripten_set_main_loop(mainLoop, 0, 1);
        }

        void pollEvents() override {
            // No need for manual polling, handled by the main loop
        }

    private:
        static void mainLoop() {
            // Render frame here
        }
    };

} // namespace almond

#endif // __EMSCRIPTEN__
