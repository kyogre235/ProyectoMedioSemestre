#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

// Contenedor de datos de malla (CPU) independiente del render
struct Mesh {
    std::vector<float> vertices;           // posiciones (x,y,z)
    std::vector<unsigned int> indices;     // índices 0-based
    glm::vec3 bbMin{0.0f};
    glm::vec3 bbMax{0.0f};
    glm::vec3 center{0.0f};
    float radius{1.0f};
    std::string name;                      // nombre del recurso (archivo)
};
