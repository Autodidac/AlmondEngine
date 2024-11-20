#pragma once

#include "Exports_DLL.h"
#include "AlmondEngine.h"
#include <vector>
#include <string>
#include <memory>
#include <filesystem>

class ModManager {
public:
    ModManager(almond::AlmondEngine* mod) : mod(mod) {}

    bool LoadMods(const std::filesystem::path& modDirectory);
    void UpdateMods(float deltaTime);
    void ShutdownMods();

private:
    std::vector<std::unique_ptr<almond::Plugin>> plugins;
    std::vector<void*> handles;
    almond::AlmondEngine* mod;

    bool SandboxMod(const std::filesystem::path& path);
    bool ValidateMod(const std::filesystem::path& path);
    bool VerifySignature(const std::filesystem::path& path);
    //bool ValidateFileIntegrity(const std::filesystem::path& path);
    //std::string CalculateFileHash(const std::filesystem::path& path);

    bool LoadModDLL(const std::filesystem::path& path);
    void LoadModFilesAndDirectories(const std::filesystem::path& path);
};