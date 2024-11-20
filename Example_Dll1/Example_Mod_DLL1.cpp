
//#include <SDL3>
#include "Example_Mod_DLL1.h"
/*
namespace almond {
    void ExampleMod::Initialize(AlmondEngine* mod) {
        this->mod = mod;
        mod->PrintMessage("ExampleMod initialized!");
    }

    void ExampleMod::Update(float deltaTime) {
        mod->PrintMessage("ExampleMod updating...");
    }

    void ExampleMod::Shutdown() {
        mod->PrintMessage("ExampleMod shutting down...");
    }

    extern "C" ENTRYPOINTLIBRARY_API Plugin * CreateMod() {
        return new ExampleMod();
    }

    extern "C" ENTRYPOINTLIBRARY_API void DestroyMod(Plugin * plugin) {
        delete plugin;
    }

//int Version() const override { return 1; }
}

#include "PluginManager.h"
#include <iostream>

class ExampleMod : public almond::plugin::IPlugin {
public:
    void Initialize() override {
        std::cout << "ExampleMod initialized!" << std::endl;
    }

    void Shutdown() override {
        std::cout << "ExampleMod shutting down!" << std::endl;
    }
};

extern "C" almond::plugin::IPlugin* CreatePlugin() {
    return new ExampleMod();
}
*/


#include "PluginManager.h"
#include <iostream>

// Define the mod class that implements the plugin interface.
class ExampleMod : public almond::plugin::IPlugin {
public:
    void Initialize() override {
        std::cout << "ExampleMod initialized!" << std::endl;
    }

    void Shutdown() override {
        std::cout << "ExampleMod shutting down!" << std::endl;
    }
};

// Exported entry point to create the mod plugin.
extern "C" __declspec(dllexport) almond::plugin::IPlugin* CreatePlugin() {
    return new ExampleMod();
}

// Exported entry point to destroy the mod plugin.
extern "C" __declspec(dllexport) void DestroyPlugin(almond::plugin::IPlugin* plugin) {
    delete plugin;
}
