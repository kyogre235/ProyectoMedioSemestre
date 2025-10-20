#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "mesh.h"

namespace Lector {
// Carga un archivo OBJ (posiciones y caras triangulares) y entrega:
// - vertices: arreglo plano de floats (x,y,z)
// - indices: indices de posiciones (0-based)
// - bbMin, bbMax: AABB
// - center: centro del AABB
// - radius: radio de la esfera envolvente
// Retorna true si la carga fue exitosa.
bool loadOBJ(const std::string &path,
             std::vector<float> &vertices,
             std::vector<unsigned int> &indices,
             glm::vec3 &bbMin,
             glm::vec3 &bbMax,
             glm::vec3 &center,
             float &radius);

// Overload orientado a modularidad: llena una estructura Mesh
bool loadOBJ(const std::string &path, Mesh &mesh);
}
