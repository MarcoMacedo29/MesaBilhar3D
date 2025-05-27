// main.cpp

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>


// Janela
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 500;

// Câmera orbital - parâmetros
float camDistance = 5.0f;
float camYaw = 0.0f;    // horizontal angulo
float camPitch = 20.0f; // vertical angulo
bool leftMousePressed = false;
double lastX, lastY;

// Funções para callback
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camDistance -= (float)yoffset * 0.5f;
    if (camDistance < 2.0f) camDistance = 2.0f;
    if (camDistance > 20.0f) camDistance = 20.0f;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            leftMousePressed = true;
            glfwGetCursorPos(window, &lastX, &lastY);
        }
        else if (action == GLFW_RELEASE) {
            leftMousePressed = false;
        }
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos) {
    if (leftMousePressed) {
        float sensitivity = 0.3f;
        float dx = (float)(xpos - lastX);
        float dy = (float)(ypos - lastY);

        camYaw += dx * sensitivity;
        camPitch -= dy * sensitivity;

        if (camPitch > 89.0f) camPitch = 89.0f;
        if (camPitch < -89.0f) camPitch = -89.0f;

        lastX = xpos;
        lastY = ypos;
    }
}

// Define vértices do paralelepípedo (mesa)
float vertices[] = {
    // Positions           // Colors (R,G,B)
    // Face frontal (z = 0.5)
    -1.0f, -0.5f,  0.5f,  1,0,0, // vermelho
     1.0f, -0.5f,  0.5f,  1,0,0,
     1.0f,  0.5f,  0.5f,  1,0,0,
    -1.0f,  0.5f,  0.5f,  1,0,0,

    // Face traseira (z = -0.5)
    -1.0f, -0.5f, -0.5f,  0,1,0, // verde
     1.0f, -0.5f, -0.5f,  0,1,0,
     1.0f,  0.5f, -0.5f,  0,1,0,
    -1.0f,  0.5f, -0.5f,  0,1,0,

    // Face esquerda (x = -1)
    -1.0f, -0.5f, -0.5f,  0,0,1, // azul
    -1.0f, -0.5f,  0.5f,  0,0,1,
    -1.0f,  0.5f,  0.5f,  0,0,1,
    -1.0f,  0.5f, -0.5f,  0,0,1,

    // Face direita (x = 1)
     1.0f, -0.5f, -0.5f,  1,1,0, // amarelo
     1.0f, -0.5f,  0.5f,  1,1,0,
     1.0f,  0.5f,  0.5f,  1,1,0,
     1.0f,  0.5f, -0.5f,  1,1,0,

     // Face topo (y = 0.5)
     -1.0f,  0.5f, -0.5f,  1,0,1, // magenta
      1.0f,  0.5f, -0.5f,  1,0,1,
      1.0f,  0.5f,  0.5f,  1,0,1,
     -1.0f,  0.5f,  0.5f,  1,0,1,

     // Face baixo (y = -0.5)
     -1.0f, -0.5f, -0.5f,  0,1,1, // ciano
      1.0f, -0.5f, -0.5f,  0,1,1,
      1.0f, -0.5f,  0.5f,  0,1,1,
     -1.0f, -0.5f,  0.5f,  0,1,1,
};

unsigned int indices[] = {
    0,1,2, 2,3,0,       // frente
    4,5,6, 6,7,4,       // trás
    8,9,10, 10,11,8,    // esquerda
    12,13,14, 14,15,12, // direita
    16,17,18, 18,19,16, // topo
    20,21,22, 22,23,20  // baixo
};

// Shader simples - vertex shader
const char* vertexShaderSource = R"(
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main(){
    gl_Position = projection * view * model * vec4(aPos,1.0);
    ourColor = aColor;
}
)";

// Shader simples - fragment shader
const char* fragmentShaderSource = R"(
#version 330 core
in vec3 ourColor;
out vec4 FragColor;

void main(){
    FragColor = vec4(ourColor,1.0);
}
)";


// Função para compilar shader e checar erros
unsigned int CompileShader(unsigned int type, const char* source) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cout << "Erro shader: " << infoLog << std::endl;
    }
    return shader;
}

// Função para criar programa shader
unsigned int CreateShaderProgram(const char* vertexSrc, const char* fragSrc) {
    unsigned int vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSrc);
    unsigned int fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragSrc);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    int success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cout << "Erro link shader program: " << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}


int main() {
    // Inicializar GLFW
    if (!glfwInit()) {
        std::cout << "Falha a inicializar GLFW" << std::endl;
        return -1;
    }

    // Criar janela
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Mesa de Bilhar - Passo 1", NULL, NULL);
    if (!window) {
        std::cout << "Falha a criar janela GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Callbacks
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);

    // Inicializar GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        std::cout << "Falha a inicializar GLEW" << std::endl;
        return -1;
    }

    // Configurar viewport
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Criar shader program
    unsigned int shaderProgram = CreateShaderProgram(vertexShaderSource, fragmentShaderSource);

    // Setup VAO e VBO
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // posição
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // cor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);

    // Ativar profundidade
    glEnable(GL_DEPTH_TEST);

    // Loop principal
    while (!glfwWindowShouldClose(window)) {
        // Limpar tela
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Calcular posição da câmera orbital
        float camX = camDistance * cos(glm::radians(camPitch)) * sin(glm::radians(camYaw));
        float camY = camDistance * sin(glm::radians(camPitch));
        float camZ = camDistance * cos(glm::radians(camPitch)) * cos(glm::radians(camYaw));
        glm::vec3 cameraPos = glm::vec3(camX, camY, camZ);
        glm::vec3 target = glm::vec3(0, 0, 0);
        glm::vec3 up = glm::vec3(0, 1, 0);

        glm::mat4 view = glm::lookAt(cameraPos, target, up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 model = glm::mat4(1.0f);

        // Passar matrizes para o shader
        unsigned int modelLoc = glGetUniformLocation(shaderProgram, "model");
        unsigned int viewLoc = glGetUniformLocation(shaderProgram, "view");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "projection");

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

        // Desenhar mesa
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        // --- MINIMAPA ---
        // Viewport pequeno no canto superior direito
        int miniSize = 200;
        glViewport(SCR_WIDTH - miniSize - 10, SCR_HEIGHT - miniSize - 10, miniSize, miniSize);
        glClear(GL_DEPTH_BUFFER_BIT); // limpar depth para desenhar minimapa

        // Câmera top-down para minimapa
        glm::vec3 miniCamPos = glm::vec3(0, 10, 0);
        glm::vec3 miniTarget = glm::vec3(0, 0, 0);
        glm::vec3 miniUp = glm::vec3(0, 0, -1); // para olhar "para frente"

        glm::mat4 miniView = glm::lookAt(miniCamPos, miniTarget, miniUp);
        glm::mat4 miniProjection = glm::ortho(-2.0f, 2.0f, -2.0f, 2.0f, 0.1f, 20.0f);

        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &miniView[0][0]);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, &miniProjection[0][0]);

        glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(unsigned int), GL_UNSIGNED_INT, 0);

        // Voltar viewport normal
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Limpar buffers
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}