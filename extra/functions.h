#pragma once
#include <string>
#include <glm/glm.hpp>
#include "../header/mesh.h"

// Genera una malla de un plano NxN centrado en el origen aplicando dos funciones de la forma y = f(x,z)
// funcId: 1 -> f(x,z) = cos(x) + sin(z)
//          2 -> f(x,z) = exp(-(x^2 + z^2))
// dominio: x en [xmin, xmax], z en [zmin, zmax]
// Rellena el Mesh con vertices, indices, bbMin, bbMax, center, radius y un nombre.
// Ademas imprime en terminal el dominio y el mínimo/maximo de la funcion evaluada sobre la malla.
namespace Extra {
bool generateFunctionMesh(int funcId,
                          int N,
                          float xmin, float xmax,
                          float zmin, float zmax,
                          Mesh &mesh);
}
