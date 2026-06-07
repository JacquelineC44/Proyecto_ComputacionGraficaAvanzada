#ifndef CASINO_SCENE_H
#define CASINO_SCENE_H

/*
 * Archivo: CasinoScene.h
 * Descripción:
 * Declara la escena del Casino dentro del sistema modular del proyecto.
 *
 * Esta escena se encarga de cargar, actualizar y renderizar el escenario
 * del casino, además de manejar la lógica del segundo objeto recolectable.
 *
 * Hereda de la clase base Scene, por lo que implementa los métodos
 * principales de entrada, actualización, renderizado y nombre de escena.
 */

#include "Scene.h"

#include <glm/glm.hpp>
#include <vector>


// =====================
// ESCENA DEL CASINO
// =====================

class CasinoScene : public Scene {
private:
    // =====================
    // RECOLECTABLE 2
    // =====================

    glm::vec3 posicionRecolectable2;
    float rotacionRecolectable2Y;
    float escalaRecolectable2;

    bool recolectable2Activo;
    float distanciaRecolectar2;

    bool estabaCercaRecolectable2;
    bool teclaRecolectar2Presionada;
    //objeto
    bool mostrarDebugTriggerRecolectable2;
    glm::vec3 escalaTriggerRecolectable2;

    /*
     * Verifica si Perry está lo suficientemente cerca
     * del segundo recolectable para poder recogerlo.
     */
    bool jugadorCercaRecolectable2();

    /*
     * Actualiza la lógica del segundo recolectable,
     * incluyendo detección de cercanía y recolección.
     */
    void actualizarRecolectable2();

    /*
     * Renderiza el segundo recolectable dentro del casino.
     */
    void renderRecolectable2();
    //objeto
    void renderTriggerRecolectable2Debug();
    // =====================
    // ENEMIGO DEL CASINO
    // =====================

    enum EstadoEnemigoCasino {
        CASINO_PATRULLANDO,
        CASINO_VIENDO_PERRY
    };

    EstadoEnemigoCasino estadoEnemigoCasino;

    glm::vec3 posicionEnemigoCasino;
    std::vector<glm::vec3> puntosPatrullaCasino;
    int puntoPatrullaCasinoActual;

    float velocidadEnemigoCasino;
    float rotacionEnemigoCasinoY;
    float distanciaVisionEnemigoCasino;
    float anguloVisionEnemigoCasino;

    //enemigo casino
    void patrullarEnemigoCasino(double dt);
    void renderEnemigoCasino();

    bool puedeVerAPerryCasino();
    void mirarAPerryCasino();

    // escondete 
    bool mostrarMensajeDetectadoCasino;

public:
    // =====================
    // FUNCIONES PRINCIPALES DE ESCENA
    // =====================

    /*
     * Se ejecuta al entrar a la escena del casino.
     */
    void onEnter() override;

    /*
     * Actualiza la lógica de la escena del casino.
     */
    void update(double dt) override;

    /*
     * Renderiza todos los elementos visibles de la escena del casino.
     */
    void render(
        const glm::mat4& projection,
        const glm::mat4& view
    ) override;

    /*
     * Regresa el nombre de la escena.
     */
    const char* getName() const override;
};

#endif