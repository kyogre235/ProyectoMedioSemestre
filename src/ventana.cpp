#include "../header/ventana.h"
#include <iostream>
#include <cmath>

Ventana::Ventana(){
    width = 900;
    height = 900;
}

Ventana::Ventana(GLuint w, GLuint h){
    width = w;
    height = h;
}


void Ventana::initGLFW()
{
        if (!glfwInit()) {
        std::cerr << "Error al inicializar GLFW" << std::endl;
        return ;
    }

        // Crear una ventana
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Cubo Rotando", nullptr, nullptr);
    if (!window) {
        std::cerr << "Error al crear la ventana" << std::endl;
        glfwTerminate();
        return;
    }

    glfwMakeContextCurrent(window);

    // Registrar el puntero de usuario y callback de scroll para poder hacer zoom con la ruedita del raton
    glfwSetWindowUserPointer(window, this);
    glfwSetScrollCallback(window, Ventana::scrollCallback);
}

// Inicializar GLEW
void Ventana::initGLEW(){
    if (glewInit() != GLEW_OK) {
        std::cerr << "Error al inicializar GLEW" << std::endl;
        return;
    }
    std::cout << "Inicialización Correcta de GLEW y GLFW" << std::endl;
    // Habilitar depth testing y face culling
    glEnable(GL_DEPTH_TEST);  // Prueba de profundidad
    glEnable(GL_CULL_FACE);   // Habilitar culling de caras
    glCullFace(GL_BACK);      // Culling de caras traseras
    glFrontFace(GL_CCW);      // Las caras frontales son las que tienen vértices en sentido antihorario
}

void Ventana::initModels(Model* m)
{
    model = m;
    model->initModel();
    setupModelBindings();
}

void Ventana::initViewProyection(){
     // Ajustar la camara y la proyeccion al tamaño del modelo y al tamaño de la ventana
    camFovRad = glm::radians(45.0f);

    // Obtener bounding sphere del modelo
    camCenter = model->getCenter();
    float radius = model->getRadius();
    if (radius <= 0.0f) radius = 1.0f;

    // Distancia para encajar el objeto completo usando el FOV vertical (deje un margen extra por si acaso)
    camEyeDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));
    camDistance = radius / tanf(camFovRad * 0.3f);
    // Margen para evitar recortes
    camDistance *= 1.2f;

    // Actualizar matrices con el estado actual de la camara
    updateCameraMatrices();

    // Inicializar ejes despues de tener el modelo cargado
    initAxes();
}

void Ventana::updateCameraMatrices(){
    float aspect = static_cast<float>(width) / static_cast<float>(height);
    float radius = model ? model->getRadius() : 1.0f;
    if (radius <= 0.0f) radius = 1.0f;
    // la posicion de la camara, el "ojo" por donde vemos con estos calculos hacemos
    // que la camara siempre vea a donde nosotros queremos
    glm::vec3 eye = camCenter + glm::normalize(camEyeDir) * camDistance;
    view = glm::lookAt(eye, camCenter, glm::vec3(0.0f,1.0f,0.0f));

    // Planos de recorte dependientes de la distancia asi los modelos no pueden
    // tapar la camara si hacemos mucho zoom o si nos alejamos
    float nearPlane = glm::max(0.01f, camDistance - radius * 2.0f);
    float farPlane  = camDistance + radius * 4.0f;
    projection = glm::perspective(camFovRad, aspect, nearPlane, farPlane);
}

void Ventana::scrollCallback(GLFWwindow* win, double /*xoffset*/, double yoffset){
    // Recuperar la posicion del raton en la ventana
    auto* self = reinterpret_cast<Ventana*>(glfwGetWindowUserPointer(win));
    if (!self || !self->model) return;

    // Ajustar distancia con base exponencial para zoom suave si ponemos lineal
    // es medio incomoda
    float radius = self->model->getRadius();
    if (radius <= 0.0f) radius = 1.0f;
    float minDist = radius * 0.1f;
    float maxDist = radius * 50.0f;

    // yoffset > 0 -> acercar (disminuir distancia)
    float factor = std::expf(static_cast<float>(-yoffset) * 0.2f);
    self->camDistance *= factor;
    if (self->camDistance < minDist) self->camDistance = minDist;
    if (self->camDistance > maxDist) self->camDistance = maxDist;

    self->updateCameraMatrices();
}

void Ventana::initAxes(){
    // Longitud de los ejes basada en el tamaño del modelo
    // (en realidad es sobre la esfera que usamos para los calculos)
    float L = glm::max(1.0f, model ? model->getRadius() * 1.5f : 1.0f);

    // Cada vértice: vec3 pos + vec3 color
    float axisVertices[] = {
        // X en rojo
        0.0f, 0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        L,    0.0f, 0.0f,   1.0f, 0.0f, 0.0f,
        // Y en verde
        0.0f, 0.0f, 0.0f,   0.0f, 1.0f, 0.0f,
        0.0f, L,    0.0f,   0.0f, 1.0f, 0.0f,
        // Z en azul
        0.0f, 0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
        0.0f, 0.0f, L,      0.0f, 0.0f, 1.0f
    };

    if (axisVAO == 0) {
        glGenVertexArrays(1, &axisVAO);
        glGenBuffers(1, &axisVBO);
    }
    glBindVertexArray(axisVAO);
    glBindBuffer(GL_ARRAY_BUFFER, axisVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axisVertices), axisVertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    if (!axisShader) {
        axisShader = new Shader("./shader/axis.vert", "./shader/axis.frag");
    }
}

void Ventana::render(){
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        // Procesar hotkeys para cambiar de modelo (qwerty)
        processModelHotkeys();
        // Procesar tecla extra (P) para superficies de funciones
        processExtraHotkey();

        // Cambiar el modo de dibujo con las teclas numéricas
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) model->setDrawMode(GL_POINTS);
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) model->setDrawMode(GL_LINES);
        if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) model->setDrawMode(GL_LINE_STRIP);
        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) model->setDrawMode(GL_TRIANGLES);
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) model->setDrawMode(GL_TRIANGLE_STRIP);
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) model->setDrawMode(GL_QUADS);

        // Cambiar el eje de rotación con flechas:
        // Flecha arriba -> eje Z, izquierda -> eje X, derecha -> eje Y
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    model->setRotationAxis(glm::vec3(0.0f, 0.0f, 1.0f));
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  model->setRotationAxis(glm::vec3(1.0f, 0.0f, 0.0f));
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) model->setRotationAxis(glm::vec3(0.0f, 1.0f, 0.0f));

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Dibujar ejes con GL_LINES en el origen
        if (axisShader && axisVAO) {
            axisShader->use();
            glm::mat4 I(1.0f);
            axisShader->setMat4x4("model", I);
            axisShader->setMat4x4("view", view);
            axisShader->setMat4x4("projection", projection);
            glBindVertexArray(axisVAO);
            glDrawArrays(GL_LINES, 0, 6);
        }

        // Dibujar el modelo
        model->renderModel(view, projection);

        glfwSwapBuffers(window);
        glfwPollEvents();
        update();
    }
}

void Ventana::update(){
    // Calcular la rotación del cubo
    model->updateModel(glfwGetTime());
}

void Ventana::idel(){}

void Ventana::setupModelBindings(){
    // Asignar modelos a las teclas de la primera fila: q w e r t y
    modelBindings[GLFW_KEY_Q] = "./obj/Teapot.obj";
    modelBindings[GLFW_KEY_W] = "./obj/bunny.obj";
    modelBindings[GLFW_KEY_E] = "./obj/dragon.obj";
    modelBindings[GLFW_KEY_R] = "./obj/Happy_Buddha.obj";
    modelBindings[GLFW_KEY_T] = "./obj/Cube_Triangles.obj";
    modelBindings[GLFW_KEY_Y] = "./obj/amogus.obj";


    // Inicializar estados previos a no presionado
    for (const auto &kv : modelBindings) {
        prevPressed[kv.first] = false;
    }
}

void Ventana::processModelHotkeys(){
    for (auto &kv : modelBindings) {
        int key = kv.first;
        const std::string &path = kv.second;
        int state = glfwGetKey(window, key);
        bool isPressed = (state == GLFW_PRESS);
        bool wasPressed = prevPressed[key];
        if (isPressed && !wasPressed) {
            // Flanco de subida: recargar modelo
            if (model->reloadOBJ(path)) {
                // Reajustar cámara y ejes al nuevo modelo
                camCenter = model->getCenter();
                float radius = model->getRadius();
                if (radius <= 0.0f) radius = 1.0f;
                camDistance = radius / tanf(camFovRad * 0.3f);
                camDistance *= 1.2f;
                updateCameraMatrices();

                // Actualizar ejes en función del nuevo tamaño
                disposeAxes();
                initAxes();

                std::cout << "Modelo cambiado con tecla: " << key << " -> " << path << std::endl;
            } else {
                std::cerr << "No se pudo cambiar al modelo: " << path << std::endl;
            }
        }
        prevPressed[key] = isPressed;
    }
}

void Ventana::disposeAxes(){
    if (axisVAO) {
        glDeleteVertexArrays(1, &axisVAO);
        axisVAO = 0;
    }
    if (axisVBO) {
        glDeleteBuffers(1, &axisVBO);
        axisVBO = 0;
    }
    if (axisShader) {
        axisShader->terminate();
        delete axisShader;
        axisShader = nullptr;
    }
}

void Ventana::processExtraHotkey(){
    int state = glfwGetKey(window, GLFW_KEY_P);
    bool isPressed = (state == GLFW_PRESS);
    if (isPressed && !prevP) {
        // Alternar entre las funciones que se ponian como punto extra
        currentFuncId = (currentFuncId == 1) ? 2 : 1;
        if (model->loadFunctionSurface(currentFuncId)) {
            // Reajustar cámara y ejes al nuevo "modelo" generado
            camCenter = model->getCenter();
            float radius = model->getRadius();
            if (radius <= 0.0f) radius = 1.0f;
            camDistance = radius / tanf(camFovRad * 0.3f);
            camDistance *= 1.2f;
            updateCameraMatrices();
            disposeAxes();
            initAxes();
            std::cout << "Superficie de función activada (id=" << currentFuncId << ") con tecla P" << std::endl;
        } else {
            std::cerr << "No se pudo generar la superficie de función" << std::endl;
        }
    }
    prevP = isPressed;
}

void Ventana::finish(){
    disposeAxes();
    model->finish();
}