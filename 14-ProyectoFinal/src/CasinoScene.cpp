#include "CasinoScene.h"

#include "Globals.h"
#include "RenderUtils.h"

#include <iostream>

#include <glm/gtc/matrix_transform.hpp>


/*
 * Archivo: CasinoScene.cpp
 * Descripción:
 * Implementa la escena del Casino, incluyendo carga de modelo,
 * configuración inicial de Perry, audio, colisiones y recolectable 2.
 */


// =====================
// ENTRADA A LA ESCENA
// =====================

void CasinoScene::onEnter() {
	std::cout << "Escena activa: Casino" << std::endl;
	mostrarMensajeInteraccion = false;
	mensajeInteraccion = "";

	camera->setPosition(glm::vec3(0.0f, 4.0f, 15.0f));

	if (!modelosCasinoCargados) {
		cargarModelosCasino();
		modelosCasinoCargados = true;
	}

	cargarModeloRecolectable2();
	//enemigoCasino
	cargarModeloEnemigoCasino();
	//

	// Posición inicial de Perry dentro del casino.
	posicionPerry = glm::vec3(0.0f, 0.0f, 12.0f);
	rotacionPerryY = 180.0f;
	perryEnSuelo = true;
	velocidadVerticalPerry = 0.0f;

	// =====================
	// ENEMIGO DEL CASINO
	// =====================

	estadoEnemigoCasino = CASINO_PATRULLANDO;

	puntosPatrullaCasino.clear();

	
	puntosPatrullaCasino.push_back(glm::vec3(1.38115f, 2.0f, -7.77791f));
	puntosPatrullaCasino.push_back(glm::vec3(-6.25288f, 2.0f, -8.03712f));
	puntosPatrullaCasino.push_back(glm::vec3(1.38115f, 2.0f, -7.77791f));
	puntosPatrullaCasino.push_back(glm::vec3(1.5663f, 2.0f, -4.95692f));
	puntosPatrullaCasino.push_back(glm::vec3(0.360923f, 2.0f, -3.30337f));
	puntosPatrullaCasino.push_back(glm::vec3(-0.574878f, 2.0f, 1.77022f));
	puntosPatrullaCasino.push_back(glm::vec3(-5.23966f, 2.0f, 2.76847f));
	puntosPatrullaCasino.push_back(glm::vec3(-4.75626f, 2.0f, 5.69605f));
	puntosPatrullaCasino.push_back(glm::vec3(0.579287f, 2.0f, 8.52192f));
	puntosPatrullaCasino.push_back(glm::vec3(0.74036f, 2.0f, 5.8284f));
	puntosPatrullaCasino.push_back(glm::vec3(8.69242f, 2.0f, 5.21063f));
	puntosPatrullaCasino.push_back(glm::vec3(8.58458f, 2.0f, -6.18375f));

	puntoPatrullaCasinoActual = 1;

	posicionEnemigoCasino = puntosPatrullaCasino[0];

	velocidadEnemigoCasino = 2.0f;
	rotacionEnemigoCasinoY = 0.0f;
	distanciaVisionEnemigoCasino = 15.0f;
	anguloVisionEnemigoCasino = 45.0f;

	//escondete
	mostrarMensajeDetectadoCasino = false;
	//fin enemigo

	//objeto
	// Configuración inicial del segundo recolectable.
	posicionRecolectable2 = glm::vec3(-6.546f, 2.0f, -8.6845f);
	rotacionRecolectable2Y = 0.0f;
	escalaRecolectable2 = 0.2f;
	recolectable2Activo = true;
	distanciaRecolectar2 = 2.0f;
	estabaCercaRecolectable2 = false;
	teclaRecolectar2Presionada = false;

	// Debug visual del trigger del recolectable 2
	mostrarDebugTriggerRecolectable2 = true;

	escalaTriggerRecolectable2 = glm::vec3(
		distanciaRecolectar2 * 2.0f,
		1.5f,
		distanciaRecolectar2 * 2.0f
	);

	actualizarCollidersCasino();

	reproducirAudioCasino();
	reproducirSonidoMisionCasino();
}


// =====================
// ACTUALIZACIÓN
// =====================

void CasinoScene::update(double dt) {
    actualizarRecolectable2();

    bool veAPerry = puedeVerAPerryCasino();
	mostrarMensajeDetectadoCasino = veAPerry;

	// Registrar detecciones para el sistema de puntuacion
	registrarDeteccionJugador(veAPerry, static_cast<float>(dt));

    if (veAPerry) {
		estadoEnemigoCasino = CASINO_VIENDO_PERRY;
		mirarAPerryCasino();

		static float tiempoDebugVisionCasino = 0.0f;
		tiempoDebugVisionCasino += static_cast<float>(dt);

		if (tiempoDebugVisionCasino >= 1.0f) {
			std::cout << "Enemigo Casino: Perry detectado" << std::endl;
			tiempoDebugVisionCasino = 0.0f;
		}
	}
    else {
        estadoEnemigoCasino = CASINO_PATRULLANDO;
        patrullarEnemigoCasino(dt);
    }
}

// =====================
// RENDER
// =====================

void CasinoScene::render(const glm::mat4& projection, const glm::mat4& view) {
	configurarMatricesCamara(projection, view);
	configurarLucesCasino();

	renderCasino();
	renderRecolectable2();
	renderRecolectable2();
	renderTriggerRecolectable2Debug();
	renderEnemigoCasino();

	if (!camaraPrimeraPersona) {
		renderPerry();
		//renderColliderPerryDebug(projection, view);
	}
	// escondete
	if (mostrarMensajeDetectadoCasino && textoPantalla != nullptr) {
    textoPantalla->render(
        "TE HAN DETECTADO, ESCONDETE",
        -0.45f, 0.0f,
        26,
        1.0f, 0.0f, 0.0f
    );
}

	//renderCollidersDebug(projection, view);
	renderSkyboxDiaNoche(view);
}


// =====================
// RECOLECTABLE 2
// =====================
//objeto 2
void CasinoScene::renderRecolectable2() {
    if (!recolectable2Activo || !modeloRecolectable2Cargado) {
        return;
    }

    // =====================
    // ANIMACIÓN VISUAL
    // =====================
    float tiempo = static_cast<float>(glfwGetTime());

    // Movimiento arriba / abajo
    float alturaLevitar = 0.35f;
    float velocidadLevitar = 2.0f;

    float desplazamientoY = sin(tiempo * velocidadLevitar) * alturaLevitar;

    // Rotación constante
    float velocidadRotacion = 80.0f;
    float rotacionY = tiempo * velocidadRotacion;

    glm::mat4 model = glm::mat4(1.0f);

    // Posición base + levitación
    model = glm::translate(
        model,
        posicionRecolectable2 + glm::vec3(0.0f, desplazamientoY, 0.0f)
    );

    // Rotación sobre su propio eje
    model = glm::rotate(
        model,
        glm::radians(rotacionY),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    // Escala del objeto
    model = glm::scale(
        model,
        glm::vec3(escalaRecolectable2)
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

    Recolectable2.render(model);
}
void CasinoScene::renderTriggerRecolectable2Debug() {
    if (!mostrarDebugTriggerRecolectable2) {
        return;
    }

    if (!recolectable2Activo) {
        return;
    }

    glm::mat4 model = glm::mat4(1.0f);

    model = glm::translate(
        model,
        glm::vec3(
            posicionRecolectable2.x,
            posicionRecolectable2.y + 0.75f,
            posicionRecolectable2.z
        )
    );

    model = glm::scale(model, escalaTriggerRecolectable2);

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
bool CasinoScene::jugadorCercaRecolectable2() {
	if (!recolectable2Activo) {
		return false;
	}

	float distancia = glm::distance(
		glm::vec2(posicionPerry.x, posicionPerry.z),
		glm::vec2(posicionRecolectable2.x, posicionRecolectable2.z)
	);

	return distancia <= distanciaRecolectar2;
}

void CasinoScene::actualizarRecolectable2() {
	if (!recolectable2Activo) {
		mostrarMensajeInteraccion = false;
		mensajeInteraccion = "";
		return;
	}

	bool cerca = jugadorCercaRecolectable2();
	bool teclaE = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
		// TECLA RECOLECTAR 
		// CONTROL XBOX
	bool botonXControl = false;

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)) {
		GLFWgamepadstate gamepad;

		if (glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad)) {
			if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_X] == GLFW_PRESS) {
				botonXControl = true;
			}
		}
	}

	// Si presiona E o X del control
	bool botonRecolectar = teclaE || botonXControl;

	if (cerca) {
		mostrarMensajeInteraccion = true;
		mensajeInteraccion = "Presiona E o X para recoger";
	}
	else if (estabaCercaRecolectable2) {
		mostrarMensajeInteraccion = false;
		mensajeInteraccion = "";
	}

	if (cerca && !estabaCercaRecolectable2) {
		std::cout << "Perry esta cerca del recolectable 2" << std::endl;
	}

	if (!cerca && estabaCercaRecolectable2) {
		std::cout << "Perry se alejo del recolectable 2" << std::endl;
	}

	if (cerca && botonRecolectar && !teclaRecolectar2Presionada) {
		recolectable2Activo = false;
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

		std::cout << "Recolectable 2 obtenido. Objetos: "
				  << objetosRecolectados
				  << "/"
				  << objetosTotales
				  << std::endl;
	}

	teclaRecolectar2Presionada = botonRecolectar;
	estabaCercaRecolectable2 = cerca;
}

//enemigo casino
void CasinoScene::patrullarEnemigoCasino(double dt) {
    if (puntosPatrullaCasino.empty()) {
        return;
    }

    float dtf = static_cast<float>(dt);

    glm::vec3 objetivo = puntosPatrullaCasino[puntoPatrullaCasinoActual];
    glm::vec3 direccion = objetivo - posicionEnemigoCasino;

    if (glm::length(direccion) > 0.01f) {
        direccion = glm::normalize(direccion);

        posicionEnemigoCasino += direccion * velocidadEnemigoCasino * dtf;

        rotacionEnemigoCasinoY = glm::degrees(
            atan2(direccion.x, direccion.z)
        );
    }

    if (glm::distance(posicionEnemigoCasino, objetivo) < 0.3f) {
        puntoPatrullaCasinoActual++;

        if (puntoPatrullaCasinoActual >= static_cast<int>(puntosPatrullaCasino.size())) {
            puntoPatrullaCasinoActual = 0;
        }
    }
}

void CasinoScene::renderEnemigoCasino() {
    if (!modeloEnemigoCasinoCargado) {
        return;
    }

    shaderMulLighting.turnOn();

    glm::mat4 modelEnemigo = glm::mat4(1.0f);

    modelEnemigo = glm::translate(
        modelEnemigo,
        posicionEnemigoCasino + glm::vec3(0.0f, -2.0f, 0.0f)
    );

    modelEnemigo = glm::rotate(
        modelEnemigo,
        glm::radians(rotacionEnemigoCasinoY),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    modelEnemigo = glm::scale(
        modelEnemigo,
        glm::vec3(0.015f)
    );

    EnemigoCasino.render(modelEnemigo);
}
bool CasinoScene::puedeVerAPerryCasino() {
    glm::vec3 direccionAPerry = posicionPerry - posicionEnemigoCasino;

    float distanciaAPerry = glm::length(direccionAPerry);

    if (distanciaAPerry > distanciaVisionEnemigoCasino) {
        return false;
    }

    if (distanciaAPerry < 0.01f) {
        return true;
    }

    direccionAPerry = glm::normalize(direccionAPerry);

    glm::vec3 frenteEnemigo = glm::vec3(
        sin(glm::radians(rotacionEnemigoCasinoY)),
        0.0f,
        cos(glm::radians(rotacionEnemigoCasinoY))
    );

    frenteEnemigo = glm::normalize(frenteEnemigo);

    float dotProducto = glm::dot(frenteEnemigo, direccionAPerry);

    dotProducto = glm::clamp(dotProducto, -1.0f, 1.0f);

    float angulo = glm::degrees(acos(dotProducto));

	if (angulo > anguloVisionEnemigoCasino) {
		return false;
	}

	// Subimos un poco el origen y destino para que el rayo vaya a la altura del cuerpo,
	// no pegado al piso.
	glm::vec3 origenVision = posicionEnemigoCasino + glm::vec3(0.0f, 1.2f, 0.0f);
	glm::vec3 destinoVision = posicionPerry + glm::vec3(0.0f, 1.2f, 0.0f);

	bool hayObstaculo = hayObstaculoVisionEntrePuntos(
		origenVision,
		destinoVision
	);

	if (hayObstaculo) {
		std::cout << "Perry oculto por collider del casino" << std::endl;
		return false;
	}

	return true;
}
void CasinoScene::mirarAPerryCasino() {
    glm::vec3 direccion = posicionPerry - posicionEnemigoCasino;

    direccion.y = 0.0f;

    if (glm::length(direccion) < 0.01f) {
        return;
    }

    direccion = glm::normalize(direccion);

    rotacionEnemigoCasinoY = glm::degrees(
        atan2(direccion.x, direccion.z)
    );
}
//

// =====================
// INFORMACIÓN DE ESCENA
// =====================

const char* CasinoScene::getName() const {
	return "Casino";
}