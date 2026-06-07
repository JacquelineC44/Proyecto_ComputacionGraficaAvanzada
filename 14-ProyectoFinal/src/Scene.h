#pragma once

/*
 * Archivo: Scene.h
 * Descripción:
 * Define la clase base abstracta Scene, utilizada como plantilla para
 * todas las escenas del proyecto.
 *
 * Esta clase establece una interfaz común para inicializar, cerrar,
 * actualizar y renderizar escenas dentro del sistema modular del proyecto.
 *
 * Cualquier escena específica, como PuebloScene, CasinoScene o BolosScene,
 * debe heredar de esta clase e implementar sus funciones principales.
 */

#include <glm/glm.hpp>

// =====================
// CLASE BASE DE ESCENAS
// =====================

class Scene {
public:
    /*
     * Destructor virtual.
     *
     * Permite destruir correctamente objetos de clases derivadas
     * cuando se manejan mediante punteros a la clase base Scene.
     */
    virtual ~Scene() {}

    /*
     * Se ejecuta al entrar a una escena.
     *
     * Puede utilizarse para inicializar recursos, configurar la cámara,
     * cargar modelos o preparar variables propias de la escena.
     */
    virtual void onEnter() {}

    /*
     * Se ejecuta al salir de una escena.
     *
     * Puede utilizarse para liberar recursos, detener sonidos,
     * limpiar variables temporales o guardar estados antes del cambio.
     */
    virtual void onExit() {}

    /*
     * Actualiza la lógica interna de la escena.
     *
     * Parámetros:
     * - dt: delta time, tiempo transcurrido entre frames.
     *
     * Puede usarse para movimiento, animaciones, colisiones,
     * inteligencia artificial o eventos internos.
     */
    virtual void update(double dt) {}

    /*
     * Renderiza los elementos visuales de la escena.
     *
     * Parámetros:
     * - projection: matriz de proyección de la cámara.
     * - view: matriz de vista de la cámara.
     *
     * Es una función virtual pura, por lo que cada escena derivada
     * debe implementar obligatoriamente su propio renderizado.
     */
    virtual void render(const glm::mat4 &projection, const glm::mat4 &view) = 0;

    /*
     * Regresa el nombre de la escena.
     *
     * Es una función virtual pura para que cada escena defina
     * su propio identificador, útil para depuración o interfaz.
     */
    virtual const char* getName() const = 0;
};