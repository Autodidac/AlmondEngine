#pragma once
#include <future>
#include <thread>
#include "PluginManager.h"

namespace almond::plugin {
    class AsyncPluginManager : public PluginManager {
    public:
        std::future<bool> LoadPluginAsync(const std::filesystem::path& path) {
            return std::async(std::launch::async, [this, path]() {
                return LoadPlugin(path);
                });
        }
    };
}
