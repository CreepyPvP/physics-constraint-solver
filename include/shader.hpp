#pragma once
#include <string>

#define Program unsigned int

struct GridShader {
    Program id;
};

GridShader createGridShader(std::string vert, std::string frag);
