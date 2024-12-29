#pragma once

#include "aleEngineConfig.h"

//#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
//#include <glm/glm.hpp>

#ifdef ALMOND_USING_GLFW

namespace almond {

    class ShaderProgram {
    public:
        // Constructor that reads and compiles shaders
        ShaderProgram(const std::string& vertexPath, const std::string& fragmentPath) {
            std::string vertexCode, fragmentCode;

            // Read shader source code from files
            if (!ReadShaderSource(vertexPath, vertexCode) || !ReadShaderSource(fragmentPath, fragmentCode)) {
                std::cerr << "ERROR: Shader source files could not be read." << std::endl;
                return;
            }

            // Compile shaders
            GLuint vertexShader = CompileShader(vertexCode.c_str(), GL_VERTEX_SHADER, "VERTEX");
            GLuint fragmentShader = CompileShader(fragmentCode.c_str(), GL_FRAGMENT_SHADER, "FRAGMENT");

            // Link shaders into the program
            ID = glCreateProgram();
            glAttachShader(ID, vertexShader);
            glAttachShader(ID, fragmentShader);
            glLinkProgram(ID);
            CheckCompileErrors(ID, "PROGRAM");

            // Delete shaders after linking
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
        }

        // Use the shader program
        void Use() const {
            glUseProgram(ID);
        }

        // Get the program's ID
        GLuint GetID() const {
            return ID;
        }

        // Set uniform variables
        void SetUniform(const std::string& name, int value) const {
            GLint location = glGetUniformLocation(ID, name.c_str());
            if (location != -1) {
                glUniform1i(location, value);
            }
        }

        void SetUniform(const std::string& name, float value) const {
            GLint location = glGetUniformLocation(ID, name.c_str());
            if (location != -1) {
                glUniform1f(location, value);
            }
        }

        void SetUniform(const std::string& name, const glm::vec2& value) const {
            GLint location = glGetUniformLocation(ID, name.c_str());
            if (location != -1) {
                glUniform2f(location, value.x, value.y);
            }
        }

        void SetUniform(const std::string& name, const glm::vec3& value) const {
            GLint location = glGetUniformLocation(ID, name.c_str());
            if (location != -1) {
                glUniform3f(location, value.x, value.y, value.z);
            }
        }

        void SetUniform(const std::string& name, const glm::mat4& value) const {
            GLint location = glGetUniformLocation(ID, name.c_str());
            if (location != -1) {
                glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
            }
        }

        // Destructor to clean up the shader program
        ~ShaderProgram() {
            if (ID != 0) {
                glDeleteProgram(ID);
            }
        }

    private:
        GLuint ID;

        // Read shader source code from file
        bool ReadShaderSource(const std::string& filepath, std::string& code) const {
            std::ifstream shaderFile(filepath);
            if (!shaderFile.is_open()) {
                std::cerr << "ERROR: Could not open shader file: " << filepath << std::endl;
                return false;
            }

            std::stringstream shaderStream;
            shaderStream << shaderFile.rdbuf();
            code = shaderStream.str();
            return true;
        }

        // Compile a shader from source code
        GLuint CompileShader(const char* code, GLenum type, const std::string& typeName) const {
            GLuint shader = glCreateShader(type);
            glShaderSource(shader, 1, &code, nullptr);
            glCompileShader(shader);
            CheckCompileErrors(shader, typeName);
            return shader;
        }

        // Check for errors during shader compilation or program linking
        void CheckCompileErrors(GLuint object, const std::string& type) const {
            GLint success;
            char infoLog[1024];
            if (type != "PROGRAM") {
                glGetShaderiv(object, GL_COMPILE_STATUS, &success);
                if (!success) {
                    glGetShaderInfoLog(object, 1024, nullptr, infoLog);
                    std::cerr << "ERROR: Shader Compilation Error (" << type << "):\n" << infoLog << std::endl;
                }
            }
            else {
                glGetProgramiv(object, GL_LINK_STATUS, &success);
                if (!success) {
                    glGetProgramInfoLog(object, 1024, nullptr, infoLog);
                    std::cerr << "ERROR: Shader Program Linking Error:\n" << infoLog << std::endl;
                }
            }
        }
    };

} // namespace almond

#endif
