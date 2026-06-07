#pragma once

/*
 * Archivo: PuebloScene.h
 * Descripción:
 * Declara la escena del Pueblo dentro del sistema modular del proyecto.
 *
 * Esta escena representa el entorno principal del pueblo, incluyendo
 * terreno, construcciones, Perry, colisiones, enemigo con patrullaje
 * y el primer objeto recolectable de la misión.
 */

#include "Scene.h"

#include <vector>

#include <glm/glm.hpp>


// =====================
// ESTRUCTURAS DE VISIÓN
// =====================

struct AABBVision {
    glm::vec3 min;
    glm::vec3 max;
};


// =====================
// ESTADOS DEL ENEMIGO
// =====================

enum EstadoEnemigo {
    PATRULLANDO,
    VIENDO_PERRY
};


// =====================
// ESCENA DEL PUEBLO
// =====================

class PuebloScene : public Scene {
private:
    // =====================
    // ENEMIGO
    // =====================

    EstadoEnemigo estadoEnemigo;

    glm::vec3 posicionEnemigo;
    std::vector<glm::vec3> puntosPatrulla;
    int puntoPatrullaActual;

    float velocidadEnemigo;
    float rotacionEnemigoY;
    float distanciaVisionEnemigo;
    float anguloVisionEnemigo;

    std::vector<AABBVision> obstaculosVision;

    void patrullarEnemigo(double dt);
    void renderEnemigo();
    void renderCampoVision();
    void dibujarLineaDebug(const glm::vec3& inicio, const glm::vec3& fin);

    bool rayoIntersectaAABB(
        const glm::vec3& origen,
        const glm::vec3& destino,
        const AABBVision& caja
    );

    bool hayLineaDeVision(
        const glm::vec3& origen,
        const glm::vec3& destino
    );

    void probarDeteccionPerry();
    bool puedeVerAPerry();
    void mirarAPerry();


    // =====================
    // RECOLECTABLE 1
    // =====================

    glm::vec3 posicionRecolectable1;
    float rotacionRecolectable1Y;
    float escalaRecolectable1;

    bool recolectable1Activo;
    float distanciaRecolectar;

    bool estabaCercaRecolectable1;
    bool teclaRecolectarPresionada;

    bool mostrarDebugTriggerRecolectable1;
    glm::vec3 escalaTriggerRecolectable1;

    void renderRecolectable1();
    void renderTriggerRecolectable1Debug();

    bool jugadorCercaRecolectable1();
    void actualizarRecolectable1();

    // escondete
    bool mostrarMensajeDetectadoPueblo;

    void limitarMovimientoPueblo();


public:
    // =====================
    // FUNCIONES PRINCIPALES DE ESCENA
    // =====================

    void onEnter() override;
    void update(double dt) override;

    void render(
        const glm::mat4& projection,
        const glm::mat4& view
    ) override;

    const char* getName() const override;
};