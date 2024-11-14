#include "AlmondEngine.h"

#include <iostream>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <shared_mutex>

// Mutex for callback thread safety
std::shared_mutex callbackMutex;
std::function<void()> m_updateCallback;

void RegisterAlmondCallback(const std::function<void()> callback) {
    std::unique_lock lock(callbackMutex);
    m_updateCallback = callback;
}

namespace almond {

    AlmondCore::AlmondCore(size_t numThreads, bool running, Scene* scene, size_t maxBufferSize)
        : m_jobSystem(numThreads), m_running(running), m_scene(scene),
        m_frameCount(0), m_lastSecond(std::chrono::steady_clock::now()), m_maxBufferSize(maxBufferSize) {}

    void AlmondCore::CaptureSnapshot() {
        if (m_recentStates.size() >= m_maxBufferSize) {
            m_recentStates.pop_front();
        }

        auto currentSceneState = m_scene->clone();
        float timestamp = std::chrono::duration<float>(
            std::chrono::steady_clock::now().time_since_epoch()).count();

        m_recentStates.emplace_back(timestamp, std::move(currentSceneState));
    }

    void AlmondCore::SaveBinaryState() {
        if (!m_recentStates.empty()) {
            const SceneSnapshot& snapshot = m_recentStates.back();

            try {
                std::ofstream ofs("savegame.dat", std::ios::binary | std::ios::app);
                if (!ofs) throw std::ios_base::failure("Failed to open save file.");

                ofs.write(reinterpret_cast<const char*>(&snapshot), sizeof(SceneSnapshot));
                ofs.close();
            }
            catch (const std::ios_base::failure& e) {
                std::cerr << e.what() << std::endl;
            }
        }
    }

    void AlmondCore::SetPlaybackTargetTime(float targetTime) {
        m_targetTime = targetTime;
    }

    void AlmondCore::UpdatePlayback() {
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

    void AlmondCore::LoadStateAtTime(float timeStamp) {
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

    void AlmondCore::RunWin32Desktop(MSG msg, HACCEL hAccelTable) {
        auto lastFrame = std::chrono::steady_clock::now();
        auto lastSave = std::chrono::steady_clock::now();

        while (m_running) {
            auto currentTime = std::chrono::steady_clock::now();
            UpdateFPS();

            if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
                if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }

                if (msg.message == WM_QUIT) {
                    SetRunning(false);
                }
            }
            else {
                // Run the update callback in a thread-safe way
                {
                    std::shared_lock lock(callbackMutex);
                    if (m_updateCallback) m_updateCallback();
                }

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
    }

    void AlmondCore::Run() {
        auto lastFrame = std::chrono::steady_clock::now();
        auto lastSave = std::chrono::steady_clock::now();

        while (m_running) {
            auto currentTime = std::chrono::steady_clock::now();

            {
                std::shared_lock lock(callbackMutex);
                if (m_updateCallback) m_updateCallback();
            }

            if (std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastSave).count() >= m_saveIntervalMinutes) {
                Serialize("savegame.dat", m_events);
                lastSave = currentTime;
                std::cout << "Game auto-saved." << std::endl;
            }

            CaptureSnapshot();

            if (m_targetTime > 0.0f) {
                UpdatePlayback();
            }

            if (m_frameLimitingEnabled) {
                LimitFrameRate(lastFrame);
            }
        }
    }

    void AlmondCore::LimitFrameRate(std::chrono::steady_clock::time_point& lastFrame) {
        auto frameDuration = std::chrono::milliseconds(1000 / m_targetFPS);
        auto now = std::chrono::steady_clock::now();
        auto timeSinceLastFrame = now - lastFrame;

        if (timeSinceLastFrame < frameDuration) {
            std::this_thread::sleep_for(frameDuration - timeSinceLastFrame);
        }

        lastFrame = std::chrono::steady_clock::now();
    }

    void AlmondCore::UpdateFPS() {
        m_frameCount++;
        auto currentTime = std::chrono::steady_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - m_lastSecond).count();

        if (elapsedTime >= 1) {
            m_fps = static_cast<float>(m_frameCount) / elapsedTime;
            m_frameCount = 0;
            m_lastSecond = currentTime;

            std::cout << "Current FPS: " << std::fixed << std::setprecision(2) << m_fps << std::endl;
        }
    }

    void AlmondCore::Serialize(const std::string& filename, const std::vector<Event>& events) {
        try {
            std::ofstream ofs(filename, std::ios::binary);
            if (!ofs) throw std::ios_base::failure("Failed to open file for serialization");

            size_t size = events.size();
            ofs.write(reinterpret_cast<const char*>(&size), sizeof(size));
            for (const auto& event : events) {
                ofs.write(reinterpret_cast<const char*>(&event), sizeof(Event));
            }
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void AlmondCore::Deserialize(const std::string& filename, std::vector<Event>& events) {
        try {
            std::ifstream ifs(filename, std::ios::binary);
            if (!ifs) throw std::ios_base::failure("Failed to open file for deserialization");

            size_t size;
            ifs.read(reinterpret_cast<char*>(&size), sizeof(size));
            events.resize(size);

            for (auto& event : events) {
                ifs.read(reinterpret_cast<char*>(&event), sizeof(Event));
            }
        }
        catch (const std::ios_base::failure& e) {
            std::cerr << e.what() << std::endl;
        }
    }

    void AlmondCore::PrintOutFPS() {
        std::cout << "Average FPS: " << std::fixed << std::setprecision(2) << m_fps << std::endl;
    }

    bool AlmondCore::IsItRunning() const {
        return m_running;
    }

    void AlmondCore::SetRunning(bool running) {
        m_running = running;
    }

    void AlmondCore::SetFrameRate(unsigned int targetFPS) {
        m_targetFPS = targetFPS;
        m_frameLimitingEnabled = (m_targetFPS > 0);
    }

    // External entry points
    AlmondCore* CreateAlmondCore(size_t numThreads, bool running, Scene* scene, size_t maxBufferSize) {
        return new AlmondCore(numThreads, running, scene, maxBufferSize);
    }

    void Run(AlmondCore& core) {
        core.Run();
    }

    bool IsRunning(AlmondCore& core) {
        return core.IsItRunning();
    }

    void PrintFPS(AlmondCore& core) {
        core.PrintOutFPS();
    }

} // namespace almond

