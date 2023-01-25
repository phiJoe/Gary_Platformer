#include "shader.h"

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

Shader::Shader(const char* vertexShader, const char* fragmentShader) {
    // Load vertex shader code
    GLchar* vertexCode = nullptr;
    GLchar* fragmentCode = nullptr;
    FILE* vertexShaderFile = fopen(vertexShader, "r");
    if (vertexShaderFile == NULL) {
        printf("Failed to open vertex-shader code\n");
    } else {
        fseek(vertexShaderFile, 0, SEEK_END);
        int32_t pos = ftell(vertexShaderFile);
        rewind(vertexShaderFile);
        vertexCode = (char*)calloc(1, pos + 1);
        if (vertexCode) {
            fread(vertexCode, pos + 1, 1, vertexShaderFile);
        }
        printf("vertex code: \n\n");
        printf("%s", vertexCode);
        printf("\n");
    }

    // Load fragment shader code
    FILE* fragmentShaderFile = fopen(fragmentShader, "r");
    if (fragmentShaderFile == NULL) {
        printf("Failed to open vertex-shader code\n");
    } else {
        fseek(fragmentShaderFile, 0, SEEK_END);
        int32_t pos = ftell(fragmentShaderFile);
        rewind(fragmentShaderFile);
        fragmentCode = (char*)calloc(1, pos + 1);
        if (fragmentCode) {
            fread(fragmentCode, pos + 1, 1, fragmentShaderFile);
        }
        printf("fragment code: \n\n");
        printf("%s", fragmentCode);
        printf("\n");
    }

    // Compile vertex code
    vertexID = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexID, 1, &vertexCode, nullptr);
    glCompileShader(vertexID);

    GLint success;
    GLchar infoLog[512];
    memset(infoLog, ' ', 512);

    glGetShaderiv(vertexID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexID, 512, NULL, infoLog);
        printf("%s\n\n", infoLog);
    } else {
        printf("Vertex shader compiled.\n");
    }

    // Compile fragment code
    fragmentID = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentID, 1, &fragmentCode, nullptr);
    glCompileShader(fragmentID);

    memset(infoLog, ' ', 512);
    glGetShaderiv(fragmentID, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentID, 512, NULL, infoLog);
        printf("%s\n\n", infoLog);
    } else {
        printf("Fragment shader compiled.\n");
    }

    // Link shader code
    shaderProgID = glCreateProgram();
    glAttachShader(shaderProgID, vertexID);
    glAttachShader(shaderProgID, fragmentID);
    glLinkProgram(shaderProgID);

    memset(infoLog, ' ', 512);
    glGetProgramiv(shaderProgID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgID, 512, NULL, infoLog);
        printf("%s\n\n", infoLog);
    } else {
        printf("Program linked.\n");
    }

    glDeleteShader(vertexID);
    glDeleteShader(fragmentID);
}

void Shader::activate() {
    glUseProgram(shaderProgID);
}

void Shader::setMatrix(const char* uniform_name, float* matrix) {
    GLuint location = glGetUniformLocation(shaderProgID, uniform_name);
    glUniformMatrix4fv(location, 1, GL_FALSE, matrix);
}

void Shader::setInt(const char* uniform_name, int value) {
    GLuint location = glGetUniformLocation(shaderProgID, uniform_name);
    glUniform1i(location, value);
}

void Shader::setBool(const char* uniform_name, bool value) {
    GLuint location = glGetUniformLocation(shaderProgID, uniform_name);
    glUniform1i(location, (int)value);
}