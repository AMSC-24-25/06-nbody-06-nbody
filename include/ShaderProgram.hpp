#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include <string>
#include <iostream>

#include <GL/glew.h>

class ShaderProgram
{
public:

    ShaderProgram();

    bool loadShader(const std::string &source_path, GLenum shader_type);
    bool link();
    void use();

    ~ShaderProgram();

private:
    GLuint _program_id;

    static const unsigned int _MAX_LOG_LENGTH = 1024;
};

#endif
