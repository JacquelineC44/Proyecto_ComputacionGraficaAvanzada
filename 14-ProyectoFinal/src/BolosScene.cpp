#include "BolosScene.h"

#include "Globals.h"
#include "RenderUtils.h"

#include <iostream>
#include <cmath>

#include <glm/gtc/type_ptr.hpp>

// =====================
// ENTRAR A ESCENA
// =====================

void BolosScene::onEnter() {
    std::cout << "Escena activa: Bolos" << std::endl;

    camera->setPosition(glm::vec3(0.0f, 4.0f, 15.0f));

    if (!modelosBolosCargados) {
        cargarModelosBolos();
        modelosBolosCargados = true;
    }

    // =====================
    // POSICIÓN INICIAL DE PERRY EN BOLICHE
    // =====================
    posicionPerry = glm::vec3(3.85609f, 0.0f, 8.36436f);
    rotacionPerryY = 180.0f;
    perryEnSuelo = true;
    velocidadVerticalPerry = 0.0f;

    // =====================
    // RECOLECTABLE 3 - OBJETO DEL BOLICHE
    // =====================
    // Temporal: cerca de Perry para probarlo rápido.
    posicionRecolectable3 = glm::vec3(0.0f, 0.5f, 1.5f);

    rotacionRecolectable3Y = 0.0f;
    escalaRecolectable3 = 0.20f;
    tiempoRecolectable3 = 0.0f;

    recolectable3Activo = true;
    distanciaRecolectar3 = 2.0f;

    estabaCercaRecolectable3 = false;
    teclaRecolectar3Presionada = false;

    // =====================
    // COLLIDERS DEL BOLICHE
    // =====================
    actualizarCollidersBolos();
}

// =====================
// UPDATE
// =====================

void BolosScene::update(double dt) {
    mostrarMensajeInteraccion = false;
    mensajeInteraccion = "";

    actualizarRecolectable3();

    if (recolectable3Activo) {
        float dtf = static_cast<float>(dt);

        tiempoRecolectable3 += dtf;

        rotacionRecolectable3Y += 90.0f * dtf;

        if (rotacionRecolectable3Y >= 360.0f) {
            rotacionRecolectable3Y -= 360.0f;
        }
    }
}

// =====================
// RENDER RECOLECTABLE 3
// =====================

void BolosScene::renderRecolectable3(
    const glm::mat4& projection,
    const glm::mat4& view
) {
    if (!recolectable3Activo) {
        return;
    }

    if (!modeloRecolectable3Cargado) {
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);

    float alturaExtra = std::sin(tiempoRecolectable3 * 2.5f) * 0.25f;

    model = glm::translate(
        model,
        glm::vec3(
            posicionRecolectable3.x,
            posicionRecolectable3.y + alturaExtra,
            posicionRecolectable3.z
        )
    );

    model = glm::rotate(
        model,
        glm::radians(rotacionRecolectable3Y),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    model = glm::scale(
        model,
        glm::vec3(escalaRecolectable3)
    );

    shaderFresnel.turnOn();

    shaderFresnel.setMatrix4(
        "projection",
        1,
        false,
        glm::value_ptr(projection)
    );

    shaderFresnel.setMatrix4(
        "view",
        1,
        false,
        glm::value_ptr(view)
    );

    shaderFresnel.setMatrix4(
        "model",
        1,
        false,
        glm::value_ptr(model)
    );

    shaderFresnel.setVectorFloat3(
        "viewPos",
        glm::value_ptr(camera->getPosition())
    );

    shaderFresnel.setVectorFloat3(
        "fresnelColor",
        glm::value_ptr(glm::vec3(1.0f, 0.8f, 0.0f))
    );

    shaderFresnel.setFloat("fresnelPower", 2.0f);
    shaderFresnel.setFloat("fresnelIntensity", 0.3f);

    Recolectable3.setShader(&shaderFresnel);
    Recolectable3.render(model);

    Recolectable3.setShader(&shaderMulLighting);
}

// =====================
// RENDER ESCENA
// =====================

void BolosScene::render(const glm::mat4 &projection, const glm::mat4 &view) {
    configurarMatricesCamara(projection, view);
    configurarLucesBoliche();

    renderBolos();

    renderRecolectable3(projection, view);

    if (!camaraPrimeraPersona) {
        renderPerry();
        //renderColliderPerryDebug(projection, view);
    }

    //renderCollidersDebug(projection, view);

    renderSkyboxDiaNoche(view);
}

// =====================
// DISTANCIA RECOLECTABLE 3
// =====================

bool BolosScene::jugadorCercaRecolectable3() {
    if (!recolectable3Activo) {
        return false;
    }

    float distancia = glm::distance(
        glm::vec2(posicionPerry.x, posicionPerry.z),
        glm::vec2(posicionRecolectable3.x, posicionRecolectable3.z)
    );

    return distancia <= distanciaRecolectar3;
}

// =====================
// ACTUALIZAR RECOLECTABLE 3
// =====================

void BolosScene::actualizarRecolectable3() {
    if (!recolectable3Activo) {
        return;
    }

    bool cerca = jugadorCercaRecolectable3();

    bool teclaE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

    // CONTROL XBOX: botón X para recolectar
    bool botonXControl = false;

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
        GLFWgamepadstate gamepad;

        if (glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad)) {
            if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS) {
                botonXControl = true;
            }
        }
    }

    bool botonRecolectar = teclaE || botonXControl;

    if (cerca) {
    mostrarMensajeInteraccion = true;
    mensajeInteraccion = "Presiona E o X para recoger";
    }
    else if (estabaCercaRecolectable3) {
        mostrarMensajeInteraccion = false;
        mensajeInteraccion = "";
    }

    if (cerca && botonRecolectar && !teclaRecolectar3Presionada) {
        recolectable3Activo = false;
        objetosRecolectados++;

        if (objetosRecolectados >= objetosTotales && !juegoGanado) {
            juegoGanado = true;
            contadorActivo = false;
            juegoPausado = false;
            mostrarMensajeInteraccion = false;
            mensajeInteraccion = "";

            reproducirAudioVictoria();
        }

        mensajeInteraccion = "";

        std::cout << "Recolectable 3 obtenido. Total: "
            << objetosRecolectados << "/"
            << objetosTotales << std::endl;
    }

    teclaRecolectar3Presionada = botonRecolectar;
    estabaCercaRecolectable3 = cerca;
}

// =====================
// NOMBRE ESCENA
// =====================

const char* BolosScene::getName() const {
    return "Bolos";
}