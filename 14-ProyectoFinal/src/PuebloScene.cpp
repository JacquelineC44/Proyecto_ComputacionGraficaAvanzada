/*
 * Archivo: PuebloScene.cpp
 * Descripción:
 * Implementa la escena del pueblo, incluyendo la carga inicial de modelos,
 * actualización de Perry, lógica del enemigo, recolectables, colisiones,
 * renderizado del terreno y elementos visuales de depuración.
 */

#include "PuebloScene.h"

#include <iostream>
#include <cmath>
#include <algorithm>

#include "Globals.h"
#include "RenderUtils.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

// =====================
// ENTRADA A ESCENA
// =====================

void PuebloScene::onEnter() {
    std::cout << "Escena activa: Pueblo" << std::endl;

    camera->setPosition(glm::vec3(0.0f, 4.0f, 12.0f));

    // =====================
    // CARGA DE MODELOS DEL PUEBLO
    // =====================

    if (!modelosPuebloCargados) {
        cargarModelosPueblo();
        modelosPuebloCargados = true;
    }

    // =====================
    // CARGA DEL MODELO DEL ENEMIGO
    // =====================
    // Esto solo carga el modelo visual.
    // El patrullaje sigue usando posicionEnemigo y rotacionEnemigoY.
    cargarModeloEnemigo();

    // =====================
    // POSICIÓN INICIAL DE PERRY
    // =====================

    posicionPerry = glm::vec3(24.4342f, 0.0f, 10.4037f);
    rotacionPerryY = 180.0f;
    perryEnSuelo = true;
    velocidadVerticalPerry = 0.0f;

    // =====================
    // ENEMIGO / PATRULLAJE
    // =====================

    distanciaVisionEnemigo = 20.0f;
    posicionEnemigo = glm::vec3(2.0f, 1.0f, 5.0f);
    // escondete
    mostrarMensajeDetectadoPueblo = false;

    estadoEnemigo = PATRULLANDO;

    puntosPatrulla.clear();

    puntosPatrulla.push_back(glm::vec3(-27.0f, 2.0f, 35.0f));   // 1
    puntosPatrulla.push_back(glm::vec3(-27.0f, 2.0f, -35.0f));  // 2
    puntosPatrulla.push_back(glm::vec3(10.0f, 2.0f, -35.0f));   // 3
    puntosPatrulla.push_back(glm::vec3(10.0f, 2.0f, 5.0f));     // 4
    puntosPatrulla.push_back(glm::vec3(24.0f, 2.0f, 5.0f));     // 5
    puntosPatrulla.push_back(glm::vec3(24.0f, 2.0f, 35.0f));    // 6
    puntosPatrulla.push_back(glm::vec3(11.0f, 2.0f, 35.0f));    // 7
    puntosPatrulla.push_back(glm::vec3(11.0f, 2.0f, -14.0f));   // 8
    puntosPatrulla.push_back(glm::vec3(24.0f, 2.0f, -14.0f));   // 9
    puntosPatrulla.push_back(glm::vec3(24.0f, 2.0f, -35.0f));   // 10
    puntosPatrulla.push_back(glm::vec3(10.0f, 2.0f, -35.0f));   // 11
    puntosPatrulla.push_back(glm::vec3(10.0f, 2.0f, -14.0f));   // 12
    puntosPatrulla.push_back(glm::vec3(-27.0f, 2.0f, -14.0f));  // 13
    puntosPatrulla.push_back(glm::vec3(-27.0f, 2.0f, 11.0f));   // 14
    puntosPatrulla.push_back(glm::vec3(11.0f, 2.0f, 11.0f));    // 15
    puntosPatrulla.push_back(glm::vec3(11.0f, 2.0f, 35.0f));    // 16

    puntoPatrullaActual = 1;

    // El enemigo empieza en el primer punto.
    posicionEnemigo = puntosPatrulla[0];

    velocidadEnemigo = 2.0f;
    rotacionEnemigoY = 0.0f;
    anguloVisionEnemigo = 45.0f;

    // =====================
    // OBSTÁCULOS DE VISIÓN
    // =====================

    obstaculosVision.clear();

    obstaculosVision.push_back({
        glm::vec3(4.0f, 0.0f, 4.0f),
        glm::vec3(6.0f, 5.0f, 8.0f)
    });

    // =====================
    // COLLIDERS DEL PUEBLO
    // =====================

    actualizarCollidersCentro();

    // =====================
    // RECOLECTABLE 1
    // =====================

    posicionRecolectable1 = glm::vec3(0.0f, 3.0f, 0.0f);
    rotacionRecolectable1Y = 0.0f;
    escalaRecolectable1 = 0.002f;
    recolectable1Activo = true;

    distanciaRecolectar = 1.0f;
    estabaCercaRecolectable1 = false;
    teclaRecolectarPresionada = false;
    mostrarDebugTriggerRecolectable1 = true;

    escalaTriggerRecolectable1 = glm::vec3(
        distanciaRecolectar * 2.0f,
        1.5f,
        distanciaRecolectar * 2.0f
    );
}

// =====================
// ACTUALIZACIÓN DE ESCENA
// =====================

void PuebloScene::update(double dt) {
    static EstadoEnemigo estadoAnterior = PATRULLANDO;
    limitarMovimientoPueblo();

    // =====================
    // RECOLECTABLE
    // =====================

    mostrarMensajeInteraccion = false;
    mensajeInteraccion = "";

    actualizarRecolectable1();

    // =====================
    // ENEMIGO
    // =====================
    //escondete
    bool veAPerry = puedeVerAPerry();
    mostrarMensajeDetectadoPueblo = veAPerry;
    // Registrar detecciones para el sistema de puntuacion
    registrarDeteccionJugador(veAPerry, static_cast<float>(dt));

    if (veAPerry) {
        estadoEnemigo = VIENDO_PERRY;
        mirarAPerry();
    }
    else {
        estadoEnemigo = PATRULLANDO;
        patrullarEnemigo(dt);
    }
    // fin escondete

    if (estadoEnemigo != estadoAnterior) {
        if (estadoEnemigo == VIENDO_PERRY) {
            std::cout << "Perry detectado: enemigo detenido mirando a Perry" << std::endl;
        }
        else {
            std::cout << "Perry perdido: enemigo vuelve a patrullar" << std::endl;
        }

        estadoAnterior = estadoEnemigo;
    }
}

// =====================
// PATRULLAJE DEL ENEMIGO
// =====================

void PuebloScene::patrullarEnemigo(double dt) {
    if (puntosPatrulla.empty()) {
        return;
    }

    float dtf = static_cast<float>(dt);

    glm::vec3 objetivo = puntosPatrulla[puntoPatrullaActual];
    glm::vec3 direccion = objetivo - posicionEnemigo;

    if (glm::length(direccion) > 0.01f) {
        direccion = glm::normalize(direccion);

        posicionEnemigo += direccion * velocidadEnemigo * dtf;

        rotacionEnemigoY = glm::degrees(
            atan2(direccion.x, direccion.z)
        );
    }

    if (glm::distance(posicionEnemigo, objetivo) < 0.3f) {
        puntoPatrullaActual++;

        if (puntoPatrullaActual >= static_cast<int>(puntosPatrulla.size())) {
            puntoPatrullaActual = 0;
        }
    }
}

// =====================
// RENDER DEL ENEMIGO
// =====================

void PuebloScene::renderEnemigo() {
    if (!modeloEnemigoCargado) {
        return;
    }

    shaderMulLighting.turnOn();

    glm::mat4 modelEnemigo = glm::mat4(1.0f);

    // Usa la posición que modifica el patrullaje.
    modelEnemigo = glm::translate(
        modelEnemigo,
        posicionEnemigo + glm::vec3(0.0f, -2.0f, 0.0f)  // Ajuste para que el modelo quede sobre el suelo.
    );

    // Usa la rotación que se calcula en patrullarEnemigo() o mirarAPerry().
    modelEnemigo = glm::rotate(
        modelEnemigo,
        glm::radians(rotacionEnemigoY),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    /*
     * Si tu modelo aparece acostado, activa esta rotación.
     * Si aparece bien parado, déjala comentada.
     */
    /*
    modelEnemigo = glm::rotate(
        modelEnemigo,
        glm::radians(-90.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    );
    */

    /*
     * Ajusta esta escala según el tamaño de tu FBX.
     * Si se ve gigante: baja a 0.01f.
     * Si se ve muy pequeño: sube a 0.05f.
     */
    modelEnemigo = glm::scale(
        modelEnemigo,
        glm::vec3(0.00025f)
    );

    Enemigo.render(modelEnemigo);
}

// =====================
// VISIÓN DEL ENEMIGO
// =====================

bool PuebloScene::rayoIntersectaAABB(
    const glm::vec3& origen,
    const glm::vec3& destino,
    const AABBVision& caja
) {
    glm::vec3 direccion = destino - origen;
    float distanciaMax = glm::length(direccion);

    if (distanciaMax <= 0.001f) {
        return false;
    }

    direccion = glm::normalize(direccion);

    float tMin = 0.0f;
    float tMax = distanciaMax;

    for (int i = 0; i < 3; i++) {
        if (fabs(direccion[i]) < 0.0001f) {
            if (origen[i] < caja.min[i] || origen[i] > caja.max[i]) {
                return false;
            }
        }
        else {
            float invD = 1.0f / direccion[i];

            float t1 = (caja.min[i] - origen[i]) * invD;
            float t2 = (caja.max[i] - origen[i]) * invD;

            if (t1 > t2) {
                std::swap(t1, t2);
            }

            tMin = std::max(tMin, t1);
            tMax = std::min(tMax, t2);

            if (tMin > tMax) {
                return false;
            }
        }
    }

    return true;
}

bool PuebloScene::hayLineaDeVision(
    const glm::vec3& origen,
    const glm::vec3& destino
) {
    for (const AABBVision& caja : obstaculosVision) {
        if (rayoIntersectaAABB(origen, destino, caja)) {
            return false;
        }
    }

    return true;
}

void PuebloScene::probarDeteccionPerry() {
    glm::vec3 posicionPerryCamara = camera->getPosition();

    float distancia = glm::distance(posicionEnemigo, posicionPerryCamara);

    bool estaCerca = distancia <= distanciaVisionEnemigo;
    bool tieneLineaVision = hayLineaDeVision(posicionEnemigo, posicionPerryCamara);

    glm::vec3 direccionPerry = posicionPerryCamara - posicionEnemigo;
    direccionPerry.y = 0.0f;

    bool dentroDelAngulo = false;
    float angulo = 999.0f;

    if (glm::length(direccionPerry) > 0.001f) {
        direccionPerry = glm::normalize(direccionPerry);

        glm::vec3 frenteEnemigo;
        frenteEnemigo.x = sin(glm::radians(rotacionEnemigoY));
        frenteEnemigo.y = 0.0f;
        frenteEnemigo.z = cos(glm::radians(rotacionEnemigoY));
        frenteEnemigo = glm::normalize(frenteEnemigo);

        float productoPunto = glm::dot(frenteEnemigo, direccionPerry);
        productoPunto = glm::clamp(productoPunto, -1.0f, 1.0f);

        angulo = glm::degrees(acos(productoPunto));

        dentroDelAngulo = angulo <= anguloVisionEnemigo;
    }

    static float tiempoDebug = 0.0f;
    tiempoDebug += 0.016f;

    if (tiempoDebug >= 1.0f) {
        std::cout << "Distancia: " << distancia
                  << " | Angulo: " << angulo
                  << " | Cerca: " << estaCerca
                  << " | En cono: " << dentroDelAngulo
                  << " | Linea vision: " << tieneLineaVision
                  << std::endl;

        if (estaCerca && dentroDelAngulo && tieneLineaVision) {
            std::cout << "Perry detectado" << std::endl;
        }
        else if (!estaCerca) {
            std::cout << "Perry demasiado lejos" << std::endl;
        }
        else if (!dentroDelAngulo) {
            std::cout << "Perry fuera del campo de vision" << std::endl;
        }
        else {
            std::cout << "Perry oculto por obstaculo" << std::endl;
        }

        tiempoDebug = 0.0f;
    }
}

bool PuebloScene::puedeVerAPerry() {
    glm::vec3 posicionPerryCamara = camera->getPosition();

    float distancia = glm::distance(posicionEnemigo, posicionPerryCamara);

    bool estaCerca = distancia <= distanciaVisionEnemigo;
    bool tieneLineaVision = hayLineaDeVision(posicionEnemigo, posicionPerryCamara);

    glm::vec3 direccionPerry = posicionPerryCamara - posicionEnemigo;
    direccionPerry.y = 0.0f;

    bool dentroDelAngulo = false;

    if (glm::length(direccionPerry) > 0.001f) {
        direccionPerry = glm::normalize(direccionPerry);

        glm::vec3 frenteEnemigo;
        frenteEnemigo.x = sin(glm::radians(rotacionEnemigoY));
        frenteEnemigo.y = 0.0f;
        frenteEnemigo.z = cos(glm::radians(rotacionEnemigoY));
        frenteEnemigo = glm::normalize(frenteEnemigo);

        float productoPunto = glm::dot(frenteEnemigo, direccionPerry);
        productoPunto = glm::clamp(productoPunto, -1.0f, 1.0f);

        float angulo = glm::degrees(acos(productoPunto));

        dentroDelAngulo = angulo <= anguloVisionEnemigo;
    }

    return estaCerca && tieneLineaVision && dentroDelAngulo;
}

void PuebloScene::mirarAPerry() {
    glm::vec3 posicionPerryCamara = camera->getPosition();

    glm::vec3 direccion = posicionPerryCamara - posicionEnemigo;
    direccion.y = 0.0f;

    if (glm::length(direccion) <= 0.001f) {
        return;
    }

    direccion = glm::normalize(direccion);

    rotacionEnemigoY = glm::degrees(
        atan2(direccion.x, direccion.z)
    );
}

// =====================
// DEBUG CAMPO DE VISIÓN
// =====================

void PuebloScene::dibujarLineaDebug(
    const glm::vec3& inicio,
    const glm::vec3& fin
) {
    glm::vec3 direccion = fin - inicio;
    float longitud = glm::length(direccion);

    if (longitud <= 0.001f) {
        return;
    }

    direccion = glm::normalize(direccion);

    glm::vec3 centro = inicio + direccion * (longitud * 0.5f);

    float anguloY = glm::degrees(atan2(direccion.x, direccion.z));

    glm::mat4 modelLinea = glm::mat4(1.0f);

    modelLinea = glm::translate(modelLinea, centro);

    modelLinea = glm::rotate(
        modelLinea,
        glm::radians(anguloY),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    modelLinea = glm::scale(
        modelLinea,
        glm::vec3(0.05f, 0.05f, longitud)
    );

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureCespedID);

    shaderMulLighting.setInt("texture1", 0);
    shaderMulLighting.setVectorFloat2(
        "scaleUV",
        glm::value_ptr(glm::vec2(1.0f, 1.0f))
    );

    boxCesped.render(modelLinea);

    glBindTexture(GL_TEXTURE_2D, 0);
}

/*void PuebloScene::renderCampoVision() {
    glm::vec3 origen = posicionEnemigo;
    origen.y += 1.0f;

    float anguloCentro = rotacionEnemigoY;
    float anguloIzquierda = anguloCentro - anguloVisionEnemigo;
    float anguloDerecha = anguloCentro + anguloVisionEnemigo;

    glm::vec3 dirCentro;
    dirCentro.x = sin(glm::radians(anguloCentro));
    dirCentro.y = 0.0f;
    dirCentro.z = cos(glm::radians(anguloCentro));

    glm::vec3 dirIzquierda;
    dirIzquierda.x = sin(glm::radians(anguloIzquierda));
    dirIzquierda.y = 0.0f;
    dirIzquierda.z = cos(glm::radians(anguloIzquierda));

    glm::vec3 dirDerecha;
    dirDerecha.x = sin(glm::radians(anguloDerecha));
    dirDerecha.y = 0.0f;
    dirDerecha.z = cos(glm::radians(anguloDerecha));

    glm::vec3 finCentro =
        origen + glm::normalize(dirCentro) * distanciaVisionEnemigo;

    glm::vec3 finIzquierda =
        origen + glm::normalize(dirIzquierda) * distanciaVisionEnemigo;

    glm::vec3 finDerecha =
        origen + glm::normalize(dirDerecha) * distanciaVisionEnemigo;

    dibujarLineaDebug(origen, finCentro);
    dibujarLineaDebug(origen, finIzquierda);
    dibujarLineaDebug(origen, finDerecha);
}*/

// =====================
// RENDER PRINCIPAL
// =====================

void PuebloScene::render(const glm::mat4 &projection, const glm::mat4 &view) {
    configurarMatricesCamara(projection, view);
    configurarLucesCentro();

    // renderPiso();
    renderTerreno();
    renderConstrucciones4();

    // =====================
    // RECOLECTABLE
    // =====================

    renderRecolectable1();
    //renderTriggerRecolectable1Debug();

    // =====================
    // PERRY
    // =====================

    if (!camaraPrimeraPersona) {
        renderPerry();
      //  renderColliderPerryDebug(projection, view);
    }

    // =====================
    // DEBUG COLLIDERS
    // =====================

 //   renderCollidersDebug(projection, view);

    // =====================
    // ENEMIGO
    // =====================

    renderEnemigo();
    // renderCampoVision();

    // =====================
    // SKYBOX
    // =====================

    renderSkyboxDiaNoche(view);

    //escondete
    if (mostrarMensajeDetectadoPueblo && textoPantalla != nullptr) {
        textoPantalla->render(
            "TE HAN DETECTADO, ESCONDETE",
            -0.45f, 0.0f,
            26,
            1.0f, 0.0f, 0.0f
        );
    }
}

// =====================
// RECOLECTABLES
// =====================

void PuebloScene::renderRecolectable1() {
    if (!recolectable1Activo) {
        return;
    }

    // =====================
    // ANIMACIÓN VISUAL
    // =====================
    float tiempo = static_cast<float>(glfwGetTime());

    // Movimiento arriba / abajo
    float alturaLevitar = 0.35f;      // qué tanto sube y baja
    float velocidadLevitar = 2.0f;    // qué tan rápido sube y baja

    float desplazamientoY = sin(tiempo * velocidadLevitar) * alturaLevitar;

    // Rotación constante
    float velocidadRotacion = 80.0f;  // grados por segundo
    float rotacionY = tiempo * velocidadRotacion;

    glm::mat4 model = glm::mat4(1.0f);

    // Posición base + levitación
    model = glm::translate(
        model,
        posicionRecolectable1 + glm::vec3(0.0f, desplazamientoY, 0.0f)
    );

    // Rotación sobre su propio eje
    model = glm::rotate(
        model,
        glm::radians(rotacionY),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Escala normal del objeto
    model = glm::scale(
        model,
        glm::vec3(escalaRecolectable1)
    );

    // =====================
    // SHADER FRESNEL
    // =====================
    shaderFresnel.turnOn();

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
        glm::value_ptr(glm::vec3(0.0f, 0.8f, 1.0f))
    );

    shaderFresnel.setFloat("fresnelPower", 1.0f);
    shaderFresnel.setFloat("fresnelIntensity", 0.5f);

    Recolectable1.render(model);
}

void PuebloScene::renderTriggerRecolectable1Debug() {
    if (!mostrarDebugTriggerRecolectable1) {
        return;
    }

    if (!recolectable1Activo) {
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(
        model,
        glm::vec3(
            posicionRecolectable1.x,
            posicionRecolectable1.y + 0.75f,
            posicionRecolectable1.z
        )
    );

    model = glm::scale(model, escalaTriggerRecolectable1);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shaderMulLighting.turnOn();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureCespedID);

    shaderMulLighting.setInt("texture1", 0);
    shaderMulLighting.setVectorFloat2(
        "scaleUV",
        glm::value_ptr(glm::vec2(1.0f, 1.0f))
    );

    boxCesped.render(model);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glBindTexture(GL_TEXTURE_2D, 0);
}

bool PuebloScene::jugadorCercaRecolectable1() {
    if (!recolectable1Activo) {
        return false;
    }

    float distancia = glm::distance(
        glm::vec2(posicionPerry.x, posicionPerry.z),
        glm::vec2(posicionRecolectable1.x, posicionRecolectable1.z)
    );

    return distancia <= distanciaRecolectar;
}

void PuebloScene::actualizarRecolectable1() {
    if (!recolectable1Activo) {
        return;
    }

    float distancia = glm::distance(
        glm::vec2(posicionPerry.x, posicionPerry.z),
        glm::vec2(posicionRecolectable1.x, posicionRecolectable1.z)
    );

    bool cerca = distancia <= distanciaRecolectar;

    // =====================
    // TECLADO
    // =====================
    bool teclaE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;

    // =====================
    // CONTROL XBOX
    // Botón X para recoger
    // =====================
    bool botonXControl = false;

    GLFWgamepadstate gamepad;

    if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
        glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad)) {

        if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS) {
            botonXControl = true;
        }
    }

    // Si presiona E o X del control
    bool botonRecolectar = teclaE || botonXControl;

    if (cerca) {
        mostrarMensajeInteraccion = true;
        mensajeInteraccion = "Presiona E o X para recoger";
    }

    if (cerca && botonRecolectar && !teclaRecolectarPresionada) {
        recolectable1Activo = false;
        objetosRecolectados++;
       if (objetosRecolectados >= objetosTotales && !juegoGanado) {
            juegoGanado = true;
            contadorActivo = false;
            juegoPausado = false;
            mostrarMensajeInteraccion = false;
            mensajeInteraccion = "";

            reproducirAudioVictoria();
        }

        mostrarMensajeInteraccion = false;
        mensajeInteraccion = "";

        std::cout << "Recolectable 1 obtenido. Objetos: "
                  << objetosRecolectados
                  << "/"
                  << objetosTotales
                  << std::endl;
    }

    teclaRecolectarPresionada = botonRecolectar;
    estabaCercaRecolectable1 = cerca;
}
void PuebloScene::limitarMovimientoPueblo() {
    // Ajusta estos valores al tamaño real de tu pueblo
    float limiteMinX = -37.0f;
    float limiteMaxX = 30.0f;

    float limiteMinZ = -40.0f;
    float limiteMaxZ = 34.0f;

    if (posicionPerry.x < limiteMinX) {
        posicionPerry.x = limiteMinX;
    }

    if (posicionPerry.x > limiteMaxX) {
        posicionPerry.x = limiteMaxX;
    }

    if (posicionPerry.z < limiteMinZ) {
        posicionPerry.z = limiteMinZ;
    }

    if (posicionPerry.z > limiteMaxZ) {
        posicionPerry.z = limiteMaxZ;
    }
}

// =====================
// INFORMACIÓN DE ESCENA
// =====================

const char* PuebloScene::getName() const {
    return "Pueblo";
}