#include "aleSDL3Context.h"

#include "aleSDLSnake.h"
#include "aleSDLGameOfLife.h"
#include "aleSDLSandSim.h"

#include "aleEngine.h"

#ifdef ALMOND_USING_SDL
#include <algorithm> // for std::find_if

namespace almond {
    SDL_Window* window;
    SDL_Renderer* renderer;
   // Renderer renderer_opengl;
    SDL_GLContext glContext;

    void initSDL3() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            throw std::runtime_error("Failed to initialize SDL: " + std::string(SDL_GetError()));
        }

        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

        window = SDL_CreateWindow("Almond - SDL Example", 800, 600, SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
        if (!window) {
            SDL_Quit();
            throw std::runtime_error("Failed to create SDL window: " + std::string(SDL_GetError()));
        }

        renderer = SDL_CreateRenderer(window, NULL);
        if (!renderer) {
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Failed to create SDL renderer: " + std::string(SDL_GetError()));
        }

        glContext = SDL_GL_CreateContext(window);
        if (!glContext) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            throw std::runtime_error("Failed to create SDL OpenGL context: " + std::string(SDL_GetError()));
        }
    }

    bool processSDL3() {
        srand(static_cast<unsigned int>(time(0)));

        CellularAutomaton automaton(80, 60);
        SandSimulation sandSim(160, 120);
        SnakeGame snake(40, 30);

        automaton.randomize();
        sandSim.randomize();

        bool running = true;
        bool isGameOfLife = true;
        bool isSnakeGame = false;

        float mouseX = 0, mouseY = 0;
        bool isMousePressed = false;

        const Uint32 frameDelay = 200;
        Uint32 lastTime = SDL_GetTicks();
        float deltaTime = 0.0f;

        // Load texture using SDL
        SDL_Surface* surface = SDL_LoadBMP("../../images/almondshell.bmp");
        if (!surface) {
            SDL_Log("Failed to load image: %s", SDL_GetError());
            return false;
        }
        SDL_Texture* sdlTexture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);
        if (!sdlTexture) {
            SDL_Log("Failed to create texture: %s", SDL_GetError());
            return false;
        }

        // Example of using OpenGL textures, if needed
      //  std::unique_ptr<Texture> texture(Texture::Create("../../../images/BMP.bmp", Texture::Format::RGBA8, true));

            SDL_Event e;
        while (running) {
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_EVENT_QUIT) {
                    running = false;
                }
                
                if (e.type == SDL_EVENT_KEY_DOWN) {
                    switch (e.key.key) {
                    case SDLK_UP: snake.updateDirection(SnakeGame::Direction::Up); break;
                    case SDLK_DOWN: snake.updateDirection(SnakeGame::Direction::Down); break;
                    case SDLK_LEFT: snake.updateDirection(SnakeGame::Direction::Left); break;
                    case SDLK_RIGHT: snake.updateDirection(SnakeGame::Direction::Right); break;
                    case SDLK_ESCAPE: running = false; break;
                    }

                    if (e.key.key == SDLK_SPACE) {
                        isGameOfLife = !isGameOfLife;
                        isSnakeGame = false;
                    }
                    else if (e.key.key == SDLK_TAB) {
                        isSnakeGame = !isSnakeGame;
                        isGameOfLife = false;
                    }
                }

                if (e.type == SDL_EVENT_MOUSE_MOTION) {
                    SDL_GetMouseState(&mouseX, &mouseY);
                }
                if (e.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        isMousePressed = true;
                    }
                }
                if (e.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        isMousePressed = false;
                    }
                }
            }

            Uint32 currentTime = SDL_GetTicks();
            deltaTime = (currentTime - lastTime) / 1000.0f;
            lastTime = currentTime;

            if (isMousePressed && !isGameOfLife && !isSnakeGame) {
                int gridX = static_cast<int>(mouseX / 5);
                int gridY = static_cast<int>(mouseY / 5);
                sandSim.addSandAt(gridX, gridY);
            }

            if (isGameOfLife) {
                automaton.update();
            }
            else if (isSnakeGame) {
                snake.update(deltaTime);
               // texture->Bind(0);
               // renderer_opengl.DrawQuad();
               // texture->Unbind();
            }
            else {
                sandSim.update();
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);

            // Render the SDL texture
            SDL_FRect destRect = { 10, 10, 800, 600 }; // Example size and position
            SDL_RenderTexture(renderer, sdlTexture, NULL, &destRect);

            if (isGameOfLife) {
                automaton.render(renderer, 10);
            }
            else if (isSnakeGame) {
                snake.render(renderer, 20);
            }
            else {
                sandSim.render(renderer, 5);
            }

            SDL_RenderPresent(renderer);
        }

        // Cleanup SDL texture
        SDL_DestroyTexture(sdlTexture);

        return false;
    }

    void cleanupSDL3() {
        if (renderer) {
            SDL_DestroyRenderer(renderer);
            renderer = nullptr;
        }
        if (window) {
            SDL_DestroyWindow(window);
            window = nullptr;
        }
        SDL_Quit();
    }

    inline ContextFuncs createContextFuncs(const std::string& type) {
        if (type == "SDL3") {
            return { initSDL3, processSDL3, cleanupSDL3 };
        }
        throw std::invalid_argument("Unsupported context type: " + type);
    }
}
#endif
