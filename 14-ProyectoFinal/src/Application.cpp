#include "Application.h"

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Globals.h"
#include "SceneManager.h"
#include "PuebloScene.h"
#include "CasinoScene.h"
#include "BolosScene.h"
#include "RenderUtils.h"
#include "Headers/TimeManager.h"
#include "Headers/FontTypeRendering.h"
#include "Headers/Terrain.h"

SceneManager sceneManager;

static void eliminarTextura(GLuint& textureID) {
	if (textureID != 0) {
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}
}

// =====================
// ENTRADA AL CASINO
// =====================

double tiempoUltimoCambioEscena = 0.0;

bool jugadorCercaDelCasino() {
	glm::vec3 posicionJugador = posicionPerry;

	glm::vec3 posicionEntradaCasino = glm::vec3(
		1.59997f,
		0.687311f,
		4.59204f
	);

	float distancia = glm::distance(
		glm::vec2(posicionJugador.x, posicionJugador.z),
		glm::vec2(posicionEntradaCasino.x, posicionEntradaCasino.z)
	);

	return distancia < 2.0f;
}
bool jugadorCercaDelBoliche() {
	glm::vec3 posicionJugador = posicionPerry;

	// CAMBIA ESTAS COORDENADAS por la entrada real del boliche en Pueblo
	glm::vec3 posicionEntradaBoliche = glm::vec3(
		-0.287145f,
		0.0f,
		13.6856f
	);

	float distancia = glm::distance(
		glm::vec2(posicionJugador.x, posicionJugador.z),
		glm::vec2(posicionEntradaBoliche.x, posicionEntradaBoliche.z)
	);

	return distancia < 2.0f;
}
//salida casino
bool jugadorCercaSalidaCasino() {
    glm::vec3 posicionJugador = posicionPerry;

    // CAMBIA ESTAS COORDENADAS por el punto donde quieras poner la salida del casino
    glm::vec3 posicionSalidaCasino = glm::vec3(
        -3.5818f,
        0.0f,
        9.6351f
    );

    float distancia = glm::distance(
        glm::vec2(posicionJugador.x, posicionJugador.z),
        glm::vec2(posicionSalidaCasino.x, posicionSalidaCasino.z)
    );

    return distancia < 4.0f;
}
bool jugadorCercaSalidaBoliche() {
	glm::vec3 posicionJugador = posicionPerry;

	// CAMBIA ESTAS COORDENADAS por la salida real dentro de BolosScene
	glm::vec3 posicionSalidaBoliche = glm::vec3(
		-13.923f,
		0.0f,
		1.60994f
	);

	float distancia = glm::distance(
		glm::vec2(posicionJugador.x, posicionJugador.z),
		glm::vec2(posicionSalidaBoliche.x, posicionSalidaBoliche.z)
	);

	return distancia < 6.0f;
}

// =====================
// INIT
// =====================

void init(int width, int height, std::string strTitle, bool bFullScreen) {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		exit(-1);
	}

	screenWidth = width;
	screenHeight = height;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(
		width,
		height,
		strTitle.c_str(),
		bFullScreen ? glfwGetPrimaryMonitor() : nullptr,
		nullptr
	);

	if (window == nullptr) {
		std::cerr << "Error to create GLFW window" << std::endl;
		destroy();
		exit(-1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetWindowSizeCallback(window, reshapeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (err != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW" << std::endl;
		exit(-1);
	}

	glViewport(0, 0, screenWidth, screenHeight);
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// =====================
	// AUDIO
	// =====================

	inicializarAudio();
	cargarMusicaMenu();
	cargarSonidoMisionAgenteP();
	cargarAudioFeria();
	cargarAudioCasino();
	cargarAudioVictoria();
	cargarAudioBoliche();
	cargarSonidoEntradaBoliche();

	// =====================
	// TEXTO / HUD
	// =====================

	textoPantalla = new FontTypeRendering::FontTypeRendering(screenWidth, screenHeight);
	textoPantalla->Initialize();

	// =====================
	// SHADERS
	// =====================

	shader.initialize("../Shaders/colorShader.vs", "../Shaders/colorShader.fs");
	shaderSkybox.initialize("../Shaders/skyBox.vs", "../Shaders/skyBox.fs");
	shaderMulLighting.initialize("../Shaders/iluminacion_textura_animation.vs", "../Shaders/multipleLights.fs");
	shaderTexture.initialize("../Shaders/texturizado.vs", "../Shaders/texturizado.fs");
	shaderFresnel.initialize("../Shaders/Fresnel.vs", "../Shaders/Fresnel.fs");

	// Shader específico para el terreno
	shaderTerreno.initialize("../Shaders/terrain.vs", "../Shaders/terrain.fs");

	// =====================
	// OBJETOS BASE
	// =====================

	skyboxSphere.init();
	skyboxSphere.setShader(&shaderSkybox);
	skyboxSphere.setScale(glm::vec3(20.0f));

	boxCesped.init();
	boxCesped.setShader(&shaderMulLighting);

	// =====================
	// INTERFAZ
	// =====================

	boxMenuFondo.init();
	boxMenuFondo.setShader(&shaderTexture);

	boxBotonIniciar.init();
	boxBotonIniciar.setShader(&shaderTexture);

	boxBotonControles.init();
	boxBotonControles.setShader(&shaderTexture);

	boxBotonSalir.init();
	boxBotonSalir.setShader(&shaderTexture);

	boxBotonRegresar.init();
	boxBotonRegresar.setShader(&shaderTexture);

	boxIcono.init();
	boxIcono.setShader(&shaderTexture);

	boxPausa.init();
	boxPausa.setShader(&shaderTexture);

	boxTiempoTerminado.init();
	boxTiempoTerminado.setShader(&shaderTexture);

	boxBotonSalirFinal.init();
	boxBotonSalirFinal.setShader(&shaderTexture);

	// Terreno
	terrain.init();
	terrain.setShader(&shaderTerreno);

	// =====================
	// TEXTURAS GLOBALES
	// =====================

	cargarSkyboxDiaNoche();
	cargarTexturaCesped();

	// Texturas RGB del terreno
	std::cout << "VOY A CARGAR TEXTURAS RGB DEL TERRENO" << std::endl;
	cargarTexturasTerrenoRGB();
	std::cout << "TERMINE DE CARGAR TEXTURAS RGB DEL TERRENO" << std::endl;

	// =====================
	// PERSONAJE PRINCIPAL
	// =====================

	cargarModeloPerry();
	// Objetos recolectables.
	cargarModeloRecolectable1();
	cargarModeloRecolectable2();
	cargarModeloRecolectable3();

	posicionPerry = glm::vec3(0.0f, 5.0f, 0.0f);
	camera->setPosition(posicionPerry + glm::vec3(0.0f, 0.0f, 0.0f));

	// =====================
	// TEXTURAS DE INTERFAZ
	// =====================

	textureMenuFondoID = cargarTexturaMenu(
		"../AssetsPfinal/TexturesP/menu_fondo_nuevo.png",
		0.45f,
		0.75f
	);

	textureBotonIniciarID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_iniciar.png");
	textureBotonIniciarHoverID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_iniciar_hover.png");

	textureBotonControlesID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_controles.png");
	textureBotonControlesHoverID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_controles_hover.png");

	textureBotonSalirID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_salir.png");
	textureBotonSalirHoverID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_salir_hover.png");

	textureBotonRegresarID = cargarTexturaMenu("../AssetsPfinal/boton_regresar.png");
	textureBotonRegresarHoverID = cargarTexturaMenu("../AssetsPfinal/boton_regresar_hover.png");

	textureIconoID = cargarTexturaMenu("../AssetsPfinal/TexturesP/icono.png");
	texturePausaID = cargarTexturaMenu("../AssetsPfinal/TexturesP/pausa.png");
	textureControlesID = cargarTexturaMenu("../AssetsPfinal/TexturesP/controles.jpg");

	textureVictoriaID = cargarTexturaMenu("../AssetsPfinal/TexturesP/ganar.png");
	

	textureTiempoTerminadoID = cargarTexturaMenu("../AssetsPfinal/TexturesP/tiempo_terminado.png");
	textureBotonSalirFinalID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_salir.png");
	textureBotonSalirFinalHoverID = cargarTexturaMenu("../AssetsPfinal/TexturesP/boton_salir_hover.png");

	textureSecuencia1ID = cargarTexturaMenu("../AssetsPfinal/TexturesP/secuencia_1.png");
	textureSecuencia2ID = cargarTexturaMenu("../AssetsPfinal/TexturesP/secuencia_2.png");
	textureSecuencia3ID = cargarTexturaMenu("../AssetsPfinal/TexturesP/secuencia_3.png");

	// =====================
	// ESCENAS
	// =====================

	sceneManager.addScene(std::make_shared<PuebloScene>());
	sceneManager.addScene(std::make_shared<CasinoScene>());
	sceneManager.addScene(std::make_shared<BolosScene>());

	sceneManager.setScene(0);
}

// =====================
// FORMATO TIEMPO
// =====================

std::string formatoTiempo(float tiempo) {
	if (tiempo < 0.0f) {
		tiempo = 0.0f;
	}

	int totalSegundos = static_cast<int>(tiempo);
	int minutos = totalSegundos / 60;
	int segundos = totalSegundos % 60;

	std::string texto = "Tiempo: ";

	if (minutos < 10) {
		texto += "0";
	}

	texto += std::to_string(minutos);
	texto += ":";

	if (segundos < 10) {
		texto += "0";
	}

	texto += std::to_string(segundos);

	return texto;
}


// =====================
// CONTROL XBOX GENERAL
// =====================

void procesarControlXbox() {
	GLFWgamepadstate gamepad;
	bool controlDetectado = false;

	// Busca cualquier control conectado, no solo GLFW_JOYSTICK_1.
	for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
		if (glfwJoystickIsGamepad(i) && glfwGetGamepadState(i, &gamepad)) {
			controlDetectado = true;
			break;
		}
	}

	if (!controlDetectado) {
		return;
	}

	static bool arribaPresionado = false;
	static bool abajoPresionado = false;
	static bool botonAPresionado = false;
	static bool botonBPresionado = false;
	static bool botonStartPresionado = false;
	static bool botonBackPresionado = false;

	const float deadZone = 0.55f;

	bool moverArriba =
		gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS ||
		gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] < -deadZone;

	bool moverAbajo =
		gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS ||
		gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y] > deadZone;

	bool botonA = gamepad.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;
	bool botonB = gamepad.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS;
	bool botonStart = gamepad.buttons[GLFW_GAMEPAD_BUTTON_START] == GLFW_PRESS;
	bool botonBack = gamepad.buttons[GLFW_GAMEPAD_BUTTON_BACK] == GLFW_PRESS;

	// =====================
	// MENÚ INICIAL
	// =====================

	if (mostrarMenu) {
		if (mostrarControles) {
			if (botonA && !botonAPresionado) {
				mostrarControles = false;
				opcionMenu = 1;

				std::cout << "Regresando al menu desde controles con A." << std::endl;
			}

			arribaPresionado = moverArriba;
			abajoPresionado = moverAbajo;
			botonAPresionado = botonA;
			botonBPresionado = botonB;
			botonStartPresionado = botonStart;
			botonBackPresionado = botonBack;

			return;
		}
		if (moverArriba && !arribaPresionado) {
			opcionMenu--;

			if (opcionMenu < 0) {
				opcionMenu = 2;
			}

			std::cout << "Xbox menu opcion: " << opcionMenu << std::endl;
		}

		if (moverAbajo && !abajoPresionado) {
			opcionMenu++;

			if (opcionMenu > 2) {
				opcionMenu = 0;
			}

			std::cout << "Xbox menu opcion: " << opcionMenu << std::endl;
		}

		if (botonA && !botonAPresionado) {
			if (opcionMenu == 0) {
				detenerMusicaMenu();
				reproducirSonidoMisionAgenteP();

				mostrarMenu = false;
				mostrarSecuencia = true;
				juegoPausado = false;

				imagenSecuenciaActual = 0;
				tiempoImagenSecuencia = 0.0f;

				std::cout << "Iniciar seleccionado con Xbox." << std::endl;
			}
			else if (opcionMenu == 1) {
				mostrarControles = true;
				std::cout << "Pantalla de controles abierta con Xbox." << std::endl;
			}
			else if (opcionMenu == 2) {
				detenerMusicaMenu();
				exitApp = true;

				std::cout << "Salir seleccionado con Xbox." << std::endl;
			}
		}

		arribaPresionado = moverArriba;
		abajoPresionado = moverAbajo;
		botonAPresionado = botonA;
		botonBPresionado = botonB;
		botonStartPresionado = botonStart;
		botonBackPresionado = botonBack;

		return;
	}

	// =====================
	// SECUENCIA INTRO
	// =====================

	if (mostrarSecuencia) {
		if (botonA && !botonAPresionado) {
			imagenSecuenciaActual++;
			tiempoImagenSecuencia = 0.0f;

			if (imagenSecuenciaActual > 2) {
				mostrarSecuencia = false;
				contadorActivo = true;
				reproducirAudioFeria();
			}

			std::cout << "Secuencia avanzada con Xbox." << std::endl;
		}

		if (botonB && !botonBPresionado) {
			mostrarSecuencia = false;
			mostrarMenu = true;
			imagenSecuenciaActual = 0;
			tiempoImagenSecuencia = 0.0f;

			std::cout << "Regresando al menu con Xbox." << std::endl;
		}

		arribaPresionado = moverArriba;
		abajoPresionado = moverAbajo;
		botonAPresionado = botonA;
		botonBPresionado = botonB;
		botonStartPresionado = botonStart;
		botonBackPresionado = botonBack;

		return;
	}

	// =====================
	// GAME OVER
	// =====================

	if (tiempoTerminado || juegoGanado) {
		if (botonA && !botonAPresionado) {
			exitApp = true;
		}

		arribaPresionado = moverArriba;
		abajoPresionado = moverAbajo;
		botonAPresionado = botonA;
		botonBPresionado = botonB;
		botonStartPresionado = botonStart;
		botonBackPresionado = botonBack;

		return;
	}
	

	// =====================
	// PAUSA
	// =====================

	if (botonStart && !botonStartPresionado) {
		juegoPausado = !juegoPausado;
		std::cout << (juegoPausado ? "Juego pausado con Xbox." : "Juego reanudado con Xbox.") << std::endl;
	}

	if (juegoPausado) {
		if ((botonA && !botonAPresionado) ||
			(botonB && !botonBPresionado)) {
			juegoPausado = false;
		}

		if (botonBack && !botonBackPresionado) {
			exitApp = true;
		}
	}

	arribaPresionado = moverArriba;
	abajoPresionado = moverAbajo;
	botonAPresionado = botonA;
	botonBPresionado = botonB;
	botonStartPresionado = botonStart;
	botonBackPresionado = botonBack;
}

// =====================
// CONTROL XBOX DIRECTO PARA MENÚ INICIAL
// =====================
// =====================
// CONTROL XBOX DIRECTO PARA MENÚ INICIAL
// Compatible con gamepad y joystick normal
// =====================

void procesarXboxMenuInicialDirecto() {
	if (!mostrarMenu) {
		return;
	}
	if (mostrarControles) {
		return;
	}

	static bool direccionPresionada = false;
	static bool botonAPresionado = false;

	bool controlDetectado = false;

	bool moverArriba = false;
	bool moverAbajo = false;
	bool botonA = false;

	const float deadZone = 0.55f;
	const float zonaCentro = 0.25f;

	// =====================
	// 1) INTENTO COMO GAMEPAD
	// =====================

	GLFWgamepadstate gamepad;

	for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
		if (glfwJoystickIsGamepad(i) && glfwGetGamepadState(i, &gamepad)) {
			controlDetectado = true;

			float ejeY = gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

			bool stickArriba = ejeY < deadZone;
			bool stickAbajo = ejeY > -deadZone;

			bool dpadArriba =
				gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP] == GLFW_PRESS;

			bool dpadAbajo =
				gamepad.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN] == GLFW_PRESS;

			moverArriba = stickArriba || dpadArriba;
			moverAbajo = stickAbajo || dpadAbajo;

			botonA =
				gamepad.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS;

			break;
		}
	}

	// =====================
	// 2) SI NO ES GAMEPAD, INTENTO COMO JOYSTICK NORMAL
	// =====================

	if (!controlDetectado) {
		for (int i = GLFW_JOYSTICK_1; i <= GLFW_JOYSTICK_LAST; i++) {
			if (glfwJoystickPresent(i)) {
				controlDetectado = true;

				const char* nombre = glfwGetJoystickName(i);

				static bool avisoJoystick = false;
				if (!avisoJoystick) {
					std::cout << "Control detectado como joystick: "
							  << (nombre ? nombre : "sin nombre")
							  << std::endl;
					avisoJoystick = true;
				}

				int axisCount = 0;
				const float* axes = glfwGetJoystickAxes(i, &axisCount);

				int buttonCount = 0;
				const unsigned char* buttons = glfwGetJoystickButtons(i, &buttonCount);

				// Stick izquierdo vertical.
				// En tu control: arriba debe subir opción y abajo debe bajar opción.
				if (axisCount > 1) {
					float ejeY = axes[1];

					moverArriba = ejeY > deadZone;
					moverAbajo = ejeY < -deadZone;
				}

				// Botón A normalmente es el botón 0.
				if (buttonCount > 0) {
					botonA = buttons[0] == GLFW_PRESS;
				}

				// D-Pad como botones.
				// Dejamos solo una dirección por botón.
				if (buttonCount > 13) {
					// Si tu D-Pad queda invertido, intercambia estos dos bloques.
					bool dpadArriba =
						buttons[11] == GLFW_PRESS ||
						buttons[13] == GLFW_PRESS;

					bool dpadAbajo =
						buttons[10] == GLFW_PRESS ||
						buttons[12] == GLFW_PRESS;

					if (dpadArriba) {
						moverArriba = false;
						moverAbajo = true;
					}

					if (dpadAbajo) {
						moverAbajo = false;
						moverArriba = true;
					}
				}

				break;
			}
		}
	}

	// =====================
	// DEBUG DE DETECCIÓN
	// =====================

	static double ultimoDebug = 0.0;
	double ahora = glfwGetTime();

	if (!controlDetectado) {
		if (ahora - ultimoDebug > 1.0) {
			std::cout << "MENU XBOX: no se detecta ningun control." << std::endl;
			ultimoDebug = ahora;
		}

		return;
	}

	// =====================
	// LIBERAR MOVIMIENTO
	// =====================
	// No vuelve a mover hasta que el stick/D-Pad regrese a neutral.

	bool direccionNeutral = !moverArriba && !moverAbajo;

	if (direccionNeutral) {
		direccionPresionada = false;
	}

	// =====================
	// NAVEGACIÓN DEL MENÚ
	// =====================

	if (!direccionPresionada && moverArriba) {
		opcionMenu--;

		if (opcionMenu < 0) {
			opcionMenu = 2;
		}

		direccionPresionada = true;

		std::cout << "MENU CONTROL: opcion arriba -> " << opcionMenu << std::endl;
	}
	else if (!direccionPresionada && moverAbajo) {
		opcionMenu++;

		if (opcionMenu > 2) {
			opcionMenu = 0;
		}

		direccionPresionada = true;

		std::cout << "MENU CONTROL: opcion abajo -> " << opcionMenu << std::endl;
	}

	// =====================
	// SELECCIÓN CON A
	// =====================

	if (botonA && !botonAPresionado) {
		std::cout << "MENU CONTROL: A presionado en opcion " << opcionMenu << std::endl;

		if (opcionMenu == 0) {
			detenerMusicaMenu();
			reproducirSonidoMisionAgenteP();

			mostrarMenu = false;
			mostrarSecuencia = true;
			juegoPausado = false;

			imagenSecuenciaActual = 0;
			tiempoImagenSecuencia = 0.0f;
		}
		else if (opcionMenu == 1) {
			mostrarControles = true;
			botonAPresionado = true;

			std::cout << "Pantalla de controles abierta con Xbox." << std::endl;
		}
		else if (opcionMenu == 2) {
			detenerMusicaMenu();
			exitApp = true;
		}
	}

	botonAPresionado = botonA;
}
// =====================
// LOOP
// =====================

void applicationLoop() {
	bool continuar = true;

	lastTime = TimeManager::Instance().GetTime();

	while (continuar) {
		currTime = TimeManager::Instance().GetTime();

		if (currTime - lastTime < 0.016666667) {
			glfwPollEvents();
			continue;
		}

		lastTime = currTime;
		TimeManager::Instance().CalculateFrameRate(true);
		deltaTime = TimeManager::Instance().DeltaTime;
		continuar = processInput(true);

		if (!continuar) {
			break;
		}

		if (!mostrarMenu && !mostrarSecuencia && !juegoPausado && !tiempoTerminado && !juegoGanado) {
			actualizarDiaNoche(deltaTime);
		}

		if (!mostrarMenu && !mostrarSecuencia && !juegoPausado && !juegoGanado && contadorActivo) {
			tiempoRestante -= deltaTime;

			if (tiempoRestante <= 0.0f) {
				tiempoRestante = 0.0f;
				contadorActivo = false;
				tiempoTerminado = true;
				juegoPausado = false;
			}
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// =====================
		// MENU PRINCIPAL
		// =====================

		if (mostrarMenu) {
			reproducirMusicaMenu();

			glfwPollEvents();
			procesarXboxMenuInicialDirecto();

			// =====================
			// PANTALLA DE CONTROLES
			// =====================
			if (mostrarControles) {
				glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glDisable(GL_DEPTH_TEST);
				glDisable(GL_CULL_FACE);
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				shaderTexture.turnOn();

				glm::mat4 projectionMenu = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
				glm::mat4 viewMenu = glm::mat4(1.0f);

				shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionMenu));
				shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewMenu));
				shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
				shaderTexture.setInt("texture1", 0);

				glActiveTexture(GL_TEXTURE0);

				glm::mat4 modelControles = glm::scale(
					glm::mat4(1.0f),
					glm::vec3(-2.0f, 2.0f, 1.0f)
				);

				glBindTexture(GL_TEXTURE_2D, textureControlesID);
				boxMenuFondo.render(modelControles);
				glBindTexture(GL_TEXTURE_2D, 0);

				double mouseX, mouseY;
				glfwGetCursorPos(window, &mouseX, &mouseY);

				int widthWindow, heightWindow;
				glfwGetWindowSize(window, &widthWindow, &heightWindow);

				float x = (mouseX / widthWindow) * 2.0f - 1.0f;
				float y = 1.0f - (mouseY / heightWindow) * 2.0f;

				// Area del boton regresar
				bool sobreRegresarControles = (
					x >= 0.20f && x <= 0.90f &&
					y >= -0.92f && y <= -0.68f
				);

				glm::mat4 modelBotonRegresarControles = glm::mat4(1.0f);
				modelBotonRegresarControles = glm::translate(
					modelBotonRegresarControles,
					glm::vec3(0.55f, -0.80f, 0.1f)
				);
				modelBotonRegresarControles = glm::scale(
					modelBotonRegresarControles,
					glm::vec3(-0.35f, 0.12f, 1.0f)
				);

				glBindTexture(
					GL_TEXTURE_2D,
					sobreRegresarControles ? textureBotonRegresarHoverID : textureBotonRegresarID
				);

				boxBotonRegresar.render(modelBotonRegresarControles);
				glBindTexture(GL_TEXTURE_2D, 0);

				glDisable(GL_BLEND);
				glEnable(GL_DEPTH_TEST);
				glEnable(GL_CULL_FACE);

				glfwSwapBuffers(window);
				glfwPollEvents();
				continue;
			}

			glClearColor(0.0f, 0.0f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shaderTexture.turnOn();

			glm::mat4 projectionMenu = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
			glm::mat4 viewMenu = glm::mat4(1.0f);

			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionMenu));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewMenu));
			shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
			shaderTexture.setInt("texture1", 0);

			glActiveTexture(GL_TEXTURE0);

			glm::mat4 modelFondo = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
			glBindTexture(GL_TEXTURE_2D, textureMenuFondoID);
			boxMenuFondo.render(modelFondo);

			glm::mat4 modelBotonIniciar = glm::mat4(1.0f);
			modelBotonIniciar = glm::translate(modelBotonIniciar, glm::vec3(0.0f, 0.25f, 0.1f));
			modelBotonIniciar = glm::scale(modelBotonIniciar, glm::vec3(0.75f, 0.18f, 1.0f));
			glBindTexture(GL_TEXTURE_2D, opcionMenu == 0 ? textureBotonIniciarHoverID : textureBotonIniciarID);
			boxBotonIniciar.render(modelBotonIniciar);

			glm::mat4 modelBotonControles = glm::mat4(1.0f);
			modelBotonControles = glm::translate(modelBotonControles, glm::vec3(0.0f, -0.05f, 0.1f));
			modelBotonControles = glm::scale(modelBotonControles, glm::vec3(0.75f, 0.18f, 1.0f));
			glBindTexture(GL_TEXTURE_2D, opcionMenu == 1 ? textureBotonControlesHoverID : textureBotonControlesID);
			boxBotonControles.render(modelBotonControles);

			glm::mat4 modelBotonSalir = glm::mat4(1.0f);
			modelBotonSalir = glm::translate(modelBotonSalir, glm::vec3(0.0f, -0.35f, 0.1f));
			modelBotonSalir = glm::scale(modelBotonSalir, glm::vec3(0.75f, 0.18f, 1.0f));
			glBindTexture(GL_TEXTURE_2D, opcionMenu == 2 ? textureBotonSalirHoverID : textureBotonSalirID);
			boxBotonSalir.render(modelBotonSalir);

			glBindTexture(GL_TEXTURE_2D, 0);

			glfwSwapBuffers(window);
			glfwPollEvents();
			continue;
		}

		// =====================
		// SECUENCIA INTRO
		// =====================

		if (mostrarSecuencia) {
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			shaderTexture.turnOn();

			glm::mat4 projectionSecuencia = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
			glm::mat4 viewSecuencia = glm::mat4(1.0f);

			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionSecuencia));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewSecuencia));
			shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
			shaderTexture.setInt("texture1", 0);

			tiempoImagenSecuencia += deltaTime;

			if (tiempoImagenSecuencia >= 9.0f) {
				tiempoImagenSecuencia = 0.0f;
				imagenSecuenciaActual++;
			}

			if (imagenSecuenciaActual > 2) {
				mostrarSecuencia = false;
				contadorActivo = true;
				reproducirAudioFeria();
			}
			else {
				glActiveTexture(GL_TEXTURE0);

				if (imagenSecuenciaActual == 0) {
					glBindTexture(GL_TEXTURE_2D, textureSecuencia1ID);
				}
				else if (imagenSecuenciaActual == 1) {
					glBindTexture(GL_TEXTURE_2D, textureSecuencia2ID);
				}
				else {
					glBindTexture(GL_TEXTURE_2D, textureSecuencia3ID);
				}

				glm::mat4 modelSecuencia = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 1.0f));
				boxMenuFondo.render(modelSecuencia);

				glfwSwapBuffers(window);
				glfwPollEvents();
				continue;
			}
		}

		// =====================
		// UPDATE
		// =====================

		if (!juegoPausado && !tiempoTerminado && !juegoGanado) {
			actualizarPerry(window, deltaTime);
			sceneManager.update(deltaTime);
		}

		// =====================
		// CAMARA
		// =====================

		glm::mat4 projection = glm::perspective(
			glm::radians(45.0f),
			static_cast<float>(screenWidth) / static_cast<float>(screenHeight),
			0.01f,
			300.0f
		);

		glm::mat4 view = obtenerVistaCamaraPerry();

		// =====================
		// RENDER 3D
		// =====================

		sceneManager.render(projection, view);

		if (!tiempoTerminado && !camaraPrimeraPersona) {
			renderPerry();
		}

		// =====================
		// GAME OVER
		// =====================

		if (tiempoTerminado) {
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shaderTexture.turnOn();

			glm::mat4 projectionFinal = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
			glm::mat4 viewFinal = glm::mat4(1.0f);

			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionFinal));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewFinal));
			shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
			shaderTexture.setInt("texture1", 0);

			glActiveTexture(GL_TEXTURE0);

			glm::mat4 modelFinal = glm::scale(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, 1.0f));
			glBindTexture(GL_TEXTURE_2D, textureTiempoTerminadoID);
			boxTiempoTerminado.render(modelFinal);

			// =====================
			// TEXTOS DE RESULTADOS
			// =====================

			int segundosRestantes = static_cast<int>(tiempoRestante);

			int puntosObjetos = objetosRecolectados * 1000;
			int puntosTiempo = segundosRestantes * 10;
			int puntosPerdidos = vecesDetectado * 100;

			int puntuacionFinal = puntosObjetos + puntosTiempo - puntosPerdidos;

			if (puntuacionFinal < 0) {
				puntuacionFinal = 0;
			}

			glUseProgram(0);
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			textoPantalla->render(
				"Objetos recolectados: " + std::to_string(objetosRecolectados) + "/" + std::to_string(objetosTotales),
				-0.45f,
				0.35f,
				22,
				1.0f,
				1.0f,
				1.0f
			);

			textoPantalla->render(
				"Tiempo restante: " + std::to_string(segundosRestantes) + " s",
				-0.45f,
				0.22f,
				22,
				1.0f,
				1.0f,
				1.0f
			);

			textoPantalla->render(
				"Veces detectado: " + std::to_string(vecesDetectado),
				-0.45f,
				0.09f,
				22,
				1.0f,
				1.0f,
				1.0f
			);

			textoPantalla->render(
				"Puntuacion final: " + std::to_string(puntuacionFinal),
				-0.45f,
				-0.04f,
				26,
				1.0f,
				1.0f,
				0.0f
			);

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			int widthWindow, heightWindow;
			glfwGetWindowSize(window, &widthWindow, &heightWindow);

			float x = (mouseX / widthWindow) * 2.0f - 1.0f;
			float y = 1.0f - (mouseY / heightWindow) * 2.0f;

			bool sobreSalir = (
				x >= -0.75f && x <= 0.75f &&
				y >= -0.25f && y <= -0.05f
			);

			glm::mat4 modelBotonSalirFinal = glm::mat4(1.0f);
			modelBotonSalirFinal = glm::translate(modelBotonSalirFinal, glm::vec3(0.0f, -0.15f, 0.1f));
			modelBotonSalirFinal = glm::scale(modelBotonSalirFinal, glm::vec3(0.75f, 0.18f, 1.0f));

			glBindTexture(GL_TEXTURE_2D, sobreSalir ? textureBotonSalirFinalHoverID : textureBotonSalirFinalID);
			boxBotonSalirFinal.render(modelBotonSalirFinal);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			glfwSwapBuffers(window);
			glfwPollEvents();
			continue;
		}
		if (juegoGanado) {
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shaderTexture.turnOn();

			glm::mat4 projectionFinal = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
			glm::mat4 viewFinal = glm::mat4(1.0f);

			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionFinal));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewFinal));
			shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
			shaderTexture.setInt("texture1", 0);

			glActiveTexture(GL_TEXTURE0);

			glm::mat4 modelFinal = glm::scale(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, 1.0f));

			// Aquí va la imagen diferente de victoria
			glBindTexture(GL_TEXTURE_2D, textureVictoriaID);
			boxTiempoTerminado.render(modelFinal);

			// =====================
			// TEXTOS DE RESULTADOS
			// =====================

			int segundosRestantes = static_cast<int>(tiempoRestante);

			int puntosObjetos = objetosRecolectados * 1000;
			int puntosTiempo = segundosRestantes * 10;
			int puntosPerdidos = vecesDetectado * 100;

			int puntuacionFinal = puntosObjetos + puntosTiempo - puntosPerdidos;

			if (puntuacionFinal < 0) {
				puntuacionFinal = 0;
			}

			glUseProgram(0);
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			textoPantalla->render(
				"Objetos recolectados: " + std::to_string(objetosRecolectados) + "/" + std::to_string(objetosTotales),
				-0.45f,
				0.35f,
				22,
				1.0f,
				1.0f,
				1.0f
			);

			textoPantalla->render(
				"Tiempo restante: " + std::to_string(segundosRestantes) + " s",
				-0.45f,
				0.22f,
				22,
				1.0f,
				1.0f,
				1.0f
			);

			textoPantalla->render(
				"Veces detectado: " + std::to_string(vecesDetectado),
				-0.45f,
				0.09f,
				22,
				1.0f,
				1.0f,
				1.0f
			);

			textoPantalla->render(
				"Puntuacion final: " + std::to_string(puntuacionFinal),
				-0.45f,
				-0.04f,
				26,
				1.0f,
				1.0f,
				0.0f
			);

			double mouseX, mouseY;
			glfwGetCursorPos(window, &mouseX, &mouseY);

			int widthWindow, heightWindow;
			glfwGetWindowSize(window, &widthWindow, &heightWindow);

			float x = (mouseX / widthWindow) * 2.0f - 1.0f;
			float y = 1.0f - (mouseY / heightWindow) * 2.0f;

			bool sobreSalir = (
				x >= -0.75f && x <= 0.75f &&
				y >= -0.25f && y <= -0.05f
			);

			glm::mat4 modelBotonSalirFinal = glm::mat4(1.0f);
			modelBotonSalirFinal = glm::translate(modelBotonSalirFinal, glm::vec3(0.0f, -0.15f, 0.1f));
			modelBotonSalirFinal = glm::scale(modelBotonSalirFinal, glm::vec3(0.75f, 0.18f, 1.0f));

			glBindTexture(GL_TEXTURE_2D, sobreSalir ? textureBotonSalirFinalHoverID : textureBotonSalirFinalID);
			boxBotonSalirFinal.render(modelBotonSalirFinal);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			glfwSwapBuffers(window);
			glfwPollEvents();
			continue;
		}
		

		// =====================
		// PAUSA
		// =====================

		if (juegoPausado) {
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			shaderTexture.turnOn();

			glm::mat4 projectionPausa = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
			glm::mat4 viewPausa = glm::mat4(1.0f);

			shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionPausa));
			shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewPausa));
			shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
			shaderTexture.setInt("texture1", 0);

			glm::mat4 modelPausa = glm::scale(glm::mat4(1.0f), glm::vec3(-2.0f, 2.0f, 1.0f));

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, texturePausaID);
			boxPausa.render(modelPausa);
			glBindTexture(GL_TEXTURE_2D, 0);

			glDisable(GL_BLEND);
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);

			glfwSwapBuffers(window);
			glfwPollEvents();
			continue;
		}

		// =====================
		// HUD TIEMPO
		// =====================

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::string textoTiempo = formatoTiempo(tiempoRestante);

		if (tiempoRestante <= 300.0f) {
			textoPantalla->render(textoTiempo, 0.35f, 0.85f, 20, 1.0f, 0.0f, 0.0f);
		}
		else {
			textoPantalla->render(textoTiempo, 0.35f, 0.85f, 20, 1.0f, 1.0f, 1.0f);
		}

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// =====================
		// HUD ICONO
		// =====================

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		shaderTexture.turnOn();

		glm::mat4 projectionIcono = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glm::mat4 viewIcono = glm::mat4(1.0f);

		shaderTexture.setMatrix4("projection", 1, false, glm::value_ptr(projectionIcono));
		shaderTexture.setMatrix4("view", 1, false, glm::value_ptr(viewIcono));
		shaderTexture.setVectorFloat2("scaleUV", glm::value_ptr(glm::vec2(1.0f)));
		shaderTexture.setInt("texture1", 0);

		glm::mat4 modelIcono = glm::mat4(1.0f);
		modelIcono = glm::translate(modelIcono, glm::vec3(-0.82f, 0.78f, 0.0f));
		modelIcono = glm::scale(modelIcono, glm::vec3(0.15f, 0.15f, 1.0f));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureIconoID);
		boxIcono.render(modelIcono);
		glBindTexture(GL_TEXTURE_2D, 0);

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		// =====================
		// HUD OBJETOS Y MENSAJES
		// =====================

		glUseProgram(0);
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		std::string textoObjetos = std::to_string(objetosRecolectados) + "/" + std::to_string(objetosTotales);
		textoPantalla->render(textoObjetos, -0.72f, 0.82f, 24, 1.0f, 1.0f, 1.0f);
		std::string escenaActualHUD = sceneManager.getActiveSceneName();

		if (escenaActualHUD == "Pueblo" && jugadorCercaDelCasino()) {
			textoPantalla->render(
				"Presiona 2 o Y para entrar al casino",
				-0.35f,
				0.92f,
				22,
				1.0f,
				1.0f,
				1.0f
			);
		}
		if (escenaActualHUD == "Pueblo" && jugadorCercaDelBoliche()) {
			textoPantalla->render(
				"Presiona 3 o RB para entrar al boliche",
				-0.35f,
				0.92f,
				22,
				1.0f,
				1.0f,
				1.0f
			);
		}

		//salida de casino
		if (std::string(sceneManager.getActiveSceneName()) == "Casino" && jugadorCercaSalidaCasino()) {
			textoPantalla->render(
				"Presiona 1 o B para volver al pueblo",
				-0.35f,
				0.92f,
				22,
				1.0f,
				1.0f,
				1.0f
			);
		}
		if (escenaActualHUD == "Bolos" && jugadorCercaSalidaBoliche()) {
			textoPantalla->render(
				"Presiona 1 o B para volver al pueblo",
				-0.35f,
				0.92f,
				22,
				1.0f,
				1.0f,
				1.0f
			);
		}

		if (mostrarMensajeInteraccion) {
			textoPantalla->render(
				mensajeInteraccion,
				-0.35f,
				-0.65f,
				22,
				1.0f,
				1.0f,
				1.0f
			);
		}

		glDisable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}
// =====================
// DESTROY
// =====================

void destroy() {
	if (textoPantalla != nullptr) {
		delete textoPantalla;
		textoPantalla = nullptr;
	}

	destruirAudioFeria();
	destruirAudio();
	destruirAudioCasino();
	destruirAudioVictoria();
	destruirAudioBoliche();
	destruirSonidoEntradaBoliche();

	Iglesia.destroy();
	Portales.destroy();
	chaparrin.destroy();
	rojita.destroy();
	morado.destroy();
	rosa.destroy();
	aqua.destroy();
	Centro.destroy();
	Casino.destroy();
	Boliche.destroy();
	PerryIdle.destroy();
	PerryCaminar.destroy();
	Recolectable1.destroy();
	Recolectable2.destroy();

	destruirCollidersDebug();
	limpiarMeshColliders();

	skyboxSphere.destroy();
	boxCesped.destroy();

	terrain.destroy();
	boxMenuFondo.destroy();
	boxBotonIniciar.destroy();
	boxBotonControles.destroy();
	boxBotonSalir.destroy();
	boxIcono.destroy();
	boxPausa.destroy();
	boxTiempoTerminado.destroy();
	boxBotonSalirFinal.destroy();
	boxBotonRegresar.destroy();

	eliminarTextura(textureCespedID);
	eliminarTextura(skyboxDiaTextureID);
	eliminarTextura(skyboxNocheTextureID);

	eliminarTextura(textureTerrenoBaseID);
	eliminarTextura(textureRID);
	eliminarTextura(textureGID);
	eliminarTextura(textureBID);
	eliminarTextura(textureBlendMapID);

	eliminarTextura(textureMenuFondoID);
	eliminarTextura(textureBotonIniciarID);
	eliminarTextura(textureBotonIniciarHoverID);
	eliminarTextura(textureBotonControlesID);
	eliminarTextura(textureBotonControlesHoverID);
	eliminarTextura(textureBotonSalirID);
	eliminarTextura(textureBotonSalirHoverID);
	eliminarTextura(textureIconoID);
	eliminarTextura(texturePausaID);
	eliminarTextura(textureTiempoTerminadoID);
	eliminarTextura(textureBotonSalirFinalID);
	eliminarTextura(textureBotonSalirFinalHoverID);
	eliminarTextura(textureSecuencia1ID);
	eliminarTextura(textureSecuencia2ID);
	eliminarTextura(textureSecuencia3ID);

	shader.destroy();
	shaderMulLighting.destroy();
	shaderSkybox.destroy();
	shaderTexture.destroy();
	shaderTerreno.destroy();
	shaderFresnel.destroy();

	if (window != nullptr) {
		glfwDestroyWindow(window);
		window = nullptr;
	}

	glfwTerminate();
}

// =====================
// CALLBACKS
// =====================

void reshapeCallback(GLFWwindow* Window, int widthRes, int heightRes) {
	screenWidth = widthRes;
	screenHeight = heightRes;
	glViewport(0, 0, widthRes, heightRes);
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (action != GLFW_PRESS) {
		return;
	}

	if (mostrarMenu || mostrarSecuencia) {
		if (key == GLFW_KEY_ESCAPE) {
			exitApp = true;
		}

		return;
	}
	if (juegoGanado) {
    	return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
		exitApp = true;
		break;

	case GLFW_KEY_1:
	{
		std::string escenaActual = sceneManager.getActiveSceneName();

		if (escenaActual == "Casino") {
			if (jugadorCercaSalidaCasino()) {
				detenerAudioCasino();
				sceneManager.setScene(0);

				if (!mostrarMenu && !mostrarSecuencia) {
					reproducirAudioFeria();
				}
			}
			else {
				std::cout << "Acercate a la salida del casino para volver al pueblo." << std::endl;
			}
		}
		else if (escenaActual == "Bolos") {
			if (jugadorCercaSalidaBoliche()) {
				sceneManager.setScene(0);

				if (!mostrarMenu && !mostrarSecuencia) {
					reproducirAudioFeria();
				}
			}
			else {
				std::cout << "Acercate a la salida del boliche para volver al pueblo." << std::endl;
			}
		}
		else {
			sceneManager.setScene(0);

			if (!mostrarMenu && !mostrarSecuencia) {
				reproducirAudioFeria();
			}
		}

		break;
	}

	case GLFW_KEY_2:
	{
		double ahora = glfwGetTime();

		if (ahora - tiempoUltimoCambioEscena < 1.0) {
			break;
		}
		// salida
		std::string escenaActual = sceneManager.getActiveSceneName();

		if (escenaActual == "Pueblo" && jugadorCercaDelCasino()) {
			tiempoUltimoCambioEscena = ahora;
			detenerAudioFeria();
			sceneManager.setScene(1);
		}

		break;
	}

	case GLFW_KEY_3:
	{
		double ahora = glfwGetTime();

		if (ahora - tiempoUltimoCambioEscena < 1.0) {
			break;
		}

		std::string escenaActual = sceneManager.getActiveSceneName();

		if (escenaActual == "Pueblo" && jugadorCercaDelBoliche()) {
			tiempoUltimoCambioEscena = ahora;
			detenerAudioFeria();
			detenerAudioCasino();
			reproducirAudioBoliche();
			reproducirSonidoEntradaBoliche();
			sceneManager.setScene(2);
		}

		break;
	}

	default:
		break;
	}
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
	offsetX = xpos - lastMousePosX;
	offsetY = ypos - lastMousePosY;

	lastMousePosX = xpos;
	lastMousePosY = ypos;
}

void mouseButtonCallback(GLFWwindow* window, int button, int state, int mod) {
	if (state != GLFW_PRESS) {
		return;
	}

	switch (button) {
	case GLFW_MOUSE_BUTTON_RIGHT:
		std::cout << "lastMousePos.y: " << lastMousePosY << std::endl;
		break;

	case GLFW_MOUSE_BUTTON_LEFT:
		std::cout << "lastMousePos.x: " << lastMousePosX << std::endl;
		break;

	case GLFW_MOUSE_BUTTON_MIDDLE:
		std::cout << "lastMousePos.x: " << lastMousePosX << std::endl;
		std::cout << "lastMousePos.y: " << lastMousePosY << std::endl;
		break;

	default:
		break;
	}
}


// =====================
// INPUT
// =====================

bool processInput(bool continueApplication) {
	if (exitApp || glfwWindowShouldClose(window) != 0) {
		return false;
	}

	// Xbox se procesa una vez por frame para menú, secuencia, pausa y game over.
	procesarControlXbox();

	// =====================
	// GAME OVER
	// =====================

	if (tiempoTerminado || juegoGanado) {

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		int widthWindow, heightWindow;
		glfwGetWindowSize(window, &widthWindow, &heightWindow);

		float x = (mouseX / widthWindow) * 2.0f - 1.0f;
		float y = 1.0f - (mouseY / heightWindow) * 2.0f;

		bool sobreSalir = (
			x >= -0.75f && x <= 0.75f &&
			y >= -0.25f && y <= -0.05f
		);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePresionadoGameOver) {
			if (sobreSalir) {
				exitApp = true;
			}

			mousePresionadoGameOver = true;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			mousePresionadoGameOver = false;
		}

		glfwPollEvents();
		return true;
	}
	// =====================
	// INPUT PANTALLA DE CONTROLES
	// =====================

	if (mostrarMenu && mostrarControles) {
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		int widthWindow, heightWindow;
		glfwGetWindowSize(window, &widthWindow, &heightWindow);

		float x = (mouseX / widthWindow) * 2.0f - 1.0f;
		float y = 1.0f - (mouseY / heightWindow) * 2.0f;

		bool sobreRegresarControles = (
			x >= 0.20f && x <= 0.90f &&
			y >= -0.92f && y <= -0.68f
		);

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePresionadoMenu) {
			if (sobreRegresarControles) {
				mostrarControles = false;
				opcionMenu = 0;
			}

			mousePresionadoMenu = true;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			mousePresionadoMenu = false;
		}

		glfwPollEvents();
		return true;
	}

	// =====================
	// MENÚ PRINCIPAL
	// =====================

	if (mostrarMenu) {
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS && !teclaMenuPresionada) {
			opcionMenu++;

			if (opcionMenu > 2) {
				opcionMenu = 0;
			}

			teclaMenuPresionada = true;
		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS && !teclaMenuPresionada) {
			opcionMenu--;

			if (opcionMenu < 0) {
				opcionMenu = 2;
			}

			teclaMenuPresionada = true;
		}

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !teclaMenuPresionada) {
			if (opcionMenu == 0) {
				detenerMusicaMenu();
				reproducirSonidoMisionAgenteP();

				mostrarMenu = false;
				mostrarSecuencia = true;
				juegoPausado = false;

				imagenSecuenciaActual = 0;
				tiempoImagenSecuencia = 0.0f;
			}
			else if (opcionMenu == 1) {
				mostrarControles = true;
			}
			else if (opcionMenu == 2) {
				detenerMusicaMenu();
				exitApp = true;
			}

			teclaMenuPresionada = true;
		}

		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_RELEASE &&
			glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
			teclaMenuPresionada = false;
		}

		// Mouse del menú: solo actualiza si realmente se mueve.
		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);

		static double mouseAnteriorX = -9999.0;
		static double mouseAnteriorY = -9999.0;

		bool mouseSeMovio =
			std::abs(mouseX - mouseAnteriorX) > 2.0 ||
			std::abs(mouseY - mouseAnteriorY) > 2.0;

		mouseAnteriorX = mouseX;
		mouseAnteriorY = mouseY;

		int widthWindow, heightWindow;
		glfwGetWindowSize(window, &widthWindow, &heightWindow);

		float x = (mouseX / widthWindow) * 2.0f - 1.0f;
		float y = 1.0f - (mouseY / heightWindow) * 2.0f;

		if (mouseSeMovio) {
			if (x >= -0.75f && x <= 0.75f && y >= 0.16f && y <= 0.34f) {
				opcionMenu = 0;
			}
			else if (x >= -0.75f && x <= 0.75f && y >= -0.14f && y <= 0.04f) {
				opcionMenu = 1;
			}
			else if (x >= -0.75f && x <= 0.75f && y >= -0.44f && y <= -0.26f) {
				opcionMenu = 2;
			}
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !mousePresionadoMenu) {
			if (opcionMenu == 0) {
				detenerMusicaMenu();
				reproducirSonidoMisionAgenteP();

				mostrarMenu = false;
				mostrarSecuencia = true;
				juegoPausado = false;

				imagenSecuenciaActual = 0;
				tiempoImagenSecuencia = 0.0f;
			}
			else if (opcionMenu == 1) {
				mostrarControles = true;
			}
			else if (opcionMenu == 2) {
				detenerMusicaMenu();
				exitApp = true;
			}

			mousePresionadoMenu = true;
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
			mousePresionadoMenu = false;
		}

		glfwPollEvents();
		return true;
	}

	// =====================
	// SECUENCIA
	// =====================

	if (mostrarSecuencia) {
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS && !teclaMenuPresionada) {
			imagenSecuenciaActual++;
			tiempoImagenSecuencia = 0.0f;

			if (imagenSecuenciaActual > 2) {
				mostrarSecuencia = false;
				contadorActivo = true;
				reproducirAudioFeria();
			}

			teclaMenuPresionada = true;
		}

		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_RELEASE) {
			teclaMenuPresionada = false;
		}

		glfwPollEvents();
		return true;
	}

	// =====================
	// PAUSA
	// =====================

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS && !teclaPausaPresionada) {
		juegoPausado = !juegoPausado;
		teclaPausaPresionada = true;
	}

	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_RELEASE) {
		teclaPausaPresionada = false;
	}

	if (juegoPausado) {
		glfwPollEvents();
		return true;
	}
	// =====================
	// CONTROL XBOX: ENTRAR AL CASINO
	// Botón Y para entrar al casino si Perry está cerca
	// =====================

	static bool botonYCasinoPresionado = false;

	bool botonYControl = false;

	GLFWgamepadstate gamepad;

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad)) {

		if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_Y] == GLFW_PRESS) {
			botonYControl = true;
		}
	}

	if (botonYControl && !botonYCasinoPresionado) {
		double ahora = glfwGetTime();

		if (ahora - tiempoUltimoCambioEscena >= 1.0) {
			std::string escenaActual = sceneManager.getActiveSceneName();
			if (escenaActual == "Pueblo" && jugadorCercaDelCasino()) {
				tiempoUltimoCambioEscena = ahora;
				detenerAudioFeria();
				sceneManager.setScene(1);
			}
		}

		botonYCasinoPresionado = true;
	}

	if (!botonYControl) {
		botonYCasinoPresionado = false;
	}
	// =====================
	// CONTROL XBOX: ENTRAR AL BOLICHE
	// Botón RB para entrar al boliche si Perry está cerca
	// =====================

	static bool botonRBBolichePresionado = false;

	bool botonRBControl = false;

	GLFWgamepadstate gamepadRB;

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadRB)) {

		if (gamepadRB.buttons[GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER] == GLFW_PRESS) {
			botonRBControl = true;
		}
	}

	if (botonRBControl && !botonRBBolichePresionado) {
		double ahora = glfwGetTime();

		if (ahora - tiempoUltimoCambioEscena >= 1.0) {
			std::string escenaActual = sceneManager.getActiveSceneName();

			if (escenaActual == "Pueblo" && jugadorCercaDelBoliche()) {
				tiempoUltimoCambioEscena = ahora;
				detenerAudioFeria();
				detenerAudioCasino();
				reproducirAudioBoliche();
				reproducirSonidoEntradaBoliche();
				sceneManager.setScene(2);
			}
		}

		botonRBBolichePresionado = true;
	}

	if (!botonRBControl) {
		botonRBBolichePresionado = false;
	}
	// =====================
	// CONTROL XBOX: VOLVER AL PUEBLO DESDE CASINO
	// Botón B para salir del casino si Perry está cerca de la salida
	// =====================

	static bool botonBSalidaCasinoPresionado = false;

	bool botonBControl = false;

	GLFWgamepadstate gamepadB;

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepadB)) {

		if (gamepadB.buttons[GLFW_GAMEPAD_BUTTON_B] == GLFW_PRESS) {
			botonBControl = true;
		}
	}

	if (botonBControl && !botonBSalidaCasinoPresionado) {
		double ahora = glfwGetTime();

		if (ahora - tiempoUltimoCambioEscena >= 1.0) {
			std::string escenaActual = sceneManager.getActiveSceneName();

			if (escenaActual == "Casino" && jugadorCercaSalidaCasino()) {
				tiempoUltimoCambioEscena = ahora;

				detenerAudioCasino();
				sceneManager.setScene(0);

				if (!mostrarMenu && !mostrarSecuencia) {
					reproducirAudioFeria();
				}
			}
			else if (escenaActual == "Bolos" && jugadorCercaSalidaBoliche()) {
				tiempoUltimoCambioEscena = ahora;

				sceneManager.setScene(0);

				if (!mostrarMenu && !mostrarSecuencia) {
					reproducirAudioFeria();
				}
			}
		}
		
		botonBSalidaCasinoPresionado = true;
	}

	if (!botonBControl) {
		botonBSalidaCasinoPresionado = false;
	}

	offsetX = 0.0f;
	offsetY = 0.0f;

	glfwPollEvents();
	return continueApplication;
}
