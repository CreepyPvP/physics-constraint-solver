#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.hpp>

#include <fstream>
#include <sstream>

static std::string readFile(std::string file) {
    try {
        std::ifstream handle(file);
        handle.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        std::stringstream buffer;
        buffer << handle.rdbuf();
        return buffer.str();
    } catch (...) {
        printf("failed to read file: %s\n", file.c_str());
    }
    return "";
}

static Program createShader(std::string vFile, std::string fFile) {
    std::string vRaw = readFile(vFile);
    std::string fRaw = readFile(fFile);

    const char* vCode = vRaw.c_str();
    const char* fCode = fRaw.c_str();

    unsigned int vertex, fragment;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("Error compiling vertex shader: %s\n", infoLog);
    }

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("Error compiling fragment shader: %s\n", infoLog);
    }

    Program shaderId = glCreateProgram();
    glAttachShader(shaderId, vertex);
    glAttachShader(shaderId, fragment);
    glLinkProgram(shaderId);
    glGetProgramiv(shaderId, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderId, 512, NULL, infoLog);
        printf("Error linking shader: %s\n", infoLog);
    }

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return shaderId;
}

GridShader createGridShader(std::string vert, std::string frag) {
    GridShader shader;
    shader.id = createShader(vert, frag);
    shader.uScreenDimensions = glGetUniformLocation(shader.id, "screenDimensions");
    return shader;
}

CircleShader createCircleShader(std::string vert, std::string frag) {
    CircleShader shader;
    shader.id = createShader(vert, frag);
    shader.uModel = glGetUniformLocation(shader.id, "model");
    shader.uView = glGetUniformLocation(shader.id, "view");
    shader.uProjection = glGetUniformLocation(shader.id, "projection");
    return shader;
}

void setUniformMat4(unsigned int uniformId, glm::mat4 *matrix) {
    glUniformMatrix4fv(uniformId, 1, GL_FALSE, &(*matrix)[0][0]);
}

void setUniformVec2(unsigned int uniformId, glm::vec2 *vec) {
    glUniform2fv(uniformId, 1, &(*vec)[0]);
}
