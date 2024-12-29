#pragma once

#include "aleEngineConfig.h"

#ifdef ALMOND_USING_GLFW
#include <vector>
#include <memory>
//#include <glad/glad.h>


namespace almond {
    class Mesh {
    public:
        Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
        ~Mesh();

        unsigned int GetVBO() const { return VBO; }
        unsigned int GetEBO() const { return EBO; }
        unsigned int GetVertexCount() const { return vertexCount; }
        unsigned int GetIndexCount() const { return indexCount; }

    private:
        unsigned int VBO = 0, EBO = 0;
        unsigned int vertexCount = 0, indexCount = 0;

        void init(const std::vector<float>& vertices, const std::vector<unsigned int>& indices);
    };
}
#endif