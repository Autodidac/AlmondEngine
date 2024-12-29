#include "aleEngine.h"

#include "aleContextFunctions.h"
#include "aleLogger.h"
#include "aleUIbutton.h"
#include "aleUImanager.h"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <thread>

namespace almond {
    //class RenderingSystem;
    // Mutex for callback thread safety
    std::shared_mutex m_callbackMutex;
    std::function<void()> m_updateCallback;

    // Register a callback for updates
    void RegisterAlmondCallback(const std::function<void()> callback) {
        std::unique_lock lock(m_callbackMutex);
        m_updateCallback = callback;
    }

    AlmondShell::AlmondShell(almond::RobustTime& m_timeSystem) : m_jobSystem(4), m_running(true), m_scene(nullptr), m_maxBufferSize(100), m_targetTime(0.0f),
        m_targetFPS(120), m_frameLimitingEnabled(false), m_saveIntervalMinutes(1), m_frameCount(0), m_fps(0.0f),
        m_pluginManager("./plugins.log", m_timeSystem), m_timeSystem(m_timeSystem)
    {
    }

    // Constructor and Destructor
    AlmondShell::AlmondShell(size_t numThreads, bool running, almond::Scene* scene, size_t maxBufferSize, almond::RobustTime& m_timeSystem)
            : m_jobSystem(numThreads), m_running(running), m_scene(scene ? std::make_unique<almond::Scene>(std::move(*scene)) : nullptr),
        m_maxBufferSize(maxBufferSize), m_targetTime(0.0f), m_targetFPS(120), m_frameLimitingEnabled(false), m_saveIntervalMinutes(1),
        m_frameCount(0), m_fps(0.0f), m_pluginManager("./plugins.log", m_timeSystem), m_timeSystem(m_timeSystem) {
        // Create renderer instance      
       // InitializeRenderer(L"Example Almond Window Title", 5.0f, 5.0f, 800, 600, 0xFFFFFFFF, nullptr);
    }

    // Destructor
    AlmondShell::~AlmondShell() = default;

    // Snapshot management
    void AlmondShell::CaptureSnapshot() {
        if (m_recentStates.size() >= m_maxBufferSize) {
            m_recentStates.pop_front();
        }
        auto currentSceneState = m_scene->clone();
        float timestamp = std::chrono::duration<float>(
            std::chrono::steady_clock::now().time_since_epoch()).count();
        m_recentStates.emplace_back(timestamp, std::move(currentSceneState));
    }

    void AlmondShell::SaveBinaryState() {
        if (m_recentStates.empty()) return;

        try {
            std::ofstream ofs("savegame.dat", std::ios::binary | std::ios::app);
            if (!ofs.is_open()) {
                throw std::ios_base::failure("Failed to open save file for writing.");
            }

            const SceneSnapshot& snapshot = m_recentStates.back();
            ofs.write(reinterpret_cast<const char*>(&snapshot), sizeof(SceneSnapshot));
            if (!ofs) {
                throw std::ios_base::failure("Error occurred while writing to save file.");
            }
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << "SaveBinaryState Error: " << e.what() << std::endl;
        }
    }

    void AlmondShell::SetPlaybackTargetTime(float targetTime) {
        m_targetTime = targetTime;
    }

    void AlmondShell::UpdatePlayback() {
        if (!m_recentStates.empty() && m_recentStates.front().timeStamp <= m_targetTime) {
            for (const auto& snapshot : m_recentStates) {
                if (snapshot.timeStamp >= m_targetTime) {
                    m_scene = std::make_unique<Scene>(std::move(*snapshot.currentState));
                    break;
                }
            }
        }
        else {
            LoadStateAtTime(m_targetTime);
        }
    }

    void AlmondShell::LoadStateAtTime(float timeStamp) {
        try {
            std::ifstream ifs("savegame.dat", std::ios::binary);
            if (!ifs) throw std::ios_base::failure("Failed to open save file.");

            SceneSnapshot snapshot;
            while (ifs.read(reinterpret_cast<char*>(&snapshot), sizeof(SceneSnapshot))) {
                if (snapshot.timeStamp >= timeStamp) {
                    m_scene = std::make_unique<Scene>(std::move(*snapshot.currentState));
                    break;
                }
            }
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    // Update FPS and frame limiting
    void AlmondShell::UpdateFPS() {
        using Clock = std::chrono::steady_clock;

        m_frameCount++;
        auto currentTime = Clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - m_lastSecond).count();

        if (elapsedTime >= 1000) { // 1 second in milliseconds
            m_fps = static_cast<float>(m_frameCount) * 1000.0f / elapsedTime; // Frames per second
            m_frameCount = 0;
            m_lastSecond = currentTime;

            std::cout << "Current FPS: " << std::fixed << std::setprecision(2) << m_fps << "\n";
        }
    }

    void AlmondShell::LimitFrameRate(std::chrono::steady_clock::time_point& lastFrame) {
        auto frameDuration = std::chrono::milliseconds(1000 / m_targetFPS);
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastFrame = now - lastFrame;

        if (timeSinceLastFrame < frameDuration) {
            std::this_thread::sleep_for(frameDuration - timeSinceLastFrame);
        }
        lastFrame = std::chrono::steady_clock::now();
    }
     
    void AlmondShell::InitializeRenderer(const char* title, float x, float y, float width, float height, unsigned int color, void* texture) {
        try {
            std::cout << "Select a library:\n";
            std::cout << "1. GLFW\n";
            std::cout << "2. SDL3\n";
            std::cout << "Enter the number corresponding to your choice: ";

            int selection;
            std::cin >> selection;
            std::cin.ignore(); // Clear newline character from input buffer

            std::string selectedLibrary;
            switch (selection) {
            case 1:
                selectedLibrary = "GLFW";
                break;
            case 2:
                selectedLibrary = "SDL3";
                break;
            default:
                std::cerr << "Error: Invalid selection.\n";
                return;
            }

            auto contextFuncs = createContextFuncs(selectedLibrary);

            if (std::get<0>(contextFuncs)) {
                std::cout << "Initializing context...\n";
                std::get<0>(contextFuncs)(); // Initialize context

                bool running = true;
                while (running) {

                    std::cout << "Processing events...\n";
                    running = std::get<1>(contextFuncs)(); // Process events
                }

                std::cout << "Cleaning up...\n";
                std::get<2>(contextFuncs)(); // Cleanup
                //return;
            }
            else {
                std::cerr << "Error: Unsupported library selected.\n";
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }     
    }

    /*
    // Ensure m_renderer is created only once
    if (!m_renderer) {
        std::cout << "Creating a new RenderingSystem instance.\n"; // Logging
        // Pass componentManager and jobSystem when constructing RenderingSystem
        m_renderer = std::make_unique<RenderingSystem>(title, x, y, width, height, color, texture);
    }

    // Initialize the context renderer, but only if it hasn't been set up yet
    if (!m_renderer->IsInitialized()) {
        try {
            std::cout << "Initializing context renderer with title: " << title << ", width: " << width << ", height: " << height << "\n"; // Logging
            // m_renderer->CreateContextRenderer<RenderContext>(title, x, y, width, height, color, texture);
           
            std::cout << "Renderer initialized successfully.\n"; // Logging
        }
        catch (const std::exception& e) {
            std::cerr << "Error initializing the renderer: " << e.what() << '\n'; // Error handling
        }
    }
    else {
        std::cout << "Renderer is already initialized.\n"; // Logging
    }
    */


    void AlmondShell::RenderFrame() {
        //if (m_renderer) {
            try {
                std::cout << "Rendering frame...\n";

                // Render all contexts in the system
                //m_renderer->clear();
               // m_renderer->swapBuffers();
                std::cout << "Frame rendered successfully.\n";
            } catch (const std::exception& e) {
                std::cerr << "Error during frame rendering: " << e.what() << '\n';
            }
       // } else {
         //   std::cerr << "Renderer is not initialized or not available. Unable to render frame.\n";
       // }
    }
    /*
    // Win32-specific functionality
    void AlmondShell::RunWin32Desktop(MSG msg, HACCEL hAccelTable) {
        auto now = std::chrono::steady_clock::now();
        auto lastFrame = now;
        auto lastSave = now;

        while (m_running) {
            auto currentTime = std::chrono::steady_clock::now();
            UpdateFPS();

            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
                if (msg.message == WM_QUIT) {
                    m_running = false;
                }
            }
            else {
                std::shared_lock lock(m_callbackMutex);
                if (m_updateCallback) m_updateCallback();

                if (std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastSave).count() >= m_saveIntervalMinutes) {
                    Serialize("savegame.dat", m_events);
                    lastSave = currentTime;
                    std::cout << "Game auto-saved." << std::endl;
                }

                if (m_frameLimitingEnabled) {
                    LimitFrameRate(lastFrame);
                }
            }
        }
    }*/

    // Main run loop
    void AlmondShell::Run() {
        auto now = std::chrono::steady_clock::now();
        auto lastFrame = now;
        auto lastSave = now;
/*
        EventSystem eventSystem;
        UIManager uiManager;

        UIButton* button = new UIButton(50, 50, 200, 50, "Click Me!");
        button->SetOnClick([]() { std::cout << "Button clicked!\n"; });
        uiManager.AddButton(button);

        auto button = std::make_unique<UIButton>(50, 50, 200, 50, "Click Me!");
        button->SetOnClick([]() { std::cout << "Button clicked!\n"; });
        uiManager.AddButton(std::move(button));
*/
        //load plugins
        std::cout << "Loading any available plugins...\n";
        //almond::RobustTime* timeSystem;
        almond::plugin::PluginManager manager("./plugins.log", m_timeSystem);

        const std::filesystem::path pluginDirectory = ".\\mods";
        if (std::filesystem::exists(pluginDirectory) && std::filesystem::is_directory(pluginDirectory)) {
            for (const auto& entry : std::filesystem::directory_iterator(pluginDirectory)) {
                if (entry.path().extension() == ".dll" || entry.path().extension() == ".so") {
                    manager.LoadPlugin(entry.path());
                }
            }
        }

        // Initialize the renderer
        InitializeRenderer("Example Almond Window Title", 5.0f, 5.0f, 800, 600, 0xFFFFFFFF, nullptr);

        // headless main loop
        while (m_running) {
            auto currentTime = std::chrono::steady_clock::now();

            // Update events
          //  uiManager.Update(eventSystem); // this should be inside the driver context code

           // RenderFrame(); // Render the frame

           //std::cout << "printing fps\n";
           UpdateFPS();
     

            // run the callbacks
            std::shared_lock lock(m_callbackMutex);
            if (m_updateCallback) m_updateCallback();

            // Perform a sequential scene buffer autosave
            if (std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastSave).count() >= m_saveIntervalMinutes) {
                Serialize("savegame.dat", m_events);
                lastSave = currentTime;
                std::cout << "Game auto-saved." << std::endl;
            }

            // Immediately Capture Current Scene Data Snapshot - ordering may need to be changed
            CaptureSnapshot(); // record time

            // rewind time
            if (m_targetTime > 0.0f) { UpdatePlayback(); }

            if (m_frameLimitingEnabled) { LimitFrameRate(lastFrame); }
        }
    }

    // Serialize and deserialize
    void AlmondShell::Serialize(const std::string& filename, const std::vector<Event>& events) {
        try {
            std::ofstream ofs(filename, std::ios::binary);
            if (!ofs) throw std::ios_base::failure("Failed to open file for serialization");

            size_t size = events.size();
            ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
            ofs.write(reinterpret_cast<const char*>(events.data()), sizeof(Event) * size);
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void AlmondShell::Deserialize(const std::string& filename, std::vector<Event>& events) {
        try {
            std::ifstream ifs(filename, std::ios::binary);
            if (!ifs) throw std::ios_base::failure("Failed to open file for deserialization");

            size_t size;
            ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
            events.resize(size);
            ifs.read(reinterpret_cast<char*>(events.data()), sizeof(Event) * size);
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void AlmondShell::PrintOutFPS() { std::cout << "Average FPS: " << std::fixed << std::setprecision(2) << m_fps << "\n"; }
    bool AlmondShell::IsItRunning() const { return m_running; }
    void AlmondShell::SetRunning(bool running) { m_running = running; }
    void AlmondShell::PrintMessage(const std::string& text) { std::cout << text << std::endl; }

    void AlmondShell::SetFrameRate(unsigned int targetFPS) {
        m_targetFPS = targetFPS;
        m_frameLimitingEnabled = (m_targetFPS > 0);
    }

    // External entry points
    extern "C" AlmondShell* CreateAlmondShell(size_t numThreads, bool running, Scene* scene, size_t maxBufferSize, almond::RobustTime& m_timeSystem)
    {
        return new AlmondShell(numThreads, running, scene, maxBufferSize,m_timeSystem);
    }

    void Run(AlmondShell& core) { core.Run(); }
    extern "C" bool IsRunning(AlmondShell& core) { return core.IsItRunning(); }
    extern "C" void PrintFPS(AlmondShell& core) { core.PrintOutFPS(); }
    /*
    void PrintMessage(AlmondShell& core, const std::string& text)
    {
        core.PrintToConsole(text);
    }*/

} // namespace almond
// in alsEntryPoint.cpp or a relevant .cpp file

