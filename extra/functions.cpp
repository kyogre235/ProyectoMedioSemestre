#include "functions.h"
#include <vector>
#include <cmath>
#include <limits>
#include <iostream>

namespace Extra {

static inline float f_eval(int funcId, float x, float z) {
    switch (funcId) {
        case 1: // cos(x) + sin(z)
            return std::cos(x) + std::sin(z);
        case 2: // exp(-(x^2 + z^2)) para mantener valores acotados
            return std::exp(-(x*x + z*z));
        default:
            return 0.0f;
    }
}

bool generateFunctionMesh(int funcId,
                          int N,
                          float xmin, float xmax,
                          float zmin, float zmax,
                          Mesh &mesh) {
    if (N < 2) N = 2;

    const int rows = N;
    const int cols = N;
    const int vertexCount = rows * cols;

    mesh.vertices.clear();
    mesh.indices.clear();
    mesh.vertices.resize(vertexCount * 3);

    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::lowest();

    auto idx = [cols](int r, int c){ return r * cols + c; };

    // Paso en cada eje
    float dx = (xmax - xmin) / float(cols - 1);
    float dz = (zmax - zmin) / float(rows - 1);

    for (int r = 0; r < rows; ++r) {
        float z = zmin + r * dz;
        for (int c = 0; c < cols; ++c) {
            float x = xmin + c * dx;
            float y = f_eval(funcId, x, z);
            int base = idx(r,c) * 3;
            mesh.vertices[base + 0] = x;
            mesh.vertices[base + 1] = y;
            mesh.vertices[base + 2] = z;
            if (y < minY) minY = y;
            if (y > maxY) maxY = y;
        }
    }

    // Indices (dos triangulos por celda)
    mesh.indices.reserve((rows - 1) * (cols - 1) * 6);
    for (int r = 0; r < rows - 1; ++r) {
        for (int c = 0; c < cols - 1; ++c) {
            unsigned int i0 = idx(r, c);
            unsigned int i1 = idx(r, c + 1);
            unsigned int i2 = idx(r + 1, c);
            unsigned int i3 = idx(r + 1, c + 1);
            // triángulo 1: i0, i2, i1
            mesh.indices.push_back(i0);
            mesh.indices.push_back(i2);
            mesh.indices.push_back(i1);
            // triángulo 2: i1, i2, i3
            mesh.indices.push_back(i1);
            mesh.indices.push_back(i2);
            mesh.indices.push_back(i3);
        }
    }

    // AABB, centro, radio
    glm::vec3 bbMin(xmin, minY, zmin);
    glm::vec3 bbMax(xmax, maxY, zmax);
    mesh.bbMin = bbMin;
    mesh.bbMax = bbMax;
    mesh.center = (bbMin + bbMax) * 0.5f;
    glm::vec3 extents = (bbMax - bbMin) * 0.5f;
    mesh.radius = std::max(extents.x, std::max(extents.y, extents.z));

    // Nombre
    mesh.name = (funcId == 1) ? std::string("f(x,z)=cos(x)+sin(z)")
                              : std::string("f(x,z)=exp(-(x^2+z^2))");

    // Mensaje informativo (dominio, min, max)
    std::cout << "Malla de función generada: " << mesh.name << "\n"
              << " - Dominio x in [" << xmin << ", " << xmax << "], z in [" << zmin << ", " << zmax << "]\n"
              << " - Vertices: " << mesh.vertices.size() / 3 << "\n"
              << " - Min y: " << minY << ", Max y: " << maxY << std::endl;

    return true;
}

} // namespace Extra
