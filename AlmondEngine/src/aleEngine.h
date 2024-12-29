#pragma once

#include "aleEventSystem.h"
#include "aleExports_DLL.h"
#include "aleEngineConfig.h"
#include "aleLoadSave.h"
#include "alePluginManager.h"

#include "aleScene.h"
#include "aleSceneSnapshot.h"
#include "aleThreadPool.h"
#include "aleTypes.h"

#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <shared_mutex>
#include <string>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#pragma warning(disable : 4273)
#endif

namespace almond {

    void RegisterAlmondCallback(std::function<void()> callback);

    class ALMONDENGINE_API AlmondShell {
    public:
        AlmondShell(almond::RobustTime& m_timeSystem);
        AlmondShell(size_t numThreads, bool running, almond::Scene* scene, size_t maxBufferSize, almond::RobustTime& m_timeSystem);

        ~AlmondShell();

        void Run();
        //void RunWin32Desktop(MSG msg, HACCEL hAccelTable);
        bool IsItRunning() const;
        void SetRunning(bool running);
        void PrintMessage(const std::string& text);

        void PrintOutFPS();
        void UpdateFPS();
        void SetFrameRate(unsigned int targetFPS);

        void SetPlaybackTargetTime(float targetTime);
        void CaptureSnapshot();
        void SaveBinaryState();
        void LoadStateAtTime(float timeStamp);

        static void RegisterCallbackUpdate(std::function<void()> callback) {
            almond::RegisterAlmondCallback(callback);
        }

        float m_fps = 0.0f;
        // New method to handle event processing using the EventSystem
        //void ProcessEvents();
        void InitializeRenderer(const char* title, float x, float y, float width, float height, unsigned int color, void* texture); // Initialize the renderer
       // void InitializeRenderer(const std::string& title, float x, float y, float width, float height, unsigned int color, void* texture); // alternative for sfml workaround

    private:
        bool m_running;
        almond::RobustTime& m_timeSystem;
        almond::plugin::PluginManager m_pluginManager;

        // rendering
       // std::unique_ptr<RenderContext> m_renderer; // Renderer instance
        void RenderFrame(); // Render a single frame

        // scene states
        size_t m_maxBufferSize;
        std::deque<SceneSnapshot> m_recentStates;
        std::unique_ptr<almond::Scene> m_scene;

        // multithreading
        ThreadPool m_jobSystem;
        almond::SaveSystem m_saveSystem;
        std::vector<almond::Event> m_events;

        // time playback
        float m_targetTime = 0.0f;
        void UpdatePlayback();

        // framerate and delta time
        void LimitFrameRate(std::chrono::steady_clock::time_point& lastFrame);
        int m_frameCount = 0;
        std::chrono::steady_clock::time_point m_lastSecond;
        unsigned int m_targetFPS = 120;
        bool m_frameLimitingEnabled = false;
        std::chrono::steady_clock::time_point m_lastFrame = std::chrono::steady_clock::now();

        int m_saveIntervalMinutes = 1;

        // event serialization
        void Serialize(const std::string& filename, const std::vector<Event>& events);
        void Deserialize(const std::string& filename, std::vector<Event>& events);
    };

    //external functions TODO: move to another file
    extern "C" AlmondShell* CreateAlmondShell(size_t numThreads, bool running, Scene* scene, size_t maxBufferSize, almond::RobustTime& m_timeSystem);
    extern "C" void Run(AlmondShell& core);
    extern "C" bool IsRunning(AlmondShell& core);
    extern "C" void PrintFPS(AlmondShell& core);
} // namespace almond
