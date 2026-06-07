#pragma once

/*
 * Archivo: Globals.h
 * Descripción:
 * Declara las variables globales utilizadas por el proyecto final.
 *
 * Este archivo centraliza recursos compartidos entre escenas y módulos,
 * como ventana, shaders, cámara, modelos, texturas, audio, menú, HUD,
 * personaje principal, sistema de día/noche, terreno y colisiones.
 *
 * Las variables aquí declaradas se definen normalmente en Globals.cpp
 * y son utilizadas por Application.cpp, RenderUtils.cpp y las escenas
 * principales del proyecto.
 */

#define _USE_MATH_DEFINES
#define GLM_FORCE_RADIANS
#define SDL_MAIN_HANDLED

#include <cmath>
#include <map>
#include <memory>
#include <string>
#include <tuple>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "Headers/Colisiones.h"
#include "Headers/Shader.h"
#include "Headers/Sphere.h"
#include "Headers/Box.h"
#include "Headers/FirstPersonCamera.h"
#include "Headers/Model.h"
#include "Headers/FontTypeRendering.h"
#include "Headers/Terrain.h"


// =====================
// VENTANA
// =====================

extern int screenWidth;
extern int screenHeight;
extern GLFWwindow* window;


// =====================
// SHADERS
// =====================

extern Shader shader;
extern Shader shaderSkybox;
extern Shader shaderMulLighting;
extern Shader shaderTexture;
extern Shader shaderTerreno;
extern Shader shaderFresnel;


// =====================
// CÁMARA
// =====================

extern std::shared_ptr<FirstPersonCamera> camera;


// =====================
// OBJETOS BASE DE RENDER
// =====================

extern Sphere skyboxSphere;
extern Box boxCesped;


// =====================
// MODELOS DE ESCENAS
// =====================

extern Model Iglesia;
extern Model Portales;
extern Model chaparrin;
extern Model rojita;
extern Model morado;
extern Model rosa;
extern Model aqua;
extern Model Centro;

extern Model Casino;
extern Model Boliche;

extern bool modelosPuebloCargados;
extern bool modelosCasinoCargados;
extern bool modelosBolosCargados;

extern Model Enemigo;
extern bool modeloEnemigoCargado;
// enemigo casino
extern Model EnemigoCasino;
extern bool modeloEnemigoCasinoCargado;



// =====================
// TEXTURAS GENERALES
// =====================

extern GLuint textureCespedID;
extern GLuint skyboxTextureID;

extern GLenum types[6];
extern std::string fileNames[6];


// =====================
// TERRENO
// =====================

extern Terrain terrain;

extern GLuint textureTerrenoBaseID;
extern GLuint textureRID;
extern GLuint textureGID;
extern GLuint textureBID;
extern GLuint textureBlendMapID;


// =====================
// CONTROL GENERAL DE APLICACIÓN
// =====================

extern bool exitApp;

extern int lastMousePosX;
extern int lastMousePosY;
extern int offsetX;
extern int offsetY;

extern double deltaTime;
extern double currTime;
extern double lastTime;

extern const float ESCALA_PUEBLO;


// =====================
// MENÚ PRINCIPAL
// =====================

extern bool mostrarMenu;
extern int opcionMenu;
extern bool teclaMenuPresionada;
extern bool mousePresionadoMenu;
extern bool mostrarControles;

extern GLuint textureMenuFondoID;
extern GLuint textureBotonIniciarID;
extern GLuint textureBotonIniciarHoverID;
extern GLuint textureBotonControlesID;
extern GLuint textureBotonControlesHoverID;
extern GLuint textureBotonSalirID;
extern GLuint textureBotonSalirHoverID;
extern GLuint textureControlesID;
extern GLuint textureBotonRegresarID;
extern GLuint textureBotonRegresarHoverID;

extern Box boxMenuFondo;
extern Box boxBotonIniciar;
extern Box boxBotonControles;
extern Box boxBotonSalir;
extern Box boxBotonRegresar;


// =====================
// INTRO / SECUENCIA INICIAL
// =====================

extern bool mostrarSecuencia;
extern int imagenSecuenciaActual;
extern float tiempoImagenSecuencia;

extern GLuint textureSecuencia1ID;
extern GLuint textureSecuencia2ID;
extern GLuint textureSecuencia3ID;

extern GLuint textureBotonSiguienteID;
extern GLuint textureBotonSiguienteHoverID;
extern GLuint textureBotonJugarID;
extern GLuint textureBotonJugarHoverID;

extern Box boxBotonSecuencia;

extern bool mousePresionadoSecuencia;
extern bool botonSecuenciaHover;


// =====================
// PAUSA
// =====================

extern bool juegoPausado;
extern bool teclaPausaPresionada;

extern GLuint texturePausaID;
extern Box boxPausa;


// =====================
// GAME OVER / TIEMPO TERMINADO
// =====================

extern bool tiempoTerminado;
extern bool juegoGanado;
extern bool mousePresionadoGameOver;

extern GLuint textureTiempoTerminadoID;
extern GLuint textureBotonSalirFinalID;
extern GLuint textureBotonSalirFinalHoverID;

extern Box boxTiempoTerminado;
extern Box boxBotonSalirFinal;

extern GLuint textureVictoriaID;


// =====================
// CONTADOR DE TIEMPO
// =====================

extern FontTypeRendering::FontTypeRendering* textoPantalla;

extern float tiempoInicial;
extern float tiempoRestante;
extern bool contadorActivo;


// =====================
// HUD Y OBJETOS RECOLECTABLES
// =====================

extern GLuint textureIconoID;
extern Box boxIcono;

extern int objetosRecolectados;
extern const int objetosTotales;



// =====================
// SISTEMA DE PUNTUACION / DETECCIONES
// =====================

extern int vecesDetectado;
extern float tiempoDesdeUltimaDeteccion;
extern bool estabaSiendoDetectado;

void registrarDeteccionJugador(bool detectado, float deltaTime);
// =====================
// SISTEMA DE PUNTUACION
// =====================
extern int puntajeFinal;
extern const int puntosPorObjeto;
extern const int puntosPorSegundo;
extern const int penalizacionPorDeteccion;

int calcularPuntajeFinal(int tiempoRestante);



// Modelos recolectables usados en las misiones.
extern Model Recolectable1;
extern Model Recolectable2;
extern Model Recolectable3;

extern bool modeloRecolectable3Cargado;
extern bool modeloRecolectable1Cargado;
extern bool modeloRecolectable2Cargado;

// Mensaje de interacción mostrado cuando Perry está cerca de un objeto.
extern bool mostrarMensajeInteraccion;
extern std::string mensajeInteraccion;


// =====================
// PERSONAJE PRINCIPAL: PERRY
// =====================

extern Model PerryIdle;
extern Model PerryCaminar;

extern bool modeloPerryCargado;

enum class EstadoPerry {
    IDLE,
    CAMINANDO,
    SALTANDO
};

extern EstadoPerry estadoPerry;

extern glm::vec3 posicionPerry;
extern glm::vec3 posicionAnteriorPerry;

extern float rotacionPerryY;

extern bool perryEnSuelo;
extern float velocidadVerticalPerry;

extern const float gravedadPerry;
extern const float fuerzaSaltoPerry;
extern const float velocidadMovimientoPerry;

extern int animacionPerryIdle;
extern int animacionPerryCaminar;
extern int animacionActualPerry;

extern bool camaraPrimeraPersona;
extern bool teclaCamaraPresionada;


// =====================
// SISTEMA DÍA / NOCHE
// =====================

extern GLuint skyboxDiaTextureID;
extern GLuint skyboxNocheTextureID;

extern float tiempoCicloDiaNoche;
extern float duracionCicloDiaNoche;
extern float factorDiaNoche;


// =====================
// AUDIO GENERAL / MENÚ
// =====================

extern Mix_Music* musicaMenu;
extern bool musicaMenuReproduciendo;

extern Mix_Chunk* sonidoMisionAgenteP;


// =====================
// AUDIO FERIA
// =====================

extern Mix_Music* musicaFeria;

extern Mix_Chunk* ambienteFeria1;
extern Mix_Chunk* ambienteFeria2;
extern Mix_Chunk* ambienteFeria3;

extern bool audioFeriaReproduciendo;

extern int canalAmbienteFeria1;
extern int canalAmbienteFeria2;
extern int canalAmbienteFeria3;


// =====================
// AUDIO CASINO
// =====================

extern Mix_Music* musicaCasino;
extern Mix_Chunk* ambienteCasino;
extern Mix_Chunk* sonidoMisionCasino;

extern bool audioCasinoReproduciendo;
extern int canalAmbienteCasino;


// =====================
// COLISIONES BASE
// =====================

extern std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersOBB;
extern std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>> collidersSBB;


// =====================
// COLISIONES POR ESCENA / OBSTÁCULOS
// =====================

extern std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersObstaculosOBB;
extern std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersPisosOBB;

extern bool mostrarDebugColliders;