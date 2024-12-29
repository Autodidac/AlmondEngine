#pragma once

#include "aleExports_DLL.h"
#include "alePluginManager.h"

#include <string>

namespace almond {
    class AlmondShell; // Forward declaration.

    class ExampleMod : public almond::plugin::IPlugin {
    public:
        void Initialize() override;
       // void Update(float deltaTime) override;
        void Shutdown() override;
       // std::string Version() const override { return "0.0.1"; }

    private:
        AlmondShell* mod;
    };
}
