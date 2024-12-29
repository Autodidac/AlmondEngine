#include <almondshell>
#include "alsEngine.h"
//#include "ContextFactory.h"

#include <chrono>
#include <iostream>
#include <string>
#include <memory>
#include <stdexcept>
#include <thread>
#include <vector>

// Entry point does not require int main() explicitly
int width = 800;
int height = 600;
const std::wstring& title = L"Almond Shell Example";

/**/
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
void runFPSCount(FPS& fpsCounter) {
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
            threads.emplace_back(runFPSCount, std::ref(fpsCounter));
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

void MyCallback() {
    std::cout << "Callback triggered!" << std::endl;
}
// Entry Point Initialization Structure
struct myInitializer {

    myInitializer(const std::wstring& title, int width, int height) {
 /*     try {
            // Initialize window and rendering contexts
            almond::WindowContext windowContext;
            almond::RenderContext renderContext;

            std::wcout << L"Creating window with title: 'Agnostic Engine', width: " << width << L", height: " << height << std::endl;
            auto windowHandle = windowContext.createWindow(L"Agnostic Engine", width, height);
            if (!windowHandle) {
                std::cerr << "Failed to create window. Handle is null." << std::endl;
            }
            else {
                std::wcout << L"Window created successfully." << std::endl;
            }

            std::cout << "Initializing render context..." << std::endl;
            // Initialize the rendering context
            renderContext.initialize;// (windowHandle);
            std::cout << "Render context initialized successfully." << std::endl;


            // Main loop
           while(true){// while (!windowContext.shouldClose()) {
                windowContext.pollEvents();
                renderContext.clearColor(0.1f, 0.2f, 0.3f, 1.0f);
                renderContext.clear();
                renderContext.swapBuffers;// (windowHandle);
            }
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Runtime error: " << e.what() << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error initializing AlmondShell: " << e.what() << std::endl;
        }
        catch (...) {
            std::cerr << "An unknown error occurred during initialization." << std::endl;
        }
    }

  */
/*
        // Choose windowing system (GLFW or SFML)
        almond::WindowContext windowContext = almond::WindowContext();
        // Choose rendering system (OpenGL or DirectX)
        almond::RenderContext renderContext = almond::RenderContext();

        // Create window and initialize rendering
        void* windowHandle = windowContext.createWindow("Agnostic Engine", 800, 600);
        renderContext.initialize(windowHandle);

        while (!windowContext.shouldClose()) {
            windowContext.pollEvents();
            renderContext.clearColor(0.1f, 0.2f, 0.3f, 1.0f);
            renderContext.clear();
            renderContext.swapBuffers(windowHandle);
        }


*/


        //std::cout << "AlmondShell v" << almond::AlmondShell::GetEngineVersion() << std::endl;

        
      /*  almond::RegisterAlmondCallback(MyCallback);*/
        almond::AlmondShell* engine = almond::CreateAlmondShell(4, true, nullptr, 10);
        almond::Run(*engine);
        //delete engine;


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

       almond::AlmondShell* myAlmondShell = almond::CreateAlmondShell(threadCount, true, &scene, maxBuffer);
*/       //myAlmondShell->SetRunning(true);  // Set running state

        // Optional: Register callbacks here if needed
       // RegisterAlmondCallback([&scene]() {
           // scene.load();
       //     });
       /*
        if (almond::IsRunning(*myAlmondShell))
        {
            std::cout << "printing fps\n";
            almond::PrintFPS(*myAlmondShell);
        }

        almond::Run(*myAlmondShell);  // Start AlmondShell's main loop
 */    

        
    }
};

// Initialize Entry Point at the Global Level
myInitializer init( title, width, height);
