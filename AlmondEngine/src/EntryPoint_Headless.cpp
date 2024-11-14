//#include "Coroutine.h"
#include "EntryPoint.h"
#include "EntryPoint_Headless.h"
#include "AlmondEngine.h"
//#include "ThreadPool.h"

#include <iostream>
//#include <thread>
//#include <chrono>
//#include <optional>
/*
// Coroutine function for game logic
inline almond::Coroutine gameLogicCoroutine() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Logic Coroutine Updated..." << std::endl;
        co_yield i; // Yield game logic frame state
    }
}

// Coroutine for simulating physics updates
inline almond::Coroutine physicsCoroutine() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Physics Coroutine Updated..." << std::endl;
        co_yield i;
    }
}

// Coroutine for simulating audio updates
inline almond::Coroutine audioCoroutine() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Audio Coroutine Updated..." << std::endl;
        co_yield i;
    }
}

// Coroutine for simulating game system updates
inline almond::Coroutine gameSystemCoroutine() {
    for (int i = 0; i < 5; ++i) {
        std::cout << "Game Coroutine Updated..." << std::endl;
        co_yield i;
    }
}
*/
// FPS Class
class FPS {
private:
    unsigned int fps;
    unsigned int fpsCount;
    std::chrono::time_point<std::chrono::steady_clock> lastTime;

public:
    FPS() : fps(0), fpsCount(0), lastTime(std::chrono::steady_clock::now()) {}

    void update() {
        fpsCount++;
        auto currentTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count();

        if (duration > 1000) {
            fps = fpsCount;
            fpsCount = 0;
            lastTime = currentTime;
        }
    }

    unsigned int getFPS() const {
        return fps;
    }
};

// FPS Counter Function
void runFPSCounter(FPS& fpsCounter) {
    // while (true) {
    fpsCounter.update();
    //system("cls");
    std::cout << "FPS: " << fpsCounter.getFPS() << std::endl;
   // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Adjust sleep time to avoid excessive logging
// }
}

struct NewScene : public almond::Scene {

    void load() override {
        // Sample Update Logic
        FPS fpsCounter;
        std::vector<std::thread> threads;

        // Create multiple threads running the FPS counter
        for (size_t i = 0; i < std::thread::hardware_concurrency(); ++i) {
            threads.emplace_back(runFPSCounter, std::ref(fpsCounter));
        }

        // Join threads to the main thread
        for (auto& t : threads) {
            t.join();
        }
        // std::cout << "scene loaded\n";
    }

    /*
       void AddEntity(int entityId) override {
           m_ecs.AddComponent(entityId, new almond::core::Position());
           m_ecs.AddComponent(entityId, new almond::core::Velocity(1.0f, 1.0f));
           m_entities.push_back(entityId);
       }   */
};


int main() {
    std::cout << "Running headless application...\n";
    NewScene scene; 
    size_t threadCount = 1;
    size_t maxBuffer = 100;

    almond::AlmondCore* myAlmondCore = almond::CreateAlmondCore(threadCount, true, &scene, maxBuffer);

    // Optional: Register callbacks here if needed
    RegisterAlmondCallback([&scene]() {
         scene.load();
        });

    almond::Run(*myAlmondCore);  // Start AlmondCore's main loop
    // Initialize thread pool with available hardware concurrency minus 1 thread
   // almond::ThreadPool threadPool(std::thread::hardware_concurrency() - 1);
/*
    // Example coroutine instances for different systems
    auto gameLogic = gameLogicCoroutine();
    auto physics = physicsCoroutine();
    auto audio = audioCoroutine();
    auto gameSystem = gameSystemCoroutine();

    // Main loop for coroutine-based game logic, physics, audio, etc.
    try {
        bool gameLogicRunning = true;
        bool physicsRunning = true;
        bool audioRunning = true;
        bool gameSystemRunning = true;

        std::cout << "Main Thread..." << std::endl;

        // Main application loop
        while (gameLogicRunning || physicsRunning || audioRunning || gameSystemRunning) {
            if (gameLogicRunning) {
                gameLogicRunning = gameLogic.resume();
                std::cout << "Game logic yielded: " << gameLogic.current_value() << std::endl;
            }

            if (physicsRunning) {
                physicsRunning = physics.resume();
                std::cout << "Game physics yielded: " << physics.current_value() << std::endl;
            }

            if (audioRunning) {
                audioRunning = audio.resume();
                std::cout << "Game audio yielded: " << audio.current_value() << std::endl;
            }

            if (gameSystemRunning) {
                gameSystemRunning = gameSystem.resume();
                std::cout << "Game System yielded: " << gameSystem.current_value() << std::endl;
            }

            // Optionally, use thread pool for heavy tasks like loading resources
          threadPool.enqueue([] {
                std::cout << "Heavy asset loading on thread." << std::endl;
                //std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulating work
                });
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;*/
}