#pragma once

#include <string>
#include "PluginManager.h"
#include "Exports_DLL.h"

namespace almond {
    class AlmondEngine; // Forward declaration.

    class ExampleMod : public almond::plugin::IPlugin {
    public:
        void Initialize() override;
       // void Update(float deltaTime) override;
        void Shutdown() override;
       // std::string Version() const override { return "0.0.1"; }

    private:
        AlmondEngine* mod;
    };
}
