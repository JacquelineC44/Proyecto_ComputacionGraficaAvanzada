#pragma once

/*
 * Archivo: Application.h
 * Descripción:
 * Declara las funciones principales de control de la aplicación.
 *
 * Este archivo contiene la interfaz general para inicializar el proyecto,
 * ejecutar el ciclo principal, procesar entradas del usuario, manejar
 * callbacks de GLFW y consultar funciones auxiliares usadas por la lógica
 * del juego.
 */

#include <string>

// Declaración adelantada para evitar incluir GLFW completo en el header.
struct GLFWwindow;


// =====================
// CICLO PRINCIPAL DE APLICACIÓN
// =====================

/*
 * Inicializa la ventana, OpenGL, recursos base, escenas, texturas,
 * modelos, audio y demás componentes principales del proyecto.
 */
void init(
    int width,
    int height,
    std::string strTitle,
    bool bFullScreen
);

/*
 * Libera los recursos utilizados por la aplicación antes de cerrar.
 */
void destroy();

/*
 * Procesa las entradas generales del usuario.
 *
 * Retorna:
 * - true si la aplicación debe continuar.
 * - false si se solicita cerrar el programa.
 */
bool processInput(bool continueApplication = true);

/*
 * Ejecuta el ciclo principal del programa.
 *
 * Dentro de este ciclo se actualiza el tiempo, se procesan entradas,
 * se actualizan escenas y se renderiza el contenido en pantalla.
 */
void applicationLoop();


// =====================
// CALLBACKS DE GLFW
// =====================

/*
 * Se ejecuta cuando cambia el tamaño de la ventana.
 */
void reshapeCallback(
    GLFWwindow* Window,
    int widthRes,
    int heightRes
);

/*
 * Se ejecuta cuando se presiona o libera una tecla.
 */
void keyCallback(
    GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mode
);

/*
 * Se ejecuta cuando se mueve el mouse.
 */
void mouseCallback(
    GLFWwindow* window,
    double xpos,
    double ypos
);

/*
 * Se ejecuta cuando se presiona o libera un botón del mouse.
 */
void mouseButtonCallback(
    GLFWwindow* window,
    int button,
    int state,
    int mod
);


// =====================
// FUNCIONES AUXILIARES DE JUEGO
// =====================

/*
 * Verifica si Perry se encuentra cerca de la entrada o zona del casino.
 */
bool jugadorCercaDelCasino();
bool jugadorCercaDelBoliche();

//salida casino
bool jugadorCercaSalidaCasino();
bool jugadorCercaSalidaBoliche();
/*
 * Convierte un valor de tiempo en segundos a formato de texto.
 */
std::string formatoTiempo(float tiempo);