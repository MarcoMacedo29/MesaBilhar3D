#pragma once
#include <string>
#include <vector>
#include <glm/glm.hpp>

struct Mesh {
    std::vector<float> vertices;
    std::vector<unsigned int> indices;
    unsigned int VAO, VBO, EBO;
};

Mesh loadOBJ(const std::string& filepath);
