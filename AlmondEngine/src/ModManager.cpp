#include "ModManager.h"

//#include <Windows.h>
#include "framework.h"

#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>

#include <wincrypt.h>
#include <softpub.h>
#include <wintrust.h>
//#include <openssl/evp.h>  // Updated header for OpenSSL 3.0

#pragma comment(lib, "wintrust.lib")
#pragma comment(lib, "crypt32.lib")

namespace fs = std::filesystem;

// Option to allow loading unsigned mods
bool allowUnsignedMods = true; // This can be loaded from a config file or user settings

bool ModManager::LoadMods(const fs::path& modDirectory) {
    for (const auto& entry : fs::directory_iterator(modDirectory)) {
        if (entry.is_regular_file()) {
            LoadModFilesAndDirectories(entry.path());
        }
    }

    for (const auto& entry : fs::directory_iterator(modDirectory)) {
        if (entry.is_directory()) {
            LoadModFilesAndDirectories(entry.path());
        }
    }

    return true;
}

void ModManager::LoadModFilesAndDirectories(const fs::path& path) {
    if (path.extension() == ".dll") {
        if (!LoadModDLL(path)) {
            std::cerr << "Failed to load mod DLL: " << path << std::endl;
        }
    }
    else {
        std::cout << "Loaded mod resource: " << path << std::endl;
    }
}

bool ModManager::LoadModDLL(const fs::path& path) {
    if (!ValidateMod(path)) {
        std::cerr << "Invalid mod DLL: " << path << std::endl;
        return false;
    }

    if (!VerifySignature(path)) {
        std::cerr << "Signature verification failed for: " << path << std::endl;

        if (!allowUnsignedMods) {
            std::cerr << "Unsigned mods are not allowed. Skipping: " << path << std::endl;
            return false;
        }
        else {
            std::cerr << "WARNING: Loading unsigned mod: " << path << std::endl;
        }
    }

    /*
    if (!ValidateFileIntegrity(path)) {
        std::cerr << "File integrity check failed for: " << path << std::endl;
        return false;
    }*/

    if (!SandboxMod(path)) {
        std::cerr << "Sandboxing failed for mod: " << path << std::endl;
        return false;
    }

    HMODULE handle = LoadLibraryW(path.c_str());
    if (!handle) {
        DWORD error = GetLastError();
        std::cerr << "Failed to load mod DLL: " << path
            << " (Error Code: " << error
            << ", Message: " << std::system_category().message(error) << ")" << std::endl;
        return false;
    }

    using CreateModFunc = almond::Plugin* (*)();
    using DestroyModFunc = void (*)(almond::Plugin*);

    auto CreateMod = reinterpret_cast<CreateModFunc>(GetProcAddress(handle, "CreateMod"));
    auto DestroyMod = reinterpret_cast<DestroyModFunc>(GetProcAddress(handle, "DestroyMod"));

    if (!CreateMod || !DestroyMod) {
        std::cerr << "Missing CreateMod/DestroyMod function in: " << path << std::endl;
        FreeLibrary(handle);
        return false;
    }

    try {
        almond::Plugin* plugin = CreateMod();
        plugins.push_back(std::unique_ptr<almond::Plugin>(plugin));
        handles.push_back(handle);

        plugin->Initialize(mod);
    }
    catch (const std::exception& ex) {
        std::cerr << "Exception during plugin initialization: " << ex.what() << std::endl;
        FreeLibrary(handle);
        return false;
    }

    return true;
}

bool ModManager::SandboxMod(const fs::path& path) {
    // Example: Implement sandboxing logic here
    // You can limit the mod’s access to certain system resources, such as file systems, network access, etc.
    // Use platform-specific APIs for this, such as Windows Job Objects or other isolation techniques.

    // Return true if sandboxing is successful
    return true;
}

void ModManager::UpdateMods(float deltaTime) {
    for (auto& plugin : plugins) {
        plugin->Update(deltaTime);
    }
}

void ModManager::ShutdownMods() {
    for (auto& plugin : plugins) {
        plugin->Shutdown();
    }

    for (void* handle : handles) {
        FreeLibrary(static_cast<HMODULE>(handle));
    }
}

bool ModManager::ValidateMod(const fs::path& path) {
    return path.extension() == ".dll";
}

bool ModManager::VerifySignature(const fs::path& path) {
    WINTRUST_FILE_INFO fileInfo = {};
    fileInfo.cbStruct = sizeof(WINTRUST_FILE_INFO);
    fileInfo.pcwszFilePath = path.c_str();

    WINTRUST_DATA winTrustData = {};
    winTrustData.cbStruct = sizeof(WINTRUST_DATA);
    winTrustData.pPolicyCallbackData = NULL;
    winTrustData.pSIPClientData = NULL;
    winTrustData.dwUnionChoice = WTD_CHOICE_FILE;
    winTrustData.dwStateAction = WTD_STATEACTION_VERIFY;
    winTrustData.pFile = &fileInfo;
    winTrustData.dwProvFlags = WTD_REVOCATION_CHECK_NONE | WTD_CACHE_ONLY_URL_RETRIEVAL;
    winTrustData.dwUIChoice = WTD_UI_NONE;
    winTrustData.fdwRevocationChecks = WTD_REVOCATION_CHECK_NONE;

    GUID WVTPolicyGUID = WINTRUST_ACTION_GENERIC_VERIFY_V2;

    LONG status = WinVerifyTrust(NULL, &WVTPolicyGUID, &winTrustData);

    if (status == TRUST_E_NOSIGNATURE) {
        std::cerr << "No signature was present in the subject for: " << path << std::endl;
        return false;
    }
    else if (status != ERROR_SUCCESS) {
        std::cerr << "Signature verification failed with error code: " << status
            << " (" << std::system_category().message(status) << ")" << std::endl;
        return false;
    }

    return true;
}

/*
std::string ModManager::CalculateFileHash(const fs::path& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for hashing: " << path << std::endl;
        return "";
    }

    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        std::cerr << "Failed to create EVP_MD_CTX" << std::endl;
        return "";
    }

    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), NULL)) {
        std::cerr << "EVP_DigestInit_ex failed" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    const size_t bufferSize = 4096;
    char buffer[bufferSize];
    while (file.read(buffer, bufferSize)) {
        if (1 != EVP_DigestUpdate(mdctx, buffer, file.gcount())) {
            std::cerr << "EVP_DigestUpdate failed" << std::endl;
            EVP_MD_CTX_free(mdctx);
            return "";
        }
    }

    if (file.bad()) {
        std::cerr << "Failed to read file: " << path << std::endl;
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    if (1 != EVP_DigestFinal_ex(mdctx, hash, &lengthOfHash)) {
        std::cerr << "EVP_DigestFinal_ex failed" << std::endl;
        EVP_MD_CTX_free(mdctx);
        return "";
    }

    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }

    return ss.str();
}
bool ModManager::ValidateFileIntegrity(const fs::path& path) {
    std::string hash = CalculateFileHash(path);

    // Lookup table or config file mechanism for known hashes
    std::unordered_map<std::string, std::string> knownHashes = {
        {"mod1.dll", "c48af92a5fd60cad911727b6b4774b7381dbb9896b99de531c32e888f69e2778"},
        {"mod2.dll", "another_known_hash_here"}
        // Add more entries as needed
    };

    std::string fileName = path.filename().string();
    auto it = knownHashes.find(fileName);

    if (it != knownHashes.end() && hash == it->second) {
        return true;
    }
    else {
        std::cerr << "Invalid mod hash: " << hash << " for file: " << fileName << std::endl;
        return false;
    }
}*/
