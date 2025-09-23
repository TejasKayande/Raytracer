
#pragma once

#include "base.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {

public:

    Shader(const std::string& computeShaderPath);
    Shader(const std::string& vertextShaderPath, const std::string& fragmentShaderPath);

    ~Shader();

    Shader(const Shader&)            = delete;
    Shader(Shader&&)                 = delete;
    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&)      = delete;

    void bind() const;
    void unbind() const;

    GLuint getProgramID() const;

    void setUniform(const std::string& name, GLint value) const;
    void setUniform(const std::string& name, GLuint value) const;
    void setUniform(const std::string& name, GLfloat value) const;
    void setUniform(const std::string& name, const glm::vec2& value) const;
    void setUniform(const std::string& name, const glm::vec3& value) const;
    void setUniform(const std::string& name, const glm::vec4& value) const;
    void setUniform(const std::string& name, const glm::mat4& value) const;

private:

    GLuint m_programID;

    std::string m_loadShaderSource(const std::string& filepath) const;
    GLuint m_compileShader(GLenum type, const std::string& source);
};
