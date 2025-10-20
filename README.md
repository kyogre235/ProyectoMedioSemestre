# Proyecto de medio semestre
 
Materia: Visualización

Profesor: Pedro Xavier Conta Romero

## INTEGRANTES:
 * Jacome Delgado Alejandro

## INDICACIONES

* El lenguaje de programacion utilizado es C/C++ con OpenGL y GLSL.
* También el uso de GLFW, GLEW y GLM al igual que todas las estructuras
  de datos que C++ tenga.
* El proyecto debe de ser ejecutado desde la terminal.
* El programa debera de visualizar el origen con un eje coordenado (X,Y,Z).
  (Utilizar GL LINES para marcar las direcciones de los ejes)
* El programa debera de leer un conjunto de OBJs los cuales se les proporcionaron junto a este documento.
* El programa debe de contener un shader que pinte un mapa de colores
  dependiendo del punto más bajo (azul) punto más alto (rojo) y el punto
  medio (amarillo)
* Se visualizará el primer objeto y utilizando las teclas o algún boton específico, el visualizador deber ́a de poder recorrer los otros modelos. El boton es a su critero.
* En la terminal debera de desplegar información de OBJ. Nombre, número
  de vertices y los mínimos y maximos de cada vertice.
* El modelo debe de visualizarse como una nube de puntos (GL POINTS),
  como un conjunto de líneas (GL LINES, GL LINE STRIP) y como una
  superficie (GL TRIANGLES, GL TRIANGLE STRIP, GL QUADS).
* El modelo debe poder girar en cualquier Eje coordenado al presionar
  alguna tecla que lo indique.

## COMPILACION Y EJECUCION DEL PROGRAMA

El proyecto se hizo tomando de base al codigo del cubo con textura procedural que se proporcionó
asi pues, seguira una forma de compilacion similar.

El proyecto requiere de la libreria GLFW, GLEW y GLM.

Entonces, asumiendo que estamos ne la carpeta del proyecto `../Proyecto1` tendremos el archivo
`Makefile` que contiene las instrucciones para compilar y ejecutar el programa. por lo tanto
solo debemos ejecutar:
```
make
```
Si es la primera vez que se ejecuta generara la siguiente salida:
```
g++ -std=c++11 -Wall -Wextra -c main.cpp -o main.o 
g++ -std=c++11 -Wall -Wextra -c src/shader.cpp -o src/shader.o 
g++ -std=c++11 -Wall -Wextra -c src/lector.cpp -o src/lector.o 
g++ -std=c++11 -Wall -Wextra -c src/model.cpp -o src/model.o 
g++ -std=c++11 -Wall -Wextra -c src/ventana.cpp -o src/ventana.o 
g++ -std=c++11 -Wall -Wextra -c extra/functions.cpp -o extra/functions.o 
g++ main.o src/shader.o src/lector.o src/model.o src/ventana.o extra/functions.o -o cubo -lglfw -lGL -lGLEW 
```
una vez compilado el programa, nos entregará un ejecutable llamado `./cubo`
al que simplemente debemos de ejecutar de la siguiente forma:
```
./cubo
```

## USO DEL PROGRAMA

Al ejecutar el programa, este cargará los obj aq que están guardados en la carpeta `/obj`
asi pues, el primer objeto que se ve es el que está en el archivo `Teapot.obj` y por 
defecto, inicia girando en el eje Y.

Para cambiar el modelo que se ve, se puede usar las siguientes teclas:
* `q` carga `Teapot.obj`
* `w` carga `bunny.obj`
* `e` carga `dragon.obj`
* `r` carga `Happy_Buddha.obj`
* `t` carga `Cube_triangles.obj`
* `y` carga `amogus.obj`

También podemos cambiar el modo en el que los objetos se renderizan, para esto
usamos las teclas numericas, asi, pues se pueden usar las siguientes teclas:
* `1` crea la vista con `GL_POINTS`
* `2` crea la vista con `GL_LINES`
* `3` crea la vista con `GL_LINE_STRIP`
* `4` crea la vista con `GL_TRIANGLES`
* `5` crea la vista con `GL_TRIANGLE_STRIP`
* `6` crea la vista con `GL_QUADS`

Otra accion que se puede hacer es cambiar la rotacion del modelo, 
para esto se utilizan las flechas del teclado de la siguiente forma:
* `Flecha arriba` rotacion en el eje Z
* `Flecha izquierda` rotacion en el eje X
* `Flecha derecha` rotacion en el eje Y

Por último, cabe mencionar que; se puede manejar el zoom de la ventana con la rueda
del raton y ademas con la tecla `p` se cargan los planos de las funciones que se pidieron
(para alternar entre las funciones presionar `p`)