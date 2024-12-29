
#include "aleMesh.h"

#ifdef ALMOND_USING_GLFW

namespace almond {
    Mesh::Mesh(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    {
        init(vertices, indices);
    }

    Mesh::~Mesh()
    {
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    void Mesh::init(const std::vector<float>& vertices, const std::vector<unsigned int>& indices)
    {
        vertexCount = static_cast<unsigned int>(vertices.size() / 4);
        indexCount = static_cast<unsigned int>(indices.size());

        glGenBuffers(1, &VBO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glGenBuffers(1, &EBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    }
}

#endif