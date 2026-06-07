#pragma once

/*
 * Archivo: SceneManager.h
 * Descripción:
 * Este archivo declara la clase SceneManager, encargada de administrar
 * las diferentes escenas del proyecto final.
 *
 * Su función principal es controlar qué escena se encuentra activa,
 * permitir el cambio entre escenas, actualizar la lógica de la escena
 * actual y renderizar únicamente la escena seleccionada.
 *
 * Este administrador permite trabajar con una estructura modular similar
 * a un sistema de escenas tipo motor de videojuegos, separando la lógica
 * del Pueblo, Casino, Bolos u otras escenas dentro del proyecto.
 */

#include <memory>
#include <vector>

#include <glm/glm.hpp>

#include "Scene.h"

// =====================
// ADMINISTRADOR DE ESCENAS
// =====================

class SceneManager {
private:
    // Vector que almacena todas las escenas disponibles dentro del proyecto.
    // Se utilizan shared_ptr para manejar automáticamente la memoria de cada escena.
    std::vector<std::shared_ptr<Scene>> scenes;

    // Índice de la escena actualmente activa.
    // El valor -1 indica que todavía no se ha seleccionado ninguna escena.
    int activeIndex = -1;

public:
    // =====================
    // GESTIÓN DE ESCENAS
    // =====================

    /*
     * Agrega una nueva escena al administrador.
     *
     * Parámetros:
     * - scene: puntero compartido a una escena derivada de la clase base Scene.
     *
     * Esta función permite registrar escenas como PuebloScene, CasinoScene,
     * BolosScene, entre otras.
     */
    void addScene(std::shared_ptr<Scene> scene);

    /*
     * Cambia la escena activa del proyecto.
     *
     * Parámetros:
     * - index: posición de la escena dentro del vector de escenas.
     *
     * Al cambiar de escena, el SceneManager debe encargarse de desactivar
     * la escena anterior y activar la nueva, dependiendo de la implementación
     * definida en SceneManager.cpp.
     */
    void setScene(int index);

    // =====================
    // CICLO DE ACTUALIZACIÓN Y RENDER
    // =====================

    /*
     * Actualiza la lógica de la escena activa.
     *
     * Parámetros:
     * - dt: delta time, es decir, el tiempo transcurrido entre frames.
     *
     * Esta función se usa para actualizar movimiento, animaciones,
     * colisiones, eventos y cualquier lógica interna de la escena actual.
     */
    void update(double dt);

    /*
     * Renderiza la escena activa.
     *
     * Parámetros:
     * - projection: matriz de proyección usada por la cámara.
     * - view: matriz de vista usada para representar la posición y orientación
     *         de la cámara dentro del mundo 3D.
     *
     * Solamente se renderiza la escena seleccionada mediante activeIndex.
     */
    void render(const glm::mat4 &projection, const glm::mat4 &view);

    // =====================
    // CONSULTA DE INFORMACIÓN
    // =====================

    /*
     * Obtiene el nombre de la escena actualmente activa.
     *
     * Retorna:
     * - const char*: nombre de la escena activa.
     *
     * Esta función es útil para depuración, impresión en consola,
     * interfaz de usuario o validación del estado actual del juego.
     */
    const char* getActiveSceneName() const;
};