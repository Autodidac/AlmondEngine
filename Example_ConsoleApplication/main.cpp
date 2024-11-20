#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_render.h>

#include <cmath>
#include <iostream>
#include <AlmondEngine.h>

struct AppContext {
    SDL_Window* window;
    SDL_Renderer* renderer;
};

void SDL_Fail(const char* msg) {
    std::cerr << "Error: " << msg << std::endl;
    std::cerr << "SDL_Error: " << SDL_GetError() << std::endl;  // Print SDL-specific error message
    SDL_Quit();
    exit(1);
}

bool SDL_AppInit(AppContext* app) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Fail("SDL_Init failed");
        return false;
    }

    // Proper SDL_CreateWindow call
    app->window = SDL_CreateWindow("Window", 800, 600, SDL_WINDOW_RESIZABLE);
    if (!app->window) {
        SDL_Fail("SDL_CreateWindow failed");
        return false;
    }

    // Creating the renderer with SDL_RENDERER_SOFTWARE (software renderer)
    app->renderer = SDL_CreateRenderer(app->window, "sdfsd"); // Software renderer fallback
    if (!app->renderer) {
        SDL_Fail("SDL_CreateRenderer failed");
        return false;
    }

    // Show the window to ensure it's visible
    SDL_ShowWindow(app->window);
    return true;
}

void SDL_AppEvent(AppContext* app, SDL_Event* event, bool& quit) {
    if (event->type == SDL_EVENT_QUIT) {
        quit = true;
    }
}

void SDL_AppIterate(AppContext* app) {
    auto time = SDL_GetTicks() / 1000.f;
    auto red = (std::sin(time) + 1) / 2.0 * 255;
    auto green = (std::sin(time / 2) + 1) / 2.0 * 255;
    auto blue = (std::sin(time) * 2 + 1) / 2.0 * 255;

    SDL_SetRenderDrawColor(app->renderer, static_cast<Uint8>(red), static_cast<Uint8>(green), static_cast<Uint8>(blue), SDL_ALPHA_OPAQUE);
    SDL_RenderClear(app->renderer);
    SDL_RenderPresent(app->renderer);
}

void SDL_AppQuit(AppContext* app) {
    if (app) {
        SDL_DestroyRenderer(app->renderer);
        SDL_DestroyWindow(app->window);
        SDL_Quit();
    }
    SDL_Log("Application quit successfully!");
}

// SDL3 provides the entry point (SDL_main)
int SDL_main(int argc, char* argv[]) {
    AppContext app;
    bool quit = false;

    // Initialize SDL and application
    if (!SDL_AppInit(&app)) {
        return 1;  // If initialization fails, return error code
    }

    SDL_Event event;

    //std::cout << GetVersionString() << std::endl;

    // Main application loop
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            SDL_AppEvent(&app, &event, quit);
        }

        SDL_AppIterate(&app);
        SDL_Delay(16);  // Approx 60 FPS
    }

    // Cleanup when done
    SDL_AppQuit(&app);
    return 0;  // SDL3 will handle proper exit
}
