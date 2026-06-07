#include "SceneManager.h"

/*
 * Archivo: SceneManager.cpp
 * Descripción:
 * Implementa la lógica del administrador de escenas declarado en SceneManager.h.
 */

// =====================
// GESTIÓN DE ESCENAS
// =====================

void SceneManager::addScene(std::shared_ptr<Scene> scene) {
    scenes.push_back(scene);

    // Si es la primera escena registrada, se activa automáticamente.
    if (activeIndex == -1) {
        activeIndex = 0;
        scenes[activeIndex]->onEnter();
    }
}

void SceneManager::setScene(int index) {
    // Evita índices inválidos o cambios hacia la misma escena.
    if (index < 0 || index >= static_cast<int>(scenes.size()) || index == activeIndex) {
        return;
    }

    // Ejecuta la salida de la escena actual antes de cambiar.
    if (activeIndex != -1) {
        scenes[activeIndex]->onExit();
    }

    // Activa la nueva escena y ejecuta su inicialización.
    activeIndex = index;
    scenes[activeIndex]->onEnter();
}

// =====================
// ACTUALIZACIÓN Y RENDERIZADO
// =====================

void SceneManager::update(double dt) {
    // Actualiza únicamente la escena activa.
    if (activeIndex != -1) {
        scenes[activeIndex]->update(dt);
    }
}

void SceneManager::render(const glm::mat4 &projection, const glm::mat4 &view) {
    // Renderiza únicamente la escena activa.
    if (activeIndex != -1) {
        scenes[activeIndex]->render(projection, view);
    }
}

// =====================
// CONSULTA DE INFORMACIÓN
// =====================

const char* SceneManager::getActiveSceneName() const {
    // Regresa un texto por defecto si aún no hay escena seleccionada.
    if (activeIndex == -1) {
        return "Sin escena";
    }

    return scenes[activeIndex]->getName();
}