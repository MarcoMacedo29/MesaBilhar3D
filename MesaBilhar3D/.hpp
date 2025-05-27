#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>

class ObjLoader {
public:
    ObjLoader(const std::string& path);
    void draw() const;
    unsigned int getVAO() const;

private:
    void loadObj(const std::string& path);
    void setupMesh();

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;
};

#endif
