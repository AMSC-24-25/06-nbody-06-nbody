#include "ShaderProgram.hpp"

#include <fstream>
#include <iostream>
#include <string>

#include <GL/glew.h>

ShaderProgram::ShaderProgram()
{
    _program_id = glCreateProgram();
}

bool ShaderProgram::loadShader(const std::string &source_path,
                               GLenum shader_type)
{
    const GLuint shader = glCreateShader(shader_type);

    // TODO: handle io error
    std::ifstream source_file(source_path);
    // Get file length
    source_file.seekg(0, source_file.end);
    int length = source_file.tellg();
    source_file.seekg(0, source_file.beg);

    std::string buffer;
    buffer.reserve(length + 1);
    source_file.read(buffer.data(), length);
    buffer[length] = 0;

    // Loading shader source code
    char *ptr = buffer.data();
    glShaderSource(shader, 1, &ptr, nullptr);

    glCompileShader(shader);
    // Checking for compilation status.
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        buffer.clear();
        buffer.reserve(_MAX_LOG_LENGTH);
        // Getting log string
        glGetShaderInfoLog(shader, _MAX_LOG_LENGTH, nullptr, buffer.data());
        std::cout << "Error while compiling shader " << source_path <<
            ": " << buffer.data();
        return false;
    }

    glAttachShader(_program_id, shader);
    glDeleteShader(shader);

    return true;
}

bool ShaderProgram::link()
{
    glLinkProgram(_program_id);

    // Checking for link status.
    int success;
    glGetProgramiv(_program_id, GL_LINK_STATUS, &success);
    if (!success) {
        std::string buffer;
        buffer.reserve(_MAX_LOG_LENGTH);
        buffer.clear();

        glGetProgramInfoLog(_program_id,
                            _MAX_LOG_LENGTH,
                            nullptr,
                            buffer.data());
        std::cout << "Error while linking program: " << buffer.data();
        return false;
    }

    return true;
}

void ShaderProgram::use()
{
    glUseProgram(_program_id);
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(_program_id);
}
