#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "../header/model.h"
#include "../header/shader.h"
#include <unordered_map>
#include <string>

class Ventana{
    public:
        GLuint width, height;
        GLFWwindow* window;
        glm::mat4 view, projection;

        Model *model;

        Ventana();
        Ventana(GLuint w, GLuint h);
        void initGLFW();
        void initGLEW();
        void initModels(Model* m);
        
        void initViewProyection();
        void render();
        void update();
        void idel();
        void finish();

    private:
        // Estado de cámara para permitir zoom con la ruedita
        glm::vec3 camCenter{0.0f};
        glm::vec3 camEyeDir{1.0f, 1.0f, 1.0f};
        float camDistance = 3.0f;
        float camFovRad = glm::radians(45.0f);
        void updateCameraMatrices();
        static void scrollCallback(GLFWwindow* win, double xoffset, double yoffset);

        // Ejes cartesianos dibujados con GL_LINES
        GLuint axisVAO = 0, axisVBO = 0;
        Shader* axisShader = nullptr;
        void initAxes();
        void disposeAxes();

        // Mapeo de teclas a rutas de OBJ
        std::unordered_map<int, std::string> modelBindings;
        std::unordered_map<int, bool> prevPressed;
        void setupModelBindings();
        void processModelHotkeys();

        // Extra: alternar superficies de funciones con tecla P
        bool prevP = false;
        int currentFuncId = 1; // alterna entre 1 y 2
        void processExtraHotkey();
};