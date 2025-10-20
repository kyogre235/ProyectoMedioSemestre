#include "../header/model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstring>
#include <limits>
#include "../header/lector.h"
#include "../header/mesh.h"
#include "../extra/functions.h"

Model::Model(){}

void Model::initModel()
{
    modelmat = glm::mat4(1.0f);
    // Cargamos la tetera para que sea dibujada por defecto
    if (!loadOBJ("./obj/Teapot.obj")) {
        //tiramos error si no encuentra el obj
        std::cerr << "Fallo al cargar OBJ. Se aborta la inicialización del modelo." << std::endl;
        return;
    }

    shader = new Shader("./shader/cubo.vert","./shader/cubo.frag");

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Vertex layout: only positions (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

void Model::updateModel(float timeValue){
    // Usar tiempo absoluto para un ángulo lineal y reconstruir la matriz desde identidad.
    angle = timeValue * glm::radians(7.0f); // 7.0 grados por segundo

    // Normalizar el eje de rotación, por defecto gira sobre el eje y
    glm::vec3 axis = rotationAxis;
    if (glm::length(axis) < 1e-6f) axis = glm::vec3(0.0f, 1.0f, 0.0f);
    axis = glm::normalize(axis);
    // construimos la matriz que aplica la transformacion
    modelmat = glm::rotate(glm::mat4(1.0f), angle, axis);
}

void Model::renderModel(glm::mat4 view, glm::mat4 projection){
    shader->use();
    // creamos las matrice que nos van a ayudar a aplicar las transformaciones a los vertrices
    shader->setMat4x4("model", modelmat);
    shader->setMat4x4("view", view);
    shader->setMat4x4("projection", projection);
    // color placeholder que luego cambiaremos por el degradado del shader
    shader->setVec3("baseColor", glm::vec3(0.9f, 0.8f, 0.6f));

    // límites verticales del modelo para el gradiente
    shader->setFloat("uMinY", bbMin.y);
    shader->setFloat("uMaxY", bbMax.y);

    glBindVertexArray(VAO);

    // Dibujar según el modo actual. Para GL_POINTS usamos los vértices directamente.
    if (drawMode == GL_POINTS) {
        GLsizei count = static_cast<GLsizei>(vertices.size() / 3);
        glDrawArrays(GL_POINTS, 0, count);
    } else if (drawMode == GL_QUADS) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    } else {
        glDrawElements(drawMode, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);
    }
}

void Model::finish(){
    shader->terminate();
    delete(shader);

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

// Aplicar datos de malla al modelo (solo CPU)
void Model::applyMesh(const Mesh &mesh) {
    vertices = mesh.vertices;
    indices = mesh.indices;
    bbMin = mesh.bbMin;
    bbMax = mesh.bbMax;
    center = mesh.center;
    radius = mesh.radius;
}

// Carga de OBJ delegada a Lector usando Mesh
bool Model::loadOBJ(const std::string &path) {
    Mesh mesh;
    if (!Lector::loadOBJ(path, mesh)) return false;
    applyMesh(mesh);
    return true;
}

bool Model::reloadOBJ(const std::string &path) {
    // Cargar nuevos datos al CPU vía Lector -> Mesh
    Mesh mesh;
    if (!Lector::loadOBJ(path, mesh)) {
        return false;
    }

    // Reemplazar datos en CPU
    applyMesh(mesh);

    // Resubir a GPU reutilizando los buffers existentes
    uploadCPUToGPU();

    // Reiniciar transformaciones para el nuevo modelo
    modelmat = glm::mat4(1.0f);
    angle = 0.0f;

    return true;
}

void Model::uploadCPUToGPU(){
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Asegurar configuracion de atributos (en caso de que el VAO se resetee fuera)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
}

bool Model::loadFunctionSurface(int funcId) {
    Mesh mesh;
    const int N = 100; // 100x100 vertices
    const float xmin = -3.1415926f;
    const float xmax = 3.1415926f;
    const float zmin = -3.1415926f;
    const float zmax = 3.1415926f;

    if (!Extra::generateFunctionMesh(funcId, N, xmin, xmax, zmin, zmax, mesh)) {
        std::cerr << "No se pudo generar la malla de funcion (id=" << funcId << ")" << std::endl;
        return false;
    }

    // Aplicar y subir a GPU
    applyMesh(mesh);
    uploadCPUToGPU();

    // Reiniciar transformaciones
    modelmat = glm::mat4(1.0f);
    angle = 0.0f;

    return true;
}