#include "Model.h"
#include <fstream>
#include <sstream>
#include <iostream>

using namespace Pool3D;

Model::Model() {}

Model::~Model() {}

bool Model::LoadOBJ(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo OBJ: " << filename << std::endl;
        return false;
    }

    directory = filename.substr(0, filename.find_last_of('/'));

    std::string line;
    while (std::getline(file, line)) {
        ProcessOBJLine(line);
    }

    file.close();
    return true;
}

void Model::ProcessOBJLine(const std::string& line) {
    std::istringstream iss(line);
    std::string prefix;
    iss >> prefix;

    if (prefix == "v") {
        glm::vec3 pos;
        iss >> pos.x >> pos.y >> pos.z;
        tempPositions.push_back(pos);
    }
    else if (prefix == "vt") {
        glm::vec2 tex;
        iss >> tex.x >> tex.y;
        tempTexCoords.push_back(tex);
    }
    else if (prefix == "vn") {
        glm::vec3 norm;
        iss >> norm.x >> norm.y >> norm.z;
        tempNormals.push_back(norm);
    }
    else if (prefix == "f") {
        std::string vertexStr;
        while (iss >> vertexStr) {
            std::istringstream viss(vertexStr);
            std::string v, t, n;

            size_t p1 = vertexStr.find('/');
            size_t p2 = vertexStr.find('/', p1 + 1);

            int vi = std::stoi(vertexStr.substr(0, p1)) - 1;
            int ti = std::stoi(vertexStr.substr(p1 + 1, p2 - p1 - 1)) - 1;
            int ni = std::stoi(vertexStr.substr(p2 + 1)) - 1;

            Vertex vertex;
            vertex.position = tempPositions[vi];
            vertex.texCoord = tempTexCoords[ti];
            vertex.normal = tempNormals[ni];

            vertices.push_back(vertex);
            indices.push_back(static_cast<unsigned int>(vertices.size() - 1));
        }
    }
    else if (prefix == "mtllib") {
        iss >> mtlFileName;
        std::cout << "Material file: " << mtlFileName << std::endl;
    }
}

