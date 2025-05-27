#ifndef MODEL_H
#define MODEL_H

#include <string>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <glad/glad.h>

namespace Pool3D {

    struct Vertex {
        glm::vec3 position;
        glm::vec2 texCoord;
        glm::vec3 normal;
    };

    struct Material {
        std::string name;
        std::string diffuseTexPath;
        GLuint textureID = 0;
    };

    struct MeshGroup {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::string materialName;

        GLuint VAO = 0, VBO = 0, EBO = 0;
        void Setup();
    };

    class Model {
    public:
        Model();
        ~Model();

        bool LoadOBJ(const std::string& filename); // Parte 1: carregar .obj
        bool LoadMTL(const std::string& mtlFilename); // Parte 2: carregar .mtl
        void Draw(); // Parte 3: renderizar com texturas

    private:
        std::string directory;
        std::string mtlFileName;

        std::vector<MeshGroup> meshGroups;
        std::map<std::string, Material> materials;

        // Funções auxiliares
        void ProcessOBJLine(const std::string& line);
        void LoadTexture(Material& material);
    };

} // namespace Pool3D

#endif // MODEL_H
