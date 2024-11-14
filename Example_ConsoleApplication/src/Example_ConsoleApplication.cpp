#include <AlmondEngine.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

// Entry point does not require int main() explicitly
int width = 800;
int height = 600;
const wchar_t* title = L"Almond Core Example";
/*
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
       // std::cout << "FPS: " << fpsCounter.getFPS() << std::endl;
       // std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Adjust sleep time to avoid excessive logging
   // }
}
// Custom Scene Implementation
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
  
 
  //  void AddEntity(int entityId) override {
  //      m_ecs.AddComponent(entityId, new almond::core::Position());
  //      m_ecs.AddComponent(entityId, new almond::core::Velocity(1.0f, 1.0f));
   //     m_entities.push_back(entityId);
  //  }   
};
*/
// Entry Point Initialization Structure
struct myInitializer {
    myInitializer(int width, int height, const wchar_t* title) {

        // Create an instance of EntryPoint using the factory function from the DLL
        //almond::EntryPoint* entryPoint = almond::CreateEntryPoint(800, 600, L"Example Console App");

        // Use the EntryPoint instance to manage the application lifecycle
        //entryPoint->show();
        //entryPoint->run();

        // Clean up the EntryPoint instance
       // almond::DestroyEntryPoint(entryPoint);
/*
        NewScene scene;
        size_t threadCount = 1;
        int maxBuffer = 100;

       almond::AlmondCore* myAlmondCore = almond::CreateAlmondCore(threadCount, true, &scene, maxBuffer);
       //myAlmondCore->SetRunning(true);  // Set running state

        // Optional: Register callbacks here if needed
        RegisterAlmondCallback([&scene]() {
           // scene.load();
            });
*/
        //almond::Run(*myAlmondCore);  // Start AlmondCore's main loop
     /*
        if (almond::IsRunning(*myAlmondCore))
        {
            almond::PrintFPS(*myAlmondCore);
        }*/
        
    }
};

// Initialize Entry Point at the Global Level
myInitializer init(width, height, title);
