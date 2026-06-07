#include "Application.h"

/*
 * Archivo: main.cpp
 * Descripción:
 * Punto de entrada principal del proyecto final.
 *
 * Inicializa la aplicación, ejecuta el ciclo principal del programa
 * y libera los recursos al finalizar.
 */

int main(int argc, char **argv) {
    // Inicializa la ventana principal y los recursos base del proyecto.
    init(800, 700, "ProyectoFinal - Sistema de Escenas", false);

    // Ejecuta el ciclo principal de actualización y renderizado.
    applicationLoop();

    // Libera memoria, modelos, texturas, audio y recursos de OpenGL.
    destroy();

    return 0;
}