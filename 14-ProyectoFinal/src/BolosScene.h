#ifndef BOLOS_SCENE_H
#define BOLOS_SCENE_H

#include "Scene.h"
#include <glm/glm.hpp>

class BolosScene : public Scene {
private:
    // =====================
    // RECOLECTABLE 3 - BOLICHE
    // =====================
    glm::vec3 posicionRecolectable3;
    float rotacionRecolectable3Y;
    float escalaRecolectable3;
    float tiempoRecolectable3;

    bool recolectable3Activo;
    float distanciaRecolectar3;

    bool estabaCercaRecolectable3;
    bool teclaRecolectar3Presionada;

    bool jugadorCercaRecolectable3();
    void actualizarRecolectable3();
    void renderRecolectable3(
    const glm::mat4& projection,
    const glm::mat4& view
    );

public:
    void onEnter() override;
    void update(double dt) override;
    void render(const glm::mat4 &projection, const glm::mat4 &view) override;
    const char* getName() const override;
};

#endif