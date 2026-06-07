#pragma once

/*
 * Archivo: RenderUtils.h
 * Descripción:
 * Declara funciones auxiliares generales utilizadas durante el renderizado
 * del proyecto final.
 *
 * Este archivo concentra la carga de modelos, texturas, skybox, terreno,
 * configuración de luces, renderizado de escenas, control visual de Perry,
 * audio, sistema de día/noche y colisiones híbridas.
 *
 * Su objetivo es separar la lógica común de renderizado y recursos gráficos
 * para que las escenas principales como Pueblo, Casino y Bolos puedan usar
 * estas funciones sin duplicar código.
 */

#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Headers/Model.h"

// =====================
// FUNCIONES AUXILIARES GENERALES
// =====================

/*
 * Resuelve una ruta de archivo para asegurar que pueda ser localizada
 * correctamente durante la ejecución del programa.
 */
std::string resolverRutaArchivo(const std::string& ruta);

/*
 * Carga un modelo específico del pueblo desde una ruta indicada.
 */
void cargarModeloPueblo(Model& modelo, const std::string& ruta);


// =====================
// CARGA DE MODELOS PRINCIPALES
// =====================

/*
 * Carga los modelos correspondientes a la escena del pueblo.
 */
void cargarModelosPueblo();

/*
 * Carga el modelo principal de la escena del casino.
 */
void cargarModelosCasino();

/*
 * Carga el modelo principal de la escena de bolos.
 */
void cargarModelosBolos();

/*
 * Carga el modelo del personaje principal Perry.
 */
void cargarModeloPerry();

/*
 * Carga el modelo del enemigo principal.
 */
void cargarModeloEnemigo();
/*
 * Carga el modelo del enemigo del casino.
 */
void cargarModeloEnemigoCasino();


// =====================
// RECOLECTABLES Y OBJETOS DE MISIÓN
// =====================

/*
 * Carga el primer modelo recolectable usado en misiones o eventos.
 */
void cargarModeloRecolectable1();

/*
 * Carga el segundo modelo recolectable usado en misiones o eventos.
 */
void cargarModeloRecolectable2();

void cargarModeloRecolectable3();


// =====================
// SKYBOX Y TEXTURAS GENERALES
// =====================

/*
 * Carga las seis texturas necesarias para formar un cubemap.
 */
GLuint cargarCubemap(const std::string nombres[6]);

/*
 * Carga los recursos del skybox utilizado para el ciclo día/noche.
 */
void cargarSkyboxDiaNoche();

/*
 * Carga la textura base del césped o piso simple.
 */
void cargarTexturaCesped();

/*
 * Carga texturas RGB utilizadas por el terreno.
 */
void cargarTexturasTerrenoRGB();

/*
 * Carga una textura usada por elementos del menú o interfaz.
 */
GLuint cargarTexturaMenu(
	const char* ruta,
	float saturacion = 1.0f,
	float brillo = 1.0f
);

// =====================
// MATRICES AUXILIARES
// =====================

/*
 * Genera una matriz de transformación para modelos del pueblo usando
 * posición, escala y rotación en Y.
 */
glm::mat4 matrizPueblo(
	glm::vec3 posicionOriginal,
	glm::vec3 escalaOriginal,
	float rotacionYGrados = 0.0f
);

/*
 * Genera una matriz de transformación usando rotación en los tres ejes.
 */
glm::mat4 matrizPuebloXYZ(
	glm::vec3 posicionOriginal,
	glm::vec3 escalaOriginal,
	glm::vec3 rotacionGrados
);


// =====================
// CÁMARA Y MATRICES DE RENDER
// =====================

/*
 * Envía las matrices de proyección y vista a los shaders principales.
 */
void configurarMatricesCamara(
	const glm::mat4& projection,
	const glm::mat4& view
);


// =====================
// CONFIGURACIÓN DE LUCES
// =====================

/*
 * Configura una iluminación general básica para escenas simples.
 */
void configurarLucesBasicas();

/*
 * Configura la iluminación principal de la escena del pueblo/centro.
 */
void configurarLucesCentro();

/*
 * Configura la iluminación estática de la escena del casino.
 */
void configurarLucesCasino();

/*
 * Configura la iluminación de la escena de bolos.
 */
void configurarLucesBoliche();

/*
 * Configura la iluminación específica del terreno.
 */
void configurarLucesTerreno();


// =====================
// RENDER DE ENTORNO GENERAL
// =====================

/*
 * Renderiza el piso plano base del escenario.
 */
void renderPiso();

/*
 * Renderiza el terreno principal del mapa.
 */
void renderTerreno();

/*
 * Renderiza un skybox simple.
 */
void renderSkybox(const glm::mat4& view);

/*
 * Renderiza el skybox con comportamiento de día y noche.
 */
void renderSkyboxDiaNoche(const glm::mat4& view);


// =====================
// RENDER DE ESCENAS PRINCIPALES
// =====================

/*
 * Renderiza las construcciones y modelos principales del pueblo.
 */
void renderConstrucciones4();

/*
 * Renderiza el escenario del casino.
 */
void renderCasino();

/*
 * Renderiza el escenario de bolos.
 */
void renderBolos();


// =====================
// PERSONAJE PRINCIPAL: PERRY
// =====================

/*
 * Actualiza la posición, movimiento y estado de Perry.
 */
void actualizarPerry(GLFWwindow* window, double deltaTime);

/*
 * Renderiza a Perry según su estado actual.
 */
void renderPerry();

/*
 * Genera la matriz base de transformación del modelo de Perry.
 */
glm::mat4 matrizPerryBase();

/*
 * Renderiza la animación o pose de Perry en estado idle.
 */
void renderPerryIdle();

/*
 * Renderiza la animación o pose de Perry caminando.
 */
void renderPerryCaminar();

/*
 * Obtiene la vista de cámara asociada a Perry.
 */
glm::mat4 obtenerVistaCamaraPerry();


// =====================
// SISTEMA DÍA / NOCHE
// =====================

/*
 * Actualiza el ciclo de día y noche con base en el tiempo transcurrido.
 */
void actualizarDiaNoche(double dt);


// =====================
// AUDIO GENERAL / MENÚ
// =====================

/*
 * Inicializa el sistema de audio general del proyecto.
 */
void inicializarAudio();

/*
 * Carga la música del menú principal.
 */
void cargarMusicaMenu();

/*
 * Reproduce la música del menú principal.
 */
void reproducirMusicaMenu();

/*
 * Detiene la música del menú principal.
 */
void detenerMusicaMenu();

/*
 * Libera los recursos generales del sistema de audio.
 */
void destruirAudio();


// =====================
// AUDIO DE MISIÓN AGENTE P
// =====================

/*
 * Carga el sonido asociado a la misión de Agente P.
 */
void cargarSonidoMisionAgenteP();

/*
 * Reproduce el sonido asociado a la misión de Agente P.
 */
void reproducirSonidoMisionAgenteP();


// =====================
// AUDIO FERIA
// =====================

/*
 * Carga los audios ambientales o musicales de la feria.
 */
void cargarAudioFeria();

/*
 * Reproduce el audio de la feria.
 */
void reproducirAudioFeria();

/*
 * Detiene el audio de la feria.
 */
void detenerAudioFeria();

/*
 * Libera los recursos de audio usados por la feria.
 */
void destruirAudioFeria();


// =====================
// AUDIO CASINO
// =====================

/*
 * Carga la música, ambiente o sonidos usados por la escena del casino.
 */
void cargarAudioCasino();

/*
 * Reproduce el audio principal del casino.
 */
void reproducirAudioCasino();

/*
 * Detiene el audio principal del casino.
 */
void detenerAudioCasino();

/*
 * Libera los recursos de audio usados por el casino.
 */
void destruirAudioCasino();

/*
 * Reproduce el sonido asociado a la misión del casino.
 */
void reproducirSonidoMisionCasino();


// =====================
// AUDIO VICTORIA
// =====================

void cargarAudioVictoria();
void reproducirAudioVictoria();
void destruirAudioVictoria();

// =====================
// AUDIO BOLICHE
// =====================

void cargarAudioBoliche();
void reproducirAudioBoliche();
void detenerAudioBoliche();
void destruirAudioBoliche();

// =====================
// SONIDO ENTRADA BOLICHE
// =====================

void cargarSonidoEntradaBoliche();
void reproducirSonidoEntradaBoliche();
void destruirSonidoEntradaBoliche();


// =====================
// COLISIONES HÍBRIDAS
// =====================

/*
 * Limpia los mesh colliders registrados en la escena.
 */
void limpiarMeshColliders();

/*
 * Registra un collider tipo caja.
 */
void registrarBoxCollider(
	const std::string& nombre,
	const glm::vec3& centro,
	const glm::vec3& mitad,
	bool solido
);

/*
 * Registra un mesh collider a partir de un archivo de modelo.
 */
void registrarMeshColliderDesdeArchivo(
	const std::string& nombre,
	const std::string& rutaModelo,
	const glm::mat4& matrizMundo,
	bool solido,
	bool piso
);

/*
 * Actualiza los colliders usados en la zona del centro/pueblo.
 */
void actualizarCollidersCentro();

/*
 * Actualiza los colliders usados en la escena del casino.
 */
void actualizarCollidersCasino();
/*
* Actualiza los colliders usados en la escena de bolos.
*/
void actualizarCollidersBolos();

/*
 * Obtiene la altura del suelo debajo de Perry según su posición.
 */
float obtenerAlturaSueloPerry(const glm::vec3& posicion);

/*
 * Verifica si Perry colisiona con algún elemento sólido de la escena.
 */
bool hayColisionPerryConEscena();
//enemigo casino
bool hayObstaculoVisionEntrePuntos(
    const glm::vec3& origen,
    const glm::vec3& destino
);


// =====================
// DEBUG VISUAL DE COLLIDERS
// =====================

/*
 * Reconstruye la geometría usada para visualizar colliders en modo debug.
 */
void reconstruirCollidersDebug();

/*
 * Renderiza los colliders generales de la escena.
 */
void renderCollidersDebug(
	const glm::mat4& projection,
	const glm::mat4& view
);

/*
 * Renderiza el collider específico de Perry.
 */
void renderColliderPerryDebug(
	const glm::mat4& projection,
	const glm::mat4& view
);

/*
 * Libera los recursos usados para visualizar colliders.
 */
void destruirCollidersDebug();