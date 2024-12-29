#pragma once

#include "alePluginManager.h"

#include <future>
#include <thread>

namespace almond::plugin {

    // AsyncPluginManager extends PluginManager to add async loading of plugins
    class AsyncPluginManager : public PluginManager {
    public:
        std::future<bool> LoadPluginAsync(const std::filesystem::path& path) {
            return std::async(std::launch::async, [this, path]() {
                return LoadPlugin(path);
                });
        }
    };

} // namespace almond::plugin
