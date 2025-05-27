#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// stb_image para carregar textura
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace P3D {

    class Model {
    public:
        Model();
        ~Model();

        bool Load(const std::string& objFilePath);
        void Install();
        void Render(GLuint shaderProgram, const glm::vec3& position, const glm::vec3& orientation);
        void BindShaderAttributes(GLuint shaderProgram);

    private:
        // Dados do modelo
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> texCoords;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        // Material
        glm::vec3 Ka;
        glm::vec3 Kd;
        glm::vec3 Ks;
        float Ns;

        GLuint textureID;

        GLuint VAO;
        GLuint VBO_Vertices;
        GLuint VBO_TexCoords;
        GLuint VBO_Normals;
        GLuint EBO;

        std::string mtlFileName;
        std::string textureFileName;

        bool LoadOBJ(const std::string& objFilePath);
        bool LoadMTL(const std::string& mtlFilePath);
        bool LoadTexture(const std::string& textureFilePath);

        void SetupBuffers();
    };

    Model::Model()
        : Ka(0.1f), Kd(0.8f), Ks(1.0f), Ns(32.0f),
        textureID(0),
        VAO(0), VBO_Vertices(0), VBO_TexCoords(0), VBO_Normals(0), EBO(0)
    {
    }

    Model::~Model() {
        glDeleteBuffers(1, &VBO_Vertices);
        glDeleteBuffers(1, &VBO_TexCoords);
        glDeleteBuffers(1, &VBO_Normals);
        glDeleteBuffers(1, &EBO);
        glDeleteVertexArrays(1, &VAO);
        glDeleteTextures(1, &textureID);
    }

    bool Model::Load(const std::string& objFilePath) {
        if (!LoadOBJ(objFilePath)) {
            std::cerr << "Erro ao carregar OBJ: " << objFilePath << std::endl;
            return false;
        }
        if (!LoadMTL(mtlFileName)) {
            std::cerr << "Erro ao carregar MTL: " << mtlFileName << std::endl;
            return false;
        }
        if (!LoadTexture(textureFileName)) {
            std::cerr << "Erro ao carregar textura: " << textureFileName << std::endl;
            return false;
        }
        return true;
    }

    bool Model::LoadOBJ(const std::string& objFilePath) {
        std::ifstream file(objFilePath);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "v") {
                glm::vec3 v;
                iss >> v.x >> v.y >> v.z;
                vertices.push_back(v);
            }
            else if (prefix == "vt") {
                glm::vec2 vt;
                iss >> vt.x >> vt.y;
                texCoords.push_back(vt);
            }
            else if (prefix == "vn") {
                glm::vec3 vn;
                iss >> vn.x >> vn.y >> vn.z;
                normals.push_back(vn);
            }
            else if (prefix == "f") {
                // Parse faces do tipo: v/vt/vn, v//vn, v/vt, ou apenas v
                std::string vertexStr;
                for (int i = 0; i < 3; ++i) {
                    iss >> vertexStr;
                    unsigned int vi = 0, ti = 0, ni = 0;

                    size_t firstSlash = vertexStr.find('/');
                    size_t lastSlash = vertexStr.rfind('/');

                    if (firstSlash == std::string::npos) {
                        // só vértice
                        vi = std::stoi(vertexStr);
                    }
                    else if (firstSlash == lastSlash) {
                        // v/vt
                        vi = std::stoi(vertexStr.substr(0, firstSlash));
                        ti = std::stoi(vertexStr.substr(firstSlash + 1));
                    }
                    else {
                        // v/vt/vn ou v//vn
                        vi = std::stoi(vertexStr.substr(0, firstSlash));
                        std::string middle = vertexStr.substr(firstSlash + 1, lastSlash - firstSlash - 1);
                        if (!middle.empty()) {
                            ti = std::stoi(middle);
                        }
                        ni = std::stoi(vertexStr.substr(lastSlash + 1));
                    }

                    indices.push_back(vi - 1);
                    // Opcional: pode armazenar ti e ni se quiser usar
                }
            }
            else if (prefix == "mtllib") {
                iss >> mtlFileName;
            }
        }
        file.close();
        return true;
    }

    bool Model::LoadMTL(const std::string& mtlFilePath) {
        std::ifstream file(mtlFilePath);
        if (!file.is_open()) return false;

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string prefix;
            iss >> prefix;

            if (prefix == "Ka") {
                iss >> Ka.r >> Ka.g >> Ka.b;
            }
            else if (prefix == "Kd") {
                iss >> Kd.r >> Kd.g >> Kd.b;
            }
            else if (prefix == "Ks") {
                iss >> Ks.r >> Ks.g >> Ks.b;
            }
            else if (prefix == "Ns") {
                iss >> Ns;
            }
            else if (prefix == "map_Kd") {
                iss >> textureFileName;
            }
        }
        file.close();
        return true;
    }

    bool Model::LoadTexture(const std::string& textureFilePath) {
        int width, height, nrChannels;
        unsigned char* data = stbi_load(textureFilePath.c_str(), &width, &height, &nrChannels, 0);
        if (!data) {
            std::cerr << "Falha ao carregar textura: " << textureFilePath << std::endl;
            return false;
        }

        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(data);

        return true;
    }

    void Model::Install() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO_Vertices);
        glGenBuffers(1, &VBO_TexCoords);
        glGenBuffers(1, &VBO_Normals);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_TexCoords);
        glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(glm::vec2), texCoords.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
        glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glBindVertexArray(0);
    }

    void Model::BindShaderAttributes(GLuint shaderProgram) {
        glBindVertexArray(VAO);

        GLint posLoc = glGetAttribLocation(shaderProgram, "position");
        if (posLoc != -1) {
            glEnableVertexAttribArray(posLoc);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_Vertices);
            glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }

        GLint texLoc = glGetAttribLocation(shaderProgram, "texcoord");
        if (texLoc != -1) {
            glEnableVertexAttribArray(texLoc);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_TexCoords);
            glVertexAttribPointer(texLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        }

        GLint normLoc = glGetAttribLocation(shaderProgram, "normal");
        if (normLoc != -1) {
            glEnableVertexAttribArray(normLoc);
            glBindBuffer(GL_ARRAY_BUFFER, VBO_Normals);
            glVertexAttribPointer(normLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
        }

        glBindVertexArray(0);
    }

    void Model::Render(GLuint shaderProgram, const glm::vec3& position, const glm::vec3& orientation) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        // Exemplo simples de orientação (assumindo vetor Euler em radians)
        model = glm::rotate(model, orientation.x, glm::vec3(1, 0, 0));
        model = glm::rotate(model, orientation.y, glm::vec3(0, 1, 0));
        model = glm::rotate(model, orientation.z, glm::vec3(0, 0, 1));
        GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // Bind da textura
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }
} // namespace P3D