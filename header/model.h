#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "../header/shader.h"

struct Mesh; // forward declaration

class Model{
public:

    Shader* shader;
    std::vector<float> vertices; // solo los vertices de posicion (x,y,z)
    std::vector<unsigned int> indices;
    float angle;
    glm::mat4 modelmat;
    GLuint VAO, VBO, EBO;
    // Buffers para ejes
    GLuint axesVAO = 0, axesVBO = 0;

    // Modo de dibujo actual (GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES, GL_TRIANGLE_STRIP, ...)
    GLenum drawMode = GL_TRIANGLES;

    // Eje de rotación configurable en tiempo real
    void setRotationAxis(const glm::vec3 &axis) { rotationAxis = axis; }

    Model();

    void initModel();
    void renderModel(glm::mat4 view, glm::mat4 projection);
    void updateModel(float timeValue);
    void finish();

    // Cambiar el modo de dibujo en tiempo real
    void setDrawMode(GLenum mode) { drawMode = mode; }
    GLenum getDrawMode() const { return drawMode; }

    // gets pasa conseguir el centro y el radio de la esfera que definimos
    glm::vec3 getCenter() const { return center; }
    float getRadius() const { return radius; }

    // Recargar un nuevo OBJ en tiempo real reutilizando los buffers
    bool reloadOBJ(const std::string &path);

    // Carga de superficies de funciones extra
    bool loadFunctionSurface(int funcId);

private:
    //carga el obj del que se la pase la direccion.
    bool loadOBJ(const std::string &path);
    // Aplicar datos de una malla al modelo (solo CPU, sin crear buffers)
    void applyMesh(const Mesh &mesh);
    void uploadCPUToGPU();

    // datos de los modelos que calculamos nosotros (el radio y las coordenadas de la caja)
    glm::vec3 bbMin{0.0f}, bbMax{0.0f};
    glm::vec3 center{0.0f};
    float radius{1.0f};

    // Estado del eje de rotación (por defecto, eje Y)
    glm::vec3 rotationAxis{0.0f, 1.0f, 0.0f};
};