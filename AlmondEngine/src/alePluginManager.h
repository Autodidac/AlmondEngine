#pragma once

#include "aleIPlugin.h"
#include "aleLogger.h"
#include "alePluginConcept.h"
#include "aleRobustTime.h"

#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <string>
#include <vector>

#ifdef _WIN32
#include "framework.h"
using PluginHandle = HMODULE;
#define LoadSharedLibrary(path) LoadLibraryA(path.string().c_str())
#define GetSymbol(handle, name) GetProcAddress(handle, name)
#define CloseLibrary(handle) FreeLibrary(handle)
#else
#include <dlfcn.h>
using PluginHandle = void*;
#define LoadSharedLibrary(path) dlopen(path.string().c_str(), RTLD_LAZY)
#define GetSymbol(handle, name) dlsym(handle, name)
#define CloseLibrary(handle) dlclose(handle)
#endif

namespace almond::plugin {

    class PluginManager {
    public:
        using PluginFactoryFunc = IPlugin * (*)();

        // Constructor with a robust time system
        explicit PluginManager(const std::string& logFileName, almond::RobustTime& timeSystem)
            : logger(almond::Logger::GetInstance(logFileName, timeSystem)), timeSystem(timeSystem) {
        }

        ~PluginManager() {
            UnloadAllPlugins();
        }

        // Load a plugin
        bool LoadPlugin(const std::filesystem::path& path) {
            logger.log("Attempting to load plugin: " + path.string());

            // Load the shared library
            PluginHandle handle = LoadSharedLibrary(path);
            if (!handle) {
                logger.log("ERROR: Failed to load plugin: " + path.string());
                return false;
            }

            // Get the plugin factory function
            auto factory = reinterpret_cast<PluginFactoryFunc>(GetSymbol(handle, "CreatePlugin"));
            if (!factory) {
                logger.log("ERROR: Missing entry point in plugin: " + path.string());
                CloseLibrary(handle);
                return false;
            }

            // Create the plugin instance
            std::unique_ptr<IPlugin> plugin(factory());
            if (plugin) {
                plugin->Initialize();
                plugins.emplace_back(std::move(plugin), handle);
                logger.log("Successfully loaded plugin: " + path.string());
                return true;
            }

            CloseLibrary(handle);
            logger.log("ERROR: Failed to create plugin instance: " + path.string());
            return false;
        }

        // Unload all plugins
        void UnloadAllPlugins() {
            logger.log("Unloading all plugins...");

            // Reverse iteration using std::ranges
            for (auto& [plugin, handle] : plugins | std::views::reverse) {
                if (plugin) {
                    logger.log("Shutting down plugin...");
                    plugin->Shutdown();
                }

                plugin.reset();
                if (handle) {
                    logger.log("Closing library handle...");
                    CloseLibrary(handle);
                }
            }

            plugins.clear();
            logger.log("All plugins unloaded.");
        }

    private:
        std::vector<std::pair<std::unique_ptr<IPlugin>, PluginHandle>> plugins;
        almond::Logger& logger;            // Shared Logger instance
        almond::RobustTime& timeSystem;    // Reference to RobustTime
    };

} // namespace almond::plugin

/*
#include "alsPluginManager.h"
#include "alsRobustTime.h"

int main() {
    almond::RobustTimeSystem timeSystem;

    // Create PluginManager with time system and optional log file name
    almond::plugin::PluginManager pluginManager(timeSystem, "plugin_log.txt");

    // Load a plugin
    pluginManager.LoadPlugin("path/to/plugin.so");

    // Unload all plugins
    pluginManager.UnloadAllPlugins();

    return 0;
}

*/