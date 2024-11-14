#pragma once

#include "Exports_DLL.h"
#include "LoadSave.h"
#include "Logger.h"
#include "Scene.h"
#include "SceneSnapshot.h"
#include "ThreadPool.h"

#include "framework.h"
#include "Types.h"

#include <chrono>
#include <deque>
#include <functional>
#include <memory>
#include <vector>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#pragma warning(disable : 4273)
#endif

extern "C" std::function<void(void)> m_updateCallback;
void RegisterAlmondCallback(std::function<void(void)> callback);

namespace almond {

    class ENTRYPOINTLIBRARY_API AlmondCore {
    public:
        AlmondCore(size_t numThreads, bool running, Scene* scene, size_t maxBufferSize);

        // Core loop functions
        void Run();
        void RunWin32Desktop(MSG msg, HACCEL hAccelTable);
        bool IsItRunning() const;
        void SetRunning(bool running);

        // Frame rate and FPS handling
        void PrintOutFPS();
        void UpdateFPS();
        void SetFrameRate(unsigned int targetFPS);

        // Time Playback and Snapshot System
        void SetPlaybackTargetTime(float targetTime); // Set specific time for playback
        void CaptureSnapshot();                       // Capture a new state snapshot
        void SaveBinaryState();                       // Archive a snapshot in binary format
        void LoadStateAtTime(float timeStamp);        // Load closest snapshot for a timestamp

        // Register callback for updates
        static void RegisterCallbackUpdate(std::function<void()> callback) {
            ::RegisterAlmondCallback(callback);
        }

    private:
        size_t m_maxBufferSize;                          // Maximum number of snapshots to retain
        std::deque<SceneSnapshot> m_recentStates;          // Buffer to hold recent snapshots

        std::unique_ptr<almond::Scene> m_scene;       // Active scene in the engine
        //Scene* m_scene;
        bool m_running;

        ThreadPool m_jobSystem;                       // Thread pool for parallel tasks
        almond::SaveSystem m_saveSystem;              // Save/load system
        std::vector<Event> m_events;                  // Event log for replay/serialization

        // Playback target and state control
        float m_targetTime = 0.0f;
        void UpdatePlayback();

        // Serialization and deserialization methods
        void Serialize(const std::string& filename, const std::vector<Event>& events);
        void Deserialize(const std::string& filename, std::vector<Event>& events);

        // Frame rate limiting and FPS control
        void LimitFrameRate(std::chrono::steady_clock::time_point& lastFrame);
        int m_frameCount = 0;
        float m_fps = 0.0f;
        std::chrono::steady_clock::time_point m_lastSecond;
        unsigned int m_targetFPS = 120;
        bool m_frameLimitingEnabled = false;
        std::chrono::steady_clock::time_point m_lastFrame = std::chrono::steady_clock::now();
/*
        float m_totalTime = 0.0f;
        int m_totalFrames = 0;
        float m_averageFPS = 0.0f;
*/
        int m_saveIntervalMinutes = 1;

    };

    // C-API compatible function prototypes
    extern "C" AlmondCore* CreateAlmondCore(size_t numThreads, bool running, Scene* scene, size_t maxBufferSize);
    extern "C" void Run(AlmondCore& core);
    extern "C" bool IsRunning(AlmondCore& core);
    extern "C" void PrintFPS(AlmondCore& core);

} // namespace almond
