#include "../header/lector.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <limits>

namespace Lector {

bool loadOBJ(const std::string &path,
             std::vector<float> &vertices,
             std::vector<unsigned int> &indices,
             glm::vec3 &bbMin,
             glm::vec3 &bbMax,
             glm::vec3 &center,
             float &radius) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo OBJ: " << path << std::endl;
        return false;
    }

    std::vector<glm::vec3> temp_positions;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;
        if (prefix == "v") {
            glm::vec3 pos;
            iss >> pos.x >> pos.y >> pos.z;
            temp_positions.push_back(pos);
        } else if (prefix == "f") {
            // espera 3 3 token para generar los triangulos. Maneja a los token v, v/vt, v//vn, v/vt/vn
            for (int k = 0; k < 3; ++k) {
                std::string token;
                iss >> token;
                if (token.empty()) break;
                // Consigue la posiccion antes del primer '/'
                size_t slashPos = token.find('/');
                std::string indexStr = (slashPos == std::string::npos) ? token : token.substr(0, slashPos);
                int idx = std::stoi(indexStr);
                // acomoda edl index pues opengl lo llama 0 y obj lo llama 1
                indices.push_back(static_cast<unsigned int>(idx - 1));
            }
        }
    }
    file.close();

    if (temp_positions.empty() || indices.empty()) {
        std::cerr << "El OBJ no contiene posiciones o indices: " << path << std::endl;
        return false;
    }

    // transforma los vec3 en arreglos de float
    vertices.resize(temp_positions.size() * 3);
    for (size_t i = 0; i < temp_positions.size(); ++i) {
        vertices[i*3 + 0] = temp_positions[i].x;
        vertices[i*3 + 1] = temp_positions[i].y;
        vertices[i*3 + 2] = temp_positions[i].z;
    }

    // calculamos la caja que contiene al objeto, sirve para hacer mas faciles
    // los calculos de la camara y otras cosas (lo saque de internet el algortimo,XD) solo lo implemente
    bbMin = glm::vec3(std::numeric_limits<float>::max());
    bbMax = glm::vec3(std::numeric_limits<float>::lowest());
    for (const auto &p : temp_positions) {
        bbMin = glm::min(bbMin, p);
        bbMax = glm::max(bbMax, p);
    }
    center = (bbMin + bbMax) * 0.5f;
    glm::vec3 extents = (bbMax - bbMin) * 0.5f;
    // mismo concepto que la caja, pero en una esfera,aunque calcualndola
    // asi es mas grande de lo que se nesesita
    radius = std::max(extents.x, std::max(extents.y, extents.z));

    return true;
}

// lo que hacemos guardar nuestros obj en un mesh que definimos para que
// sea mas facil acceder a los datos en las otras clases
bool loadOBJ(const std::string &path, Mesh &mesh) {
    std::vector<float> verts;
    std::vector<unsigned int> inds;
    glm::vec3 bbMin, bbMax, center;
    float radius = 1.0f;

    bool ok = loadOBJ(path, verts, inds, bbMin, bbMax, center, radius);
    if (!ok) return false;

    mesh.vertices = std::move(verts);
    mesh.indices = std::move(inds);
    mesh.bbMin = bbMin;
    mesh.bbMax = bbMax;
    mesh.center = center;
    mesh.radius = radius;

    // Extraer nombre de archivo
    mesh.name = path;
    size_t pos = mesh.name.find_last_of("/\\");
    if (pos != std::string::npos) mesh.name = mesh.name.substr(pos + 1);

    // Imprimir información del OBJ aquí para centralizar la responsabilidad
    std::cout << "OBJ cargado: " << mesh.name << '\n'
              << " - Vertices: " << mesh.vertices.size() / 3 << '\n'
              << " - Min (x,y,z): (" << mesh.bbMin.x << ", " << mesh.bbMin.y << ", " << mesh.bbMin.z << ")\n"
              << " - Max (x,y,z): (" << mesh.bbMax.x << ", " << mesh.bbMax.y << ", " << mesh.bbMax.z << ")" << std::endl;

    return true;
}

} // namespace Lector
