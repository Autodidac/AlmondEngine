#pragma once

#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <functional>
#include <iostream>

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

    class IPlugin {
    public:
        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;
        virtual ~IPlugin() = default;
    };

    class PluginManager {
    public:
        using PluginFactoryFunc = IPlugin * (*)();

        PluginManager() = default;

        ~PluginManager() {
            UnloadAllPlugins();
        }

        bool LoadPlugin(const std::filesystem::path& path) {
            PluginHandle handle = LoadSharedLibrary(path);
            if (!handle) {
                LogError("Failed to load plugin: " + path.string());
                return false;
            }

            auto factory = reinterpret_cast<PluginFactoryFunc>(GetSymbol(handle, "CreatePlugin"));
            if (!factory) {
                LogError("Missing entry point in plugin: " + path.string());
                CloseLibrary(handle);
                return false;
            }

            std::unique_ptr<IPlugin> plugin(factory());
            if (plugin) {
                plugin->Initialize();
                plugins.emplace_back(std::move(plugin), handle);
                LogInfo("Successfully loaded plugin: " + path.string());
                return true;
            }

            CloseLibrary(handle);
            return false;
        }

        void UnloadAllPlugins() {
            for (auto& [plugin, handle] : plugins) {
                plugin->Shutdown();
                CloseLibrary(handle);
            }
            plugins.clear();
        }

    private:
        std::vector<std::pair<std::unique_ptr<IPlugin>, PluginHandle>> plugins;

        void LogInfo(const std::string& message) const {
            std::cout << "[PluginManager] INFO: " << message << '\n';
        }

        void LogError(const std::string& message) const {
            std::cerr << "[PluginManager] ERROR: " << message << '\n';
        }
    };

} // namespace almond::plugin
