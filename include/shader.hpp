#pragma once
#include <glm/mat4x4.hpp>
#include <string>

#define Program unsigned int
#define Uniform unsigned int

struct GridShader {
    Program id;
    Uniform uScreenDimensions;
};

struct CircleShader {
    Program id;
    Uniform uModel;
    Uniform uView;
    Uniform uProjection;
};

GridShader createGridShader(std::string vert, std::string frag);

CircleShader createCircleShader(std::string vert, std::string frag);

void setUniformMat4(unsigned int uniformId, glm::mat4* matrix);

void setUniformVec2(unsigned int uniformId, glm::vec2* vec);
