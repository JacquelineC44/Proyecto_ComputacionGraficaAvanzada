#include "Globals.h"

/*
 * Archivo: Globals.cpp
 * Descripción:
 * Define e inicializa las variables globales declaradas en Globals.h.
 */


// =====================
// VENTANA
// =====================

int screenWidth = 0;
int screenHeight = 0;
GLFWwindow* window = nullptr;


// =====================
// SHADERS
// =====================

Shader shader;
Shader shaderSkybox;
Shader shaderMulLighting;
Shader shaderTexture;
Shader shaderTerreno;
Shader shaderFresnel;


// =====================
// CÁMARA
// =====================

std::shared_ptr<FirstPersonCamera> camera(new FirstPersonCamera());

bool camaraPrimeraPersona = false;
bool teclaCamaraPresionada = false;


// =====================
// OBJETOS BASE DE RENDER
// =====================

Sphere skyboxSphere(20, 20);
Box boxCesped;


// =====================
// MODELOS DE ESCENAS
// =====================

Model Iglesia;
Model Portales;
Model chaparrin;
Model rojita;
Model morado;
Model rosa;
Model aqua;
Model Centro;

Model Casino;
Model Boliche;

Model Enemigo;
bool modeloEnemigoCargado = false;
//enemigo casino
Model EnemigoCasino;
bool modeloEnemigoCasinoCargado = false;
//

bool modelosPuebloCargados = false;
bool modelosCasinoCargados = false;
bool modelosBolosCargados = false;


// =====================
// TEXTURAS GENERALES
// =====================

GLuint textureCespedID = 0;
GLuint skyboxTextureID = 0;

GLenum types[6] = {
    GL_TEXTURE_CUBE_MAP_POSITIVE_X,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
    GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
    GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
};

std::string fileNames[6] = {
    "../Textures/mp_bloodvalley/blood-valley_ft.tga",
    "../Textures/mp_bloodvalley/blood-valley_bk.tga",
    "../Textures/mp_bloodvalley/blood-valley_up.tga",
    "../Textures/mp_bloodvalley/blood-valley_dn.tga",
    "../Textures/mp_bloodvalley/blood-valley_rt.tga",
    "../Textures/mp_bloodvalley/blood-valley_lf.tga"
};


// =====================
// TERRENO
// =====================

Terrain terrain(-1, -1, 200, 25, "../AssetsPfinal/TexturesP/MapaAlturas.png");

GLuint textureTerrenoBaseID = 0;
GLuint textureRID = 0;
GLuint textureGID = 0;
GLuint textureBID = 0;
GLuint textureBlendMapID = 0;


// =====================
// CONTROL GENERAL DE APLICACIÓN
// =====================

bool exitApp = false;

int lastMousePosX = 0;
int lastMousePosY = 0;
int offsetX = 0;
int offsetY = 0;

double deltaTime = 0.0;
double currTime = 0.0;
double lastTime = 0.0;

const float ESCALA_PUEBLO = 0.08f;


// =====================
// MENÚ PRINCIPAL
// =====================

bool mostrarMenu = true;
int opcionMenu = 0;
bool teclaMenuPresionada = false;
bool mousePresionadoMenu = false;
bool mostrarControles = false;

GLuint textureMenuFondoID = 0;
GLuint textureBotonIniciarID = 0;
GLuint textureBotonIniciarHoverID = 0;
GLuint textureBotonControlesID = 0;
GLuint textureBotonControlesHoverID = 0;
GLuint textureBotonSalirID = 0;
GLuint textureBotonSalirHoverID = 0;
GLuint textureControlesID = 0;
GLuint textureBotonRegresarID = 0;
GLuint textureBotonRegresarHoverID = 0;

Box boxMenuFondo;
Box boxBotonIniciar;
Box boxBotonControles;
Box boxBotonSalir;
Box boxBotonRegresar;


// =====================
// INTRO / SECUENCIA INICIAL
// =====================

bool mostrarSecuencia = false;
int imagenSecuenciaActual = 0;
float tiempoImagenSecuencia = 0.0f;

GLuint textureSecuencia1ID = 0;
GLuint textureSecuencia2ID = 0;
GLuint textureSecuencia3ID = 0;

GLuint textureBotonSiguienteID = 0;
GLuint textureBotonSiguienteHoverID = 0;
GLuint textureBotonJugarID = 0;
GLuint textureBotonJugarHoverID = 0;

Box boxBotonSecuencia;

bool mousePresionadoSecuencia = false;
bool botonSecuenciaHover = false;


// =====================
// PAUSA
// =====================

bool juegoPausado = false;
bool teclaPausaPresionada = false;

GLuint texturePausaID = 0;
Box boxPausa;


// =====================
// GAME OVER / TIEMPO TERMINADO
// =====================

bool tiempoTerminado = false;
bool juegoGanado = false;
bool mousePresionadoGameOver = false;

GLuint textureTiempoTerminadoID = 0;
GLuint textureBotonSalirFinalID = 0;
GLuint textureBotonSalirFinalHoverID = 0;

Box boxTiempoTerminado;
Box boxBotonSalirFinal;

GLuint textureVictoriaID = 0;


// =====================
// CONTADOR DE TIEMPO
// =====================

FontTypeRendering::FontTypeRendering* textoPantalla = nullptr;

float tiempoInicial = 1200.0f;
float tiempoRestante = tiempoInicial;
bool contadorActivo = true;


// =====================
// HUD Y OBJETOS RECOLECTABLES
// =====================

GLuint textureIconoID = 0;
Box boxIcono;

int objetosRecolectados = 0;
const int objetosTotales = 3;


// =====================
// SISTEMA DE PUNTUACION / DETECCIONES
// =====================

int vecesDetectado = 0;
float tiempoDesdeUltimaDeteccion = 0.0f;
bool estabaSiendoDetectado = false;

// =====================
// SISTEMA DE PUNTUACION
// =====================

int puntajeFinal = 0;

const int puntosPorObjeto = 1000;
const int puntosPorSegundo = 10;
const int penalizacionPorDeteccion = 100;

void registrarDeteccionJugador(bool detectado, float deltaTime) {
	if (detectado) {
		if (!estabaSiendoDetectado) {
			// Primera vez que el enemigo detecta a Perry
			vecesDetectado++;
			tiempoDesdeUltimaDeteccion = 0.0f;
			estabaSiendoDetectado = true;

			std::cout << "Deteccion registrada. Total detecciones: "
				<< vecesDetectado << std::endl;
		}
		else {
			// Perry sigue siendo detectado
			tiempoDesdeUltimaDeteccion += deltaTime;

			if (tiempoDesdeUltimaDeteccion >= 10.0f) {
				vecesDetectado++;
				tiempoDesdeUltimaDeteccion = 0.0f;

				std::cout << "Deteccion continua registrada. Total detecciones: "
					<< vecesDetectado << std::endl;
			}
		}
	}
	else {
		// Si Perry deja de ser visto, se reinicia el conteo continuo
		estabaSiendoDetectado = false;
		tiempoDesdeUltimaDeteccion = 0.0f;
	}
}
int calcularPuntajeFinal(int tiempoRestante) {
	int puntosObjetos = objetosRecolectados * puntosPorObjeto;
	int puntosTiempo = tiempoRestante * puntosPorSegundo;
	int puntosPerdidos = vecesDetectado * penalizacionPorDeteccion;

	puntajeFinal = puntosObjetos + puntosTiempo - puntosPerdidos;

	if (puntajeFinal < 0) {
		puntajeFinal = 0;
	}

	return puntajeFinal;
}

// Modelos recolectables usados en misiones.
Model Recolectable1;
Model Recolectable2;
Model Recolectable3;
bool modeloRecolectable3Cargado = false;
bool modeloRecolectable1Cargado = false;
bool modeloRecolectable2Cargado = false;

// Mensaje global de interacción para mostrar instrucciones al jugador.
bool mostrarMensajeInteraccion = false;
std::string mensajeInteraccion = "";


// =====================
// PERSONAJE PRINCIPAL: PERRY
// =====================

Model PerryIdle;
Model PerryCaminar;

bool modeloPerryCargado = false;

EstadoPerry estadoPerry = EstadoPerry::IDLE;

glm::vec3 posicionPerry = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 posicionAnteriorPerry = glm::vec3(0.0f, 0.0f, 0.0f);

float rotacionPerryY = 0.0f;

bool perryEnSuelo = true;
float velocidadVerticalPerry = 0.0f;

const float gravedadPerry = -18.0f;
const float fuerzaSaltoPerry = 6.0f;
const float velocidadMovimientoPerry = 5.0f;

// Índices usados para controlar animaciones sin reasignarlas cada frame.
int animacionPerryIdle = 0;
int animacionPerryCaminar = 0;
int animacionActualPerry = -1;


// =====================
// SISTEMA DÍA / NOCHE
// =====================

GLuint skyboxDiaTextureID = 0;
GLuint skyboxNocheTextureID = 0;

float tiempoCicloDiaNoche = 0.0f;
float duracionCicloDiaNoche = 120.0f;
float factorDiaNoche = 0.0f;


// =====================
// AUDIO GENERAL / MENÚ
// =====================

Mix_Music* musicaMenu = nullptr;
bool musicaMenuReproduciendo = false;

Mix_Chunk* sonidoMisionAgenteP = nullptr;


// =====================
// AUDIO FERIA
// =====================

Mix_Music* musicaFeria = nullptr;

Mix_Chunk* ambienteFeria1 = nullptr;
Mix_Chunk* ambienteFeria2 = nullptr;
Mix_Chunk* ambienteFeria3 = nullptr;

bool audioFeriaReproduciendo = false;

int canalAmbienteFeria1 = -1;
int canalAmbienteFeria2 = -1;
int canalAmbienteFeria3 = -1;


// =====================
// AUDIO CASINO
// =====================

Mix_Music* musicaCasino = nullptr;
Mix_Chunk* ambienteCasino = nullptr;
Mix_Chunk* sonidoMisionCasino = nullptr;

bool audioCasinoReproduciendo = false;
int canalAmbienteCasino = -1;



// =====================
// COLISIONES BASE
// =====================

std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersOBB;
std::map<std::string, std::tuple<AbstractModel::SBB, glm::mat4, glm::mat4>> collidersSBB;


// =====================
// COLISIONES POR ESCENA / OBSTÁCULOS
// =====================

std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersObstaculosOBB;
std::map<std::string, std::tuple<AbstractModel::OBB, glm::mat4, glm::mat4>> collidersPisosOBB;

bool mostrarDebugColliders = false;