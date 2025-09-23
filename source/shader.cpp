
#include "shader.hpp"

#include "base.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const std::string& computeShaderPath) {

    std::string source = m_loadShaderSource(computeShaderPath);
    GLuint shader = m_compileShader(GL_COMPUTE_SHADER, source);

    m_programID = glCreateProgram();

    glAttachShader(m_programID, shader);
    glLinkProgram(m_programID);

    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed\n" << infoLog << "\n";
    }

    glDeleteShader(shader);
}

Shader::Shader(const std::string& vertexShaderPath, const std::string& fragmentShaderPath) {

    std::string vertSrc = m_loadShaderSource(vertexShaderPath);
    std::string fragSrc = m_loadShaderSource(fragmentShaderPath);

    GLuint vertShader = m_compileShader(GL_VERTEX_SHADER, vertSrc);
    GLuint fragShader = m_compileShader(GL_FRAGMENT_SHADER, fragSrc);

    m_programID = glCreateProgram();
    glAttachShader(m_programID, vertShader);
    glAttachShader(m_programID, fragShader);
    glLinkProgram(m_programID);

    GLint success;
    glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_programID, 512, nullptr, infoLog);
        std::cerr << "Shader Program Linking Failed\n" << infoLog << "\n";
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

Shader::~Shader() {

    glDeleteProgram(m_programID);
}

void Shader::bind() const {

    glUseProgram(m_programID);
}

void Shader::unbind() const {

    glUseProgram(0);
}

GLuint Shader::getProgramID() const {

    return m_programID;
}

void Shader::setUniform(const std::string& name, GLint value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniform1i(loc, value);
    }
}

void Shader::setUniform(const std::string& name, GLuint value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniform1ui(loc, value);
    }
}

void Shader::setUniform(const std::string& name, GLfloat value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniform1f(loc, value);
    }
}

void Shader::setUniform(const std::string& name, const glm::vec2& value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniform2fv(loc, 1, &value[0]);
    }
}

void Shader::setUniform(const std::string& name, const glm::vec3& value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniform3fv(loc, 1, &value[0]);
    }
}

void Shader::setUniform(const std::string& name, const glm::vec4& value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniform4fv(loc, 1, &value[0]);
    }
}

void Shader::setUniform(const std::string& name, const glm::mat4& value) const {

    GLint loc = glGetUniformLocation(m_programID, name.c_str());
    if (loc == -1) {
        std::cerr << "Uniform '" << name << "' not found in shader.\n";
    } else {
        glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]);
    }
}

std::string Shader::m_loadShaderSource(const std::string& filepath) const {

    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Shader File Not Found: " << filepath << "\n";
        return "";
    }

    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

GLuint Shader::m_compileShader(GLenum type, const std::string& source) {

    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "Shader Compilation Falied\n" << infoLog << "\n";
    }

    return shader;
}
