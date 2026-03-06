#pragma once
#include <SunnyLog/SunnyLog.h>
#include <glad/glad.h>

#include <fstream>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <sstream>
#include <string>

class Shader {
public:
    Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {

        SunnyLog::setLevel(SunnyLog::ERROR);
        std::string vertexCode, fragmentCode;
        std::ifstream vertexShaderFile, fragmentShaderFile;
        vertexShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fragmentShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        // open and read the files:

        try {
            std::stringstream vertexShaderStream, fragmentShaderStream;
            vertexShaderFile.open(vertexShaderPath);
            vertexShaderStream << vertexShaderFile.rdbuf();
            vertexShaderFile.close();
            vertexCode = vertexShaderStream.str();

            fragmentShaderFile.open(fragmentShaderPath);
            fragmentShaderStream << fragmentShaderFile.rdbuf();
            fragmentShaderFile.close();
            fragmentCode = fragmentShaderStream.str();
        } catch (std::ifstream::failure e) {
            SunnyLog::log("Failed to read shaders ", e.what());
        }

        // compile shaders:

        const char* vertexShaderCode = vertexCode.c_str();
        const char* fragmentShaderCode = fragmentCode.c_str();

        unsigned int vertex, fragment;
        int success;
        char infoLog[512];

        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vertexShaderCode, nullptr);
        glCompileShader(vertex);
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
            SunnyLog::log("ERROR::SHADER::VERTEX::COMPILATION_FAILED", infoLog);
        }

        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fragmentShaderCode, nullptr);
        glCompileShader(fragment);
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
            SunnyLog::log("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED", infoLog);
        }

        // create the program and attack the shaders to it

        m_program = glCreateProgram();
        glAttachShader(m_program, vertex);
        glAttachShader(m_program, fragment);
        glLinkProgram(m_program);
        glGetProgramiv(m_program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(m_program, 512, nullptr, infoLog);
            SunnyLog::log("ERROR::SHADER::PROGRAM::LINKING_FAILED", infoLog);
        }
        SunnyLog::log(SunnyLog::INFO, "Linked Successfully");

        // get uniform location for less calls
        m_modelLoc = glGetUniformLocation(m_program, "model");
        m_viewLoc = glGetUniformLocation(m_program, "view");
        m_projLoc = glGetUniformLocation(m_program, "projection");

        // delete the shaders as they're linked into program now and no longer necessary
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    };

    ~Shader() { glDeleteProgram(m_program); }

    inline void useProgram() const { glUseProgram(m_program); }

    inline unsigned int& getProgram() { return m_program; }

    inline void deleteProgram() { glDeleteProgram(m_program); }

    inline void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(m_program, name.c_str()), (int)value);
    }

    inline void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(m_program, name.c_str()), value);
    }

    inline void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(m_program, name.c_str()), value);
    }

    inline void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(m_program, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    inline void setVec2f(const std::string& name, float value1, float value2) const {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            std::cerr << "ERROR: Uniform '" << name << "' not found!" << std::endl;
        }
        glUniform2f(loc, value1, value2);
    }

    inline void setVec3f(const std::string& name, float value1, float value2, float value3) const {
        GLint loc = glGetUniformLocation(m_program, name.c_str());
        if (loc == -1) {
            std::cerr << "ERROR: Uniform '" << name << "' not found!" << std::endl;
        }
        glUniform3f(loc, value1, value2, value3);
    }

    inline void setModel(const glm::mat4& mat) const { glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, &mat[0][0]); }

    inline void setView(const glm::mat4& mat) const { glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, &mat[0][0]); }

    inline void setProjection(const glm::mat4& mat) const { glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, &mat[0][0]); }

private:
    unsigned int m_program;
    GLint m_modelLoc = -1;
    GLint m_viewLoc = -1;
    GLint m_projLoc = -1;
};
