
#include "aleGLFWContext.h"
#include "aleEngineConfig.h"
//#include "alsEngine.h"

//#include <stb/stb_image.h>
/*
#include"Texture.h"
#include"Shaderclass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
*/
#ifdef ALMOND_USING_GLFW

#ifdef ALMOND_USING_OPENGLTEXTURE
    #include "aleTexture.h"
    #include "aleOpenGLTexture.h" // OpenGL texture manager
    #include "aleOpenGLRenderer.h"
    #include "aleOpenGLTextureAtlas.h"
#endif

#include "aleGLFWSandSim.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include <vector>

//------- Ignore this ----------
#include<filesystem>

namespace fs = std::filesystem;
//------------------------------

namespace almond {

    GLFWwindow* glfwWindow = nullptr;

    int width = 800;
    int height = 600;
    SandSimulation sandSim(width, height);
    int posx;
    int posy;
    bool isButtonHeld = false;

    void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            std::cout << "Key Pressed: " << key << "\n";
        }
        else if (action == GLFW_RELEASE) {
            std::cout << "Key Released: " << key << "\n";
        }

        if (action == GLFW_KEY_SPACE) {
            //isGameOfLife = !isGameOfLife;
            //isSnakeGame = false;
        }
    }

    void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        if (action == GLFW_PRESS) isButtonHeld = true;
        if (action == GLFW_RELEASE) isButtonHeld = false;
    }

    void ProcessInput() {
        if (isButtonHeld) {
            // Sand brush logic here
        }
    }

    void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {

        //GLFW_CURSOR
        if (isButtonHeld)
        {
            posx = static_cast<int>(xpos);
            posy = static_cast<int>(ypos);
            std::cout << "Cursor Position: (" << xpos << ", " << ypos << ")\n";
        }

    }
    void FramebufferSizeCallback(GLFWwindow* window, int width, int height) {
        std::cout << "Window resized: " << width << "x" << height << "\n";
        glViewport(0, 0, width, height); // Update OpenGL viewport
    }

    // Create and initialize the sand simulation with a grid of 100x100

    void initGLFW() {
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        glfwWindow = glfwCreateWindow(width, height, "GLFW with OpenGL", nullptr, nullptr);
        if (!glfwWindow) {
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }

        glfwMakeContextCurrent(glfwWindow);
        // important step
        //gladLoadGL();
        gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        // glfwSwapInterval(1); // Enable vsync
       // glViewport(0, 0, width, height); // Update OpenGL viewport

        //        const char* version = (const char*)glGetString(GL_VERSION);
        //        std::cout << "OpenGL Version: " << version << std::endl;

        glfwSetKeyCallback(glfwWindow, KeyCallback);
        glfwSetMouseButtonCallback(glfwWindow, MouseButtonCallback);
        glfwSetCursorPosCallback(glfwWindow, CursorPositionCallback);
        glfwSetFramebufferSizeCallback(glfwWindow, FramebufferSizeCallback);

        std::cout << "GLFW and OpenGL initialized successfully\n";
    }

    bool processGLFW() {

        if (glfwWindowShouldClose(glfwWindow)) {
            return false;
        }

        auto lastTime = std::chrono::steady_clock::now();
        int frameCount = 0;

        //grab the grid
        const auto& grid = sandSim.getGrid();

        // Setup Texture Atlas
        std::filesystem::path texturePath = std::filesystem::current_path().parent_path().parent_path() / "images" / "bmp3.bmp";
        OpenGLTextureAtlas atlas(2048, 2048);
        Renderer renderer(atlas); // Create a renderer object for the texture atlas

        renderer.AddTexture(texturePath.string().c_str(), 0, 0);
        renderer.AddTexture(texturePath.string().c_str(), 256, 0);
        renderer.AddTexture(texturePath.string().c_str(), 0, 256);

        std::vector<float> quadVertices = {
            -0.5f, -0.5f, 0.0f, 0.0f,  // Bottom-left
             0.5f, -0.5f, 1.0f, 0.0f,  // Bottom-right
             0.5f,  0.5f, 1.0f, 1.0f,  // Top-right
            -0.5f,  0.5f, 0.0f, 1.0f   // Top-left
        };

        std::vector<unsigned int> quadIndices = {
            0, 1, 2, 2, 3, 0
        };

        renderer.AddMesh("sand_quad", quadVertices, quadIndices); // Create a new mesh object in the renderer for our quad

        // Enable wireframe mode
       // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        while (!glfwWindowShouldClose(glfwWindow)) {
            auto currentTime = std::chrono::steady_clock::now();
            frameCount++;

            // Calculate FPS
            std::chrono::duration<float> elapsedTime = currentTime - lastTime;
            if (elapsedTime.count() >= 1.0f) {
                std::cout << "FPS: " << frameCount << "\n";
                frameCount = 0;
                lastTime = currentTime;
            }

            // Clear screen
            glClearColor(0.2f, 0.2f, 0.2f, 0.1f);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // Process input
            ProcessInput();

            // Update simulation
            sandSim.update();

            // Initialize and fill activeParticles with the positions of active particles
            std::vector<std::pair<int, int>> activeParticles;
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    if (grid[y * width + x] > 0) {
                        activeParticles.emplace_back(x, y);
                    }
                }
            }

            // Render particles
            std::vector<std::shared_ptr<almond::RenderCommand>> commands;
            auto mesh = renderer.GetMesh("sand_quad");
            if (mesh) {
                for (const auto& particle : activeParticles) {
                    float xpos = (float)particle.first / width * 2.0f - 1.0f; // Normalize to [-1, 1] range
                    float ypos = (float)particle.second / height * 2.0f - 1.0f; // Normalize to [-1, 1] range
                    commands.push_back(std::make_unique<almond::DrawCommand>(
                        mesh, 0, almond::RenderMode::TextureAtlas, xpos, ypos));
                }
                renderer.DrawBatch(commands);
            }

            if (isButtonHeld)
            {
                // Create a brush, which adds sand in a radius
                for (int y = -4; y <= 4; ++y)
                {
                    for (int x = -4; x <= 4; ++x)
                    {
                        if (x * x + y * y < 16) // Check if it is in the radius
                        {
                            int gridX = (posx / 5) + x;
                            int gridY = (posy / 5) + y;

                            if (gridX < 0) gridX = 0;
                            if (gridY < 0) gridY = 0;
                            if (gridX >= width) gridX = width - 1;
                            if (gridY >= height) gridY = height - 1;

                            sandSim.addSandAt(gridX, gridY);

                        }
                    }
                }
            }

            // Swap buffers and poll events
            glfwSwapBuffers(glfwWindow);
            glfwPollEvents();
        }

        return true;
    }

    void cleanupGLFW() {
        if (glfwWindow) {
            glfwDestroyWindow(glfwWindow);
            glfwWindow = nullptr;
        }

        //sandSim.cleanup(); // Cleanup sand simulation resources
        glfwTerminate();
    }
}

#endif
