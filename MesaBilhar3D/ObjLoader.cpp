#include "ObjLoader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glad/glad.h>

ObjLoader::ObjLoader(const std::string& path) {
    loadObj(path);
    setupMesh();
}

void ObjLoader::loadObj(const std::string& path) {
    std::ifstream file(path);
    std::string line;
    std::vector<glm::vec3> temp_pos;
    std::vector<glm::vec2> temp_tex;
    std::vector<glm::vec3> temp_norm;

    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            glm::vec3 pos;
            ss >> pos.x >> pos.y >> pos.z;
            temp_pos.push_back(pos);
        }
        else if (type == "vt") {
            glm::vec2 tex;
            ss >> tex.x >> tex.y;
            texCoords.push_back(tex);
        }
        else if (type == "vn") {
            glm::vec3 norm;
            ss >> norm.x >> norm.y >> norm.z;
            temp_norm.push_back(norm);
        }
        else if (type == "f") {
            unsigned int p, t, n;
            char slash;
            for (int i = 0; i < 3; i++) {
                ss >> p >> slash >> t >> slash >> n;
                positions.push_back(temp_pos[p - 1]);
                if (!temp_tex.empty()) texCoords.push_back(texCoords[t - 1]);
                if (!temp_norm.empty()) normals.push_back(temp_norm[n - 1]);
                indices.push_back(indices.size());
            }
        }
    }
}

void ObjLoader::setupMesh() {
    std::vector<float> vertexData;
    for (size_t i = 0; i < positions.size(); ++i) {
        vertexData.push_back(positions[i].x);
        vertexData.push_back(positions[i].y);
        vertexData.push_back(positions[i].z);
        if (i < texCoords.size()) {
            vertexData.push_back(texCoords[i].x);
            vertexData.push_back(texCoords[i].y);
        }
    }

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), &vertexData[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    int stride = 5 * sizeof(float); // pos (3) + tex (2)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void ObjLoader::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

unsigned int ObjLoader::getVAO() const {
    return VAO;
}
