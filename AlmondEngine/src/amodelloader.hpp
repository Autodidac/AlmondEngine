/**************************************************************
 *   █████╗ ██╗     ███╗   ███╗   ███╗   ██╗    ██╗██████╗    *
 *  ██╔══██╗██║     ████╗ ████║ ██╔═══██╗████╗  ██║██╔══██╗   *
 *  ███████║██║     ██╔████╔██║ ██║   ██║██╔██╗ ██║██║  ██║   *
 *  ██╔══██║██║     ██║╚██╔╝██║ ██║   ██║██║╚██╗██║██║  ██║   *
 *  ██║  ██║███████╗██║ ╚═╝ ██║ ╚██████╔╝██║ ╚████║██████╔╝   *
 *  ╚═╝  ╚═╝╚══════╝╚═╝     ╚═╝  ╚═════╝ ╚═╝  ╚═══╝╚═════╝    *
 *                                                            *
 *   This file is part of the Almond Project.                 *
 *   AlmondEngine - Modular C++ Game Engine                   *
 *                                                            *
 *   SPDX-License-Identifier: LicenseRef-MIT-NoSell           *
 *                                                            *
 *   Provided "AS IS", without warranty of any kind.          *
 *   Use permitted for non-commercial purposes only           *
 *   without prior commercial licensing agreement.            *
 *                                                            *
 *   Redistribution allowed with this notice.                 *
 *   No obligation to disclose modifications.                 *
 *   See LICENSE file for full terms.                         *
 **************************************************************/
#pragma once

#include "acontext.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <fstream> // Add this include to resolve incomplete type "std::ifstream" error  
#include <sstream> // Add this include for std::istringstream  
#include <algorithm> // Add this include for std::replace  

namespace almondnamespace {

    // Pure CPU-side mesh data (no GL types)
    struct MeshData {
        std::vector<float> positions;  // x,y,z triples
        std::vector<float> normals;    // x,y,z triples
        std::vector<float> uvs;        // u,v pairs
        std::vector<unsigned int> indices;
    };

    // Model composed of one or more meshes
    struct ModelData {
        std::vector<MeshData> meshes;
    };

    // Model cache to avoid reloading
    class ModelCache {
    public:
        ModelCache() = default;
        ModelData& get(const std::string& path) {
            auto it = m_cache.find(path);
            if (it != m_cache.end()) return *it->second;
            auto model = std::make_unique<ModelData>();
            loadOBJ(path, *model);
            ModelData& ref = *model;
            m_cache[path] = std::move(model);
            return ref;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<ModelData>> m_cache;

        void loadOBJ(const std::string& filepath, ModelData& model) {
            // Simple CPU-only OBJ parse: fill MeshData.positions, normals, uvs, indices
            std::ifstream file(filepath);
            if (!file) throw std::runtime_error("Cannot open OBJ: " + filepath);

            std::vector<float> tempPos;
            std::vector<float> tempUV;
            std::vector<float> tempNorm;
            MeshData mesh;
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream iss(line);
                std::string prefix;
                iss >> prefix;
                if (prefix == "v") {
                    float x, y, z; iss >> x >> y >> z;
                    tempPos.push_back(x);
                    tempPos.push_back(y);
                    tempPos.push_back(z);
                }
                else if (prefix == "vt") {
                    float u, v; iss >> u >> v;
                    tempUV.push_back(u);
                    tempUV.push_back(v);
                }
                else if (prefix == "vn") {
                    float nx, ny, nz; iss >> nx >> ny >> nz;
                    tempNorm.push_back(nx);
                    tempNorm.push_back(ny);
                    tempNorm.push_back(nz);
                }
                else if (prefix == "f") {
                    // face: v/t/n
                    for (int i = 0; i < 3; ++i) {
                        std::string token; iss >> token;
                        unsigned int vi = 0, uvi = 0, ni = 0;
                        std::replace(token.begin(), token.end(), '/', ' ');
                        std::istringstream ts(token);
                        ts >> vi >> uvi >> ni;
                        // OBJ is 1-based
                        size_t pidx = (vi - 1) * 3;
                        mesh.positions.push_back(tempPos[pidx + 0]);
                        mesh.positions.push_back(tempPos[pidx + 1]);
                        mesh.positions.push_back(tempPos[pidx + 2]);
                        size_t uvIdx = (uvi - 1) * 2;
                        mesh.uvs.push_back(tempUV[uvIdx + 0]);
                        mesh.uvs.push_back(tempUV[uvIdx + 1]);
                        size_t nIdx = (ni - 1) * 3;
                        mesh.normals.push_back(tempNorm[nIdx + 0]);
                        mesh.normals.push_back(tempNorm[nIdx + 1]);
                        mesh.normals.push_back(tempNorm[nIdx + 2]);
                        mesh.indices.push_back((unsigned int)mesh.indices.size());
                    }
                }
            }
            model.meshes.push_back(std::move(mesh));
        }
    };

    // Global CPU model cache
    inline ModelCache g_modelCache;

    // Thunk to integrate into Context: returns handle to CPU ModelData
    inline int add_model_thunk(char const* name, char const* path) {
        ModelData& m = g_modelCache.get(path);
        return (int)(intptr_t)&m;
    }

    // Register model API on Context (CPU-only signatures)
    inline void register_model_api(Context& ctx) {
        ctx.add_model = &add_model_thunk;
        // drawing and GL integration lives in aopenglcontext.hpp
    }

} // namespace almondnamespace
