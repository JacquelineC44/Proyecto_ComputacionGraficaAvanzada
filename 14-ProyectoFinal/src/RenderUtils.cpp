#include "RenderUtils.h"

/*
 * Archivo: RenderUtils.cpp
 * Descripción:
 * Implementa funciones auxiliares de carga de modelos, texturas, terreno,
 * luces, renderizado, audio, control de Perry y colisiones del proyecto.
 */

#include <filesystem>
#include <iostream>
#include <vector>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <limits>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <glm/gtx/norm.hpp>
#include <sstream>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include "Globals.h"
#include "Headers/Texture.h"
#include "Headers/AnimationUtils.h"



// ===================== FUNCIONES AUXILIARES =====================

std::string resolverRutaArchivo(const std::string &ruta) {
	namespace fs = std::filesystem;

	const fs::path original(ruta);

	const std::vector<fs::path> candidatos = {
		original,
		fs::path("..") / original,
		fs::path("../..") / original,
		fs::path("../../..") / original
	};

	for (const auto &candidato : candidatos) {
		if (fs::exists(candidato)) {
			return candidato.lexically_normal().generic_string();
		}
	}

	std::cerr << "No se encontró el archivo: " << ruta << std::endl;
	std::cerr << "Directorio de ejecución actual: " << fs::current_path().string() << std::endl;

	for (const auto &candidato : candidatos) {
		std::cerr << "  Intentado: " << candidato.string() << std::endl;
	}

	return ruta;
}

void cargarModeloPueblo(Model &modelo, const std::string &ruta) {
	modelo.loadModel(resolverRutaArchivo(ruta));
	modelo.setShader(&shaderMulLighting);
}

// ===================== CARGA DE MODELOS =====================

void cargarModelosPueblo() {
	cargarModeloPueblo(Iglesia,   "../AssetsPfinal/Casas/Iglesia/Iglesia.fbx");
	cargarModeloPueblo(Portales,  "../AssetsPfinal/Casas/PortalesGrandes/PortalesGrandes.fbx");
	cargarModeloPueblo(chaparrin, "../AssetsPfinal/Casas/PortalesChicos/PortalesChicos.fbx");
	cargarModeloPueblo(rojita,    "../AssetsPfinal/Casas/CasaRoja/CasaRoja.fbx");
	cargarModeloPueblo(morado,    "../AssetsPfinal/Casas/CasaMorada/CasaMorada.fbx");
	cargarModeloPueblo(rosa,      "../AssetsPfinal/Casas/CasaMorada/CasaMorada2.fbx");
	cargarModeloPueblo(aqua,      "../AssetsPfinal/Casas/CasaRoja/CasaRoja2.fbx");
	cargarModeloPueblo(Centro,    "../AssetsPfinal/Casas/ModeloCentro/ModeloCentro.fbx");
}

void cargarModelosCasino() {
	std::cout << "==============================" << std::endl;
	std::cout << "INICIANDO CARGA DE CASINO" << std::endl;

	std::string rutaCasino = "../AssetsPfinal/Casino/EscenarioCasino.fbx";
	std::string rutaResuelta = resolverRutaArchivo(rutaCasino);

	std::cout << "Ruta solicitada: " << rutaCasino << std::endl;
	std::cout << "Ruta resuelta: " << rutaResuelta << std::endl;

	if (!std::filesystem::exists(rutaResuelta)) {
		std::cerr << "ERROR: No existe el archivo del casino." << std::endl;
		std::cerr << "Revisa nombre, carpeta o extension del archivo." << std::endl;
		return;
	}

	std::cout << "Archivo encontrado correctamente." << std::endl;
	std::cout << "ANTES de cargar Casino.loadModel()" << std::endl;

	try {
		cargarModeloPueblo(Casino, rutaResuelta);
		std::cout << "DESPUES de cargar Casino.loadModel()" << std::endl;
	}
	catch (const std::exception &e) {
		std::cerr << "EXCEPCION cargando casino: " << e.what() << std::endl;
		return;
	}
	catch (...) {
		std::cerr << "ERROR DESCONOCIDO cargando casino." << std::endl;
		return;
	}

	std::cout << "CASINO CARGADO SIN TRONAR" << std::endl;
	std::cout << "==============================" << std::endl;
}

void cargarModelosBolos() {
	std::cout << "==============================" << std::endl;
	std::cout << "INICIANDO CARGA DE BOLOS" << std::endl;

	std::string rutaBolos = "../AssetsPfinal/Boliche/boliche.fbx";
	std::string rutaResuelta = resolverRutaArchivo(rutaBolos);

	std::cout << "Ruta solicitada: " << rutaBolos << std::endl;
	std::cout << "Ruta resuelta: " << rutaResuelta << std::endl;

	if (!std::filesystem::exists(rutaResuelta)) {
		std::cerr << "ERROR: No existe el archivo de bolos." << std::endl;
		std::cerr << "Revisa nombre, carpeta o extension del archivo." << std::endl;
		return;
	}

	std::cout << "Archivo encontrado correctamente." << std::endl;
	std::cout << "ANTES de cargar Boliche.loadModel()" << std::endl;

	try {
		cargarModeloPueblo(Boliche, rutaResuelta);
		std::cout << "DESPUES de cargar Boliche.loadModel()" << std::endl;
	}
	catch (const std::exception &e) {
		std::cerr << "EXCEPCION cargando bolos: " << e.what() << std::endl;
		return;
	}
	catch (...) {
		std::cerr << "ERROR DESCONOCIDO cargando bolos." << std::endl;
		return;
	}

	std::cout << "BOLOS CARGADO SIN TRONAR" << std::endl;
	std::cout << "==============================" << std::endl;
}

// ===================== MATRICES AUXILIARES =====================

glm::mat4 matrizPuebloXYZ(glm::vec3 posicionOriginal,
                          glm::vec3 escalaOriginal,
                          glm::vec3 rotacionGrados) {
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, posicionOriginal * ESCALA_PUEBLO);
	model = glm::scale(model, escalaOriginal * ESCALA_PUEBLO);

	if (rotacionGrados.x != 0.0f) {
		model = glm::rotate(
			model,
			glm::radians(rotacionGrados.x),
			glm::vec3(1.0f, 0.0f, 0.0f)
		);
	}

	if (rotacionGrados.y != 0.0f) {
		model = glm::rotate(
			model,
			glm::radians(rotacionGrados.y),
			glm::vec3(0.0f, 1.0f, 0.0f)
		);
	}

	if (rotacionGrados.z != 0.0f) {
		model = glm::rotate(
			model,
			glm::radians(rotacionGrados.z),
			glm::vec3(0.0f, 0.0f, 1.0f)
		);
	}

	return model;
}

glm::mat4 matrizPueblo(glm::vec3 posicionOriginal,
                       glm::vec3 escalaOriginal,
                       float rotacionYGrados) {
	return matrizPuebloXYZ(
		posicionOriginal,
		escalaOriginal,
		glm::vec3(0.0f, rotacionYGrados, 0.0f)
	);
}

// ===================== CARGA DE SKYBOX Y TEXTURAS =====================

GLuint cargarCubemap(const std::string nombres[6]) {
	Texture skyboxTexture = Texture("");

	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum tipos[6] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};

	for (int i = 0; i < 6; i++) {
		skyboxTexture = Texture(nombres[i]);
		skyboxTexture.loadImage(true);

		if (skyboxTexture.getData()) {
			GLenum formato = skyboxTexture.getChannels() == 3 ? GL_RGB : GL_RGBA;

			glTexImage2D(
				tipos[i],
				0,
				formato,
				skyboxTexture.getWidth(),
				skyboxTexture.getHeight(),
				0,
				formato,
				GL_UNSIGNED_BYTE,
				skyboxTexture.getData()
			);
		}
		else {
			std::cout << "No se pudo cargar skybox: " << nombres[i] << std::endl;
		}

		skyboxTexture.freeImage();
	}

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	return textureID;
}

void cargarSkyboxDiaNoche() {
	std::string skyboxDia[6] = {
		"../Textures/skybox_dia/px.png",
		"../Textures/skybox_dia/nx.png",
		"../Textures/skybox_dia/py.png",
		"../Textures/skybox_dia/ny.png",
		"../Textures/skybox_dia/pz.png",
		"../Textures/skybox_dia/nz.png"
	};

	std::string skyboxNoche[6] = {
		"../Textures/skybox_noche/px.png",
		"../Textures/skybox_noche/nx.png",
		"../Textures/skybox_noche/py.png",
		"../Textures/skybox_noche/ny.png",
		"../Textures/skybox_noche/pz.png",
		"../Textures/skybox_noche/nz.png"
	};

	std::cout << "Cargando skybox de dia..." << std::endl;
	skyboxDiaTextureID = cargarCubemap(skyboxDia);

	std::cout << "Cargando skybox de noche..." << std::endl;
	skyboxNocheTextureID = cargarCubemap(skyboxNoche);
}

void cargarTexturaCesped() {
	Texture textureCesped("../Textures/grassy2.png");
	textureCesped.loadImage();

	glGenTextures(1, &textureCespedID);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textureCesped.getData()) {
		std::cout << "Numero de canales piso :=> " << textureCesped.getChannels() << std::endl;

		GLenum formato = textureCesped.getChannels() == 3 ? GL_RGB : GL_RGBA;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			formato,
			textureCesped.getWidth(),
			textureCesped.getHeight(),
			0,
			formato,
			GL_UNSIGNED_BYTE,
			textureCesped.getData()
		);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture: ../Textures/grassy2.png" << std::endl;
	}

	textureCesped.freeImage();
	glBindTexture(GL_TEXTURE_2D, 0);
}


// ===================== TEXTURAS DEL TERRENO =====================

GLuint cargarTextura2D(const char* ruta) {
	Texture textura(ruta);
	textura.loadImage();

	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textura.getData()) {
		std::cout << "Textura cargada: " << ruta
			<< " canales: " << textura.getChannels() << std::endl;

		GLenum formato = textura.getChannels() == 3 ? GL_RGB : GL_RGBA;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			formato,
			textura.getWidth(),
			textura.getHeight(),
			0,
			formato,
			GL_UNSIGNED_BYTE,
			textura.getData()
		);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "No se pudo cargar textura: " << ruta << std::endl;
	}

	textura.freeImage();
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}


void cargarTexturasTerrenoRGB() {
	textureTerrenoBaseID = cargarTextura2D("../AssetsPfinal/TexturesP/cesped.png");

	textureRID = cargarTextura2D("../AssetsPfinal/TexturesP/paja.png");
	textureGID = cargarTextura2D("../AssetsPfinal/TexturesP/paja.png");
	textureBID = cargarTextura2D("../AssetsPfinal/TexturesP/tierra.png");

	textureBlendMapID = cargarTextura2D("../AssetsPfinal/TexturesP/blendMap.png");
}

// ===================== INTERFAZ / MENU =====================

GLuint cargarTexturaMenu(
	const char* ruta,
	float saturacion,
	float brillo
) {
	Texture textura(ruta);
	textura.loadImage();

	GLuint textureID = 0;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (textura.getData()) {
		int canales = textura.getChannels();
		unsigned char* data = textura.getData();

		int width = textura.getWidth();
		int height = textura.getHeight();

		// Ajuste de saturación y brillo directamente sobre la imagen cargada.
		if (canales >= 3) {
			for (int i = 0; i < width * height; i++) {
				int index = i * canales;

				float r = data[index + 0] / 255.0f;
				float g = data[index + 1] / 255.0f;
				float b = data[index + 2] / 255.0f;

				// Luminancia aproximada del pixel.
				float gris = r * 0.299f + g * 0.587f + b * 0.114f;

				r = gris + (r - gris) * saturacion;
				g = gris + (g - gris) * saturacion;
				b = gris + (b - gris) * saturacion;

				r *= brillo;
				g *= brillo;
				b *= brillo;

				data[index + 0] = static_cast<unsigned char>(glm::clamp(r, 0.0f, 1.0f) * 255.0f);
				data[index + 1] = static_cast<unsigned char>(glm::clamp(g, 0.0f, 1.0f) * 255.0f);
				data[index + 2] = static_cast<unsigned char>(glm::clamp(b, 0.0f, 1.0f) * 255.0f);
			}
		}

		GLenum formato = canales == 3 ? GL_RGB : GL_RGBA;

		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			formato,
			width,
			height,
			0,
			formato,
			GL_UNSIGNED_BYTE,
			data
		);

		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "No se pudo cargar textura de menu: " << ruta << std::endl;
	}

	textura.freeImage();
	glBindTexture(GL_TEXTURE_2D, 0);

	return textureID;
}

// ===================== CONFIGURACIÓN DE CÁMARA Y LUCES =====================

void configurarMatricesCamara(const glm::mat4 &projection, const glm::mat4 &view) {
	shader.setMatrix4("projection", 1, false, glm::value_ptr(projection));
	shader.setMatrix4("view", 1, false, glm::value_ptr(view));

	shaderSkybox.setMatrix4("projection", 1, false, glm::value_ptr(projection));
	shaderSkybox.setMatrix4("view", 1, false, glm::value_ptr(glm::mat4(glm::mat3(view))));

	shaderMulLighting.setMatrix4("projection", 1, false, glm::value_ptr(projection));
	shaderMulLighting.setMatrix4("view", 1, false, glm::value_ptr(view));

	shaderTerreno.setMatrix4("projection", 1, false, glm::value_ptr(projection));
	shaderTerreno.setMatrix4("view", 1, false, glm::value_ptr(view));

	shaderFresnel.setMatrix4("projection", 1, false, glm::value_ptr(projection));
	shaderFresnel.setMatrix4("view", 1, false, glm::value_ptr(view));
}

void configurarLucesBasicas() {
	shaderMulLighting.turnOn();

	shaderMulLighting.setVectorFloat3(
		"viewPos",
		glm::value_ptr(camera->getPosition())
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.ambient",
		glm::value_ptr(glm::vec3(0.35f, 0.35f, 0.35f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.diffuse",
		glm::value_ptr(glm::vec3(0.75f, 0.75f, 0.75f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.specular",
		glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.9f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.direction",
		glm::value_ptr(glm::vec3(-1.0f, -0.3f, -0.2f))
	);

	shaderMulLighting.setInt("spotLightCount", 0);
	shaderMulLighting.setInt("pointLightCount", 0);

	shaderTerreno.turnOn();

	shaderTerreno.setVectorFloat3(
		"viewPos",
		glm::value_ptr(camera->getPosition())
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.light.ambient",
		glm::value_ptr(glm::vec3(0.35f, 0.35f, 0.35f))
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.light.diffuse",
		glm::value_ptr(glm::vec3(0.75f, 0.75f, 0.75f))
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.light.specular",
		glm::value_ptr(glm::vec3(0.9f, 0.9f, 0.9f))
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.direction",
		glm::value_ptr(glm::vec3(-1.0f, -0.3f, -0.2f))
	);

	shaderTerreno.setInt("spotLightCount", 0);
	shaderTerreno.setInt("pointLightCount", 0);
}

// ===================== RENDER COMÚN =====================


// ===================== RENDER DEL TERRENO =====================

void configurarLucesTerreno() {
	shaderTerreno.turnOn();

	shaderTerreno.setVectorFloat3(
		"viewPos",
		glm::value_ptr(camera->getPosition())
	);

	// factorDiaNoche: 0.0 = día, 1.0 = noche.
	float noche = factorDiaNoche;
	float dia = 1.0f - factorDiaNoche;

	glm::vec3 ambienteDia = glm::vec3(0.38f, 0.36f, 0.30f);
	glm::vec3 ambienteNoche = glm::vec3(0.015f, 0.018f, 0.035f);

	glm::vec3 difusaDia = glm::vec3(0.70f, 0.65f, 0.52f);
	glm::vec3 difusaNoche = glm::vec3(0.025f, 0.030f, 0.055f);

	glm::vec3 specularDia = glm::vec3(0.12f, 0.12f, 0.10f);
	glm::vec3 specularNoche = glm::vec3(0.005f, 0.006f, 0.012f);

	glm::vec3 ambienteFinal = ambienteDia * dia + ambienteNoche * noche;
	glm::vec3 difusaFinal = difusaDia * dia + difusaNoche * noche;
	glm::vec3 specularFinal = specularDia * dia + specularNoche * noche;

	shaderTerreno.setVectorFloat3(
		"directionalLight.light.ambient",
		glm::value_ptr(ambienteFinal)
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.light.diffuse",
		glm::value_ptr(difusaFinal)
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.light.specular",
		glm::value_ptr(specularFinal)
	);

	shaderTerreno.setVectorFloat3(
		"directionalLight.direction",
		glm::value_ptr(glm::vec3(-0.4f, -1.0f, -0.3f))
	);

	shaderTerreno.setInt("pointLightCount", 0);
	shaderTerreno.setInt("spotLightCount", 0);
}

void renderTerreno() {
	shaderTerreno.turnOn();
	configurarLucesTerreno();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureTerrenoBaseID);
	shaderTerreno.setInt("backgroundTexture", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureRID);
	shaderTerreno.setInt("rTexture", 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureGID);
	shaderTerreno.setInt("gTexture", 2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureBID);
	shaderTerreno.setInt("bTexture", 3);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textureBlendMapID);
	shaderTerreno.setInt("blendMapTexture", 4);

	terrain.setPosition(glm::vec3(100.0f, 0.0f, 100.0f));

	shaderTerreno.setVectorFloat2(
		"scaleUV",
		glm::value_ptr(glm::vec2(40.0f, 40.0f))
	);

	terrain.render();

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	shaderMulLighting.turnOn();
}

void renderPiso() {
	shaderMulLighting.turnOn();

	glm::mat4 modelCesped = glm::mat4(1.0f);

	modelCesped = glm::translate(modelCesped, glm::vec3(0.0f, -0.02f, 0.0f));
	modelCesped = glm::scale(modelCesped, glm::vec3(80.0f, 0.001f, 80.0f));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureCespedID);

	shaderMulLighting.setInt("texture1", 0);
	shaderMulLighting.setVectorFloat2(
		"scaleUV",
		glm::value_ptr(glm::vec2(80.0f, 80.0f))
	);

	boxCesped.render(modelCesped);

	shaderMulLighting.setVectorFloat2(
		"scaleUV",
		glm::value_ptr(glm::vec2(0.0f, 0.0f))
	);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void renderSkybox(const glm::mat4 &view) {
	shaderSkybox.turnOn();

	shaderSkybox.setMatrix4(
		"view",
		1,
		false,
		glm::value_ptr(glm::mat4(glm::mat3(view)))
	);

	GLint oldCullFaceMode;
	GLint oldDepthFuncMode;

	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	shaderSkybox.setFloat("skybox", 0);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureID);

	skyboxSphere.render();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}

// ===================== RENDER DEL PUEBLO =====================

void renderConstrucciones4() {
	shaderMulLighting.turnOn();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glm::mat4 modelIglesia;
	glm::mat4 modelPortalesGrandesD;
	glm::mat4 modelPortalesGrandesI;
	glm::mat4 modelPortalesChicos;
	glm::mat4 modelRojita;
	glm::mat4 modelMorado;
	glm::mat4 modelRosa;
	glm::mat4 modelAqua;
	glm::mat4 modelCentro;

	// Iglesia
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelIglesia = matrizPueblo(
		glm::vec3(0.0f, 38.0f, -245.0f),
		glm::vec3(0.125f, 0.125f, 0.125f),
		0.0f
	);

	Iglesia.render(modelIglesia);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Portales izquierdo
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelPortalesGrandesI = matrizPuebloXYZ(
		glm::vec3(160.0f, 53.0f, -25.0F),
		glm::vec3(0.000030f, 0.000030f, 0.000030f),
		glm::vec3(180.0f, 90.0f, 0.0f)
	);

	Portales.render(modelPortalesGrandesI);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Portales derecho
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelPortalesGrandesD = matrizPuebloXYZ(
		glm::vec3(-160.0f, 53.0f, -25.0f),
		glm::vec3(0.000030f, 0.000030f, 0.000030f),
		glm::vec3(180.0f, -90.0f, 0.0f)
	);

	Portales.render(modelPortalesGrandesD);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Portales chicos
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelPortalesChicos = matrizPuebloXYZ(
		glm::vec3(0.0f, -10.0f, 245.0f),
		glm::vec3(0.00125f, 0.00125f, 0.00125f),
		glm::vec3(90.0f, 180.0f, 0.0f)
	);

	chaparrin.render(modelPortalesChicos);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Casa roja
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelRojita = matrizPuebloXYZ(
		glm::vec3(200.0f, 46.0f, 220.0f),
		glm::vec3(0.12f, 0.12f, 0.12f),
		glm::vec3(-90.0f, 180.0f, 0.0f)
	);

	rojita.render(modelRojita);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Casa morada
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelMorado = matrizPuebloXYZ(
		glm::vec3(-200.0f, 29.0f, 220.0f),
		glm::vec3(0.13f, 0.13f, 0.13f),
		glm::vec3(-90.0f, 180.0f, 0.0f)
	);

	morado.render(modelMorado);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Casa rosa
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelRosa = matrizPuebloXYZ(
		glm::vec3(200.0f, 29.0f, -260.0f),
		glm::vec3(0.13f, 0.13f, 0.13f),
		glm::vec3(-90.0f, 180.0f, 180.0f)
	);

	rosa.render(modelRosa);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Casa lateral / aqua
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelAqua = matrizPuebloXYZ(
		glm::vec3(-200.0f, 49.0f, -260.0f),
		glm::vec3(0.0013f, 0.0013f, 0.0013f),
		glm::vec3(0.0f, 180.0f, 180.0f)
	);

	aqua.render(modelAqua);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);

	// Centro
	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelCentro = matrizPuebloXYZ(
		glm::vec3(0.0f, 10.0f, 0.0f),
		glm::vec3(0.3f, 0.3f, 0.3f),
		glm::vec3(0.0f, 0.0f, 0.0f)
	);

	Centro.render(modelCentro);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);
}

// ===================== RENDER DEL CASINO =====================

void renderCasino() {
	shaderMulLighting.turnOn();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glm::mat4 modelCasino;

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelCasino = matrizPuebloXYZ(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(15.0f, 10.0f, 15.0f),
		glm::vec3(-90.0f, 0.0f, 0.0f)
	);

	Casino.render(modelCasino);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);
}

// render de los bolos 
void renderBolos() {
	shaderMulLighting.turnOn();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glm::mat4 modelBoliche;

	glDisable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	modelBoliche = matrizPuebloXYZ(
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(50.0f, 50.0f, 50.0f),
		glm::vec3(-90.0f, 0.0f, 0.0f)
	);

	Boliche.render(modelBoliche);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glActiveTexture(GL_TEXTURE0);
}


glm::mat4 obtenerVistaCamaraPerry() {
	float rad = glm::radians(rotacionPerryY);

	glm::vec3 frentePerry = glm::vec3(
		std::sin(rad),
		0.0f,
		std::cos(rad)
	);

	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	glm::vec3 posicionCamara;
	glm::vec3 objetivoCamara;

	if (camaraPrimeraPersona) {
		const float alturaOjosPerry = 1.5f;

		posicionCamara =
			posicionPerry +
			glm::vec3(0.0f, alturaOjosPerry, 0.0f);

		objetivoCamara =
			posicionCamara +
			frentePerry;
	}
	else {
		const float distanciaCamara = 5.0f;
		const float alturaCamara = 3.0f;
		const float alturaObjetivo = 1.2f;

		posicionCamara =
			posicionPerry -
			frentePerry * distanciaCamara +
			glm::vec3(0.0f, alturaCamara, 0.0f);

		objetivoCamara =
			posicionPerry +
			frentePerry * 2.0f +
			glm::vec3(0.0f, alturaObjetivo, 0.0f);
	}

	camera->setPosition(posicionCamara);

	return glm::lookAt(posicionCamara, objetivoCamara, up);
}

// =====================
// PERSONAJE PRINCIPAL: PERRY
// =====================

void cargarModeloEnemigo() {
	if (modeloEnemigoCargado) {
		return;
	}

	std::cout << "==============================" << std::endl;
	std::cout << "Cargando modelo Enemigo..." << std::endl;

	cargarModeloPueblo(
		Enemigo,
		"../AssetsPfinal/Personajes/EvilQueen/EvilQueenWalking.fbx"
	);

	Enemigo.setAnimationIndex(0);

	modeloEnemigoCargado = true;

	std::cout << "Modelo Enemigo cargado correctamente." << std::endl;
	std::cout << "==============================" << std::endl;
}
//enemigo casino
void cargarModeloEnemigoCasino() {
    if (modeloEnemigoCasinoCargado) {
        return;
    }

    std::cout << "==============================" << std::endl;
    std::cout << "Cargando modelo Enemigo Casino..." << std::endl;

    cargarModeloPueblo(
        EnemigoCasino,
        "../AssetsPfinal/Personajes/panic/panic.fbx"
    );

    EnemigoCasino.setAnimationIndex(0);

    modeloEnemigoCasinoCargado = true;

    std::cout << "Modelo Enemigo Casino cargado correctamente." << std::endl;
    std::cout << "==============================" << std::endl;
}

void cargarModeloPerry() {
	if (modeloPerryCargado) {
		return;
	}

	std::cout << "==============================" << std::endl;
	std::cout << "Cargando modelo Perry Idle..." << std::endl;

	cargarModeloPueblo(
		PerryIdle,
		"../AssetsPfinal/Personajes/Perry/Perry_Idle.fbx"
	);

	PerryIdle.setAnimationIndex(0);

	std::cout << "Perry Idle cargado." << std::endl;

	std::cout << "Cargando modelo Perry Walking..." << std::endl;

	cargarModeloPueblo(
		PerryCaminar,
		"../AssetsPfinal/Personajes/Perry/Perry_Walking02.fbx"
	);

	PerryCaminar.setAnimationIndex(0);

	std::cout << "Perry Walking cargado." << std::endl;

	modeloPerryCargado = true;

	std::cout << "Perry cargado correctamente." << std::endl;
	std::cout << "==============================" << std::endl;
}


void actualizarPerry(GLFWwindow* window, double dt) {
	if (!modeloPerryCargado) {
		return;
	}

	float dtf = static_cast<float>(dt);

	bool seEstaMoviendo = false;
	bool quiereSaltar = false;

	// =====================
	// PARÁMETROS DE CONTROL
	// =====================

	const float velocidadRotacionPerry = 120.0f; // grados por segundo

	float entradaAvance = 0.0f;   // W/S o stick vertical
	float entradaRotacion = 0.0f; // A/D o stick horizontal

	// =====================
	// TECLADO
	// =====================

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		entradaAvance += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		entradaAvance -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		entradaRotacion += 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		entradaRotacion -= 1.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		quiereSaltar = true;
	}

	// =====================
	// CONTROL XBOX
	// =====================

	GLFWgamepadstate gamepad;

	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad)) {

		float stickX = gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
		float stickY = gamepad.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];

		const float deadZone = 0.20f;

		if (std::abs(stickX) > deadZone) {
			entradaRotacion += -stickX;
		}

		if (std::abs(stickY) > deadZone) {
			entradaAvance += -stickY;
		}

		if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_A] == GLFW_PRESS) {
			quiereSaltar = true;
		}
	}

	// =====================
	// GUARDAR POSICIÓN ANTERIOR
	// =====================

	glm::vec3 posicionAntesDeMover = posicionPerry;

	// =====================
	// ROTACIÓN LENTA DE PERRY
	// =====================

	if (entradaRotacion != 0.0f) {
		rotacionPerryY += entradaRotacion * velocidadRotacionPerry * dtf;
	}

	if (rotacionPerryY >= 360.0f) {
		rotacionPerryY -= 360.0f;
	}
	else if (rotacionPerryY < 0.0f) {
		rotacionPerryY += 360.0f;
	}

	// =====================
	// AVANCE SEGÚN HACIA DONDE MIRA PERRY
	// =====================

	if (entradaAvance != 0.0f) {
		float rad = glm::radians(rotacionPerryY);

		glm::vec3 frentePerry = glm::vec3(
			std::sin(rad),
			0.0f,
			std::cos(rad)
		);

		posicionPerry += frentePerry * entradaAvance * velocidadMovimientoPerry * dtf;

		seEstaMoviendo = true;
	}

	// =====================
	// COLISIÓN HORIZONTAL
	// =====================
	// Si Perry chocó con algo, regresamos X/Z.
	// Dejamos Y aparte para no romper pisos ni salto.

	if (hayColisionPerryConEscena()) {
		posicionPerry.x = posicionAntesDeMover.x;
		posicionPerry.z = posicionAntesDeMover.z;

		std::cout << "Movimiento bloqueado por colision." << std::endl;
	}

	// =====================
	// ALTURA DEL PISO ACTUAL
	// =====================
	// Aquí calculamos el piso dependiendo de X/Z.
	// Esto permite que Perry suba o baje según la zona donde esté.

	float sueloActualPerry = obtenerAlturaSueloPerry(posicionPerry);

	// =====================
	// SALTO FÍSICO
	// =====================

	if (quiereSaltar && perryEnSuelo) {
		velocidadVerticalPerry = fuerzaSaltoPerry;
		perryEnSuelo = false;
	}

	if (!perryEnSuelo) {
		velocidadVerticalPerry += gravedadPerry * dtf;
		posicionPerry.y += velocidadVerticalPerry * dtf;

		// Si Perry cae hasta el suelo actual, se queda en ese piso.
		if (posicionPerry.y <= sueloActualPerry) {
			posicionPerry.y = sueloActualPerry;
			velocidadVerticalPerry = 0.0f;
			perryEnSuelo = true;
		}
	}
	else {
		float diferenciaAltura = sueloActualPerry - posicionPerry.y;

		// Subida pequeña: escalón o banqueta.
		if (diferenciaAltura > 0.0f && diferenciaAltura <= 0.75f) {
			posicionPerry.y = sueloActualPerry;
		}

		// Si el supuesto piso está demasiado arriba, lo ignoramos.
		else if (diferenciaAltura > 0.75f) {
			// No hacemos nada.
		}

		// Bajada normal: empieza a caer.
		else if (diferenciaAltura < -0.05f) {
			perryEnSuelo = false;
			velocidadVerticalPerry = 0.0f;
		}

		// Piso casi igual.
		else {
			posicionPerry.y = sueloActualPerry;
		}
	}

	// =====================
	// ESTADO VISUAL
	// =====================

	static EstadoPerry estadoAnterior = EstadoPerry::IDLE;

	if (seEstaMoviendo) {
		estadoPerry = EstadoPerry::CAMINANDO;
	}
	else {
		estadoPerry = EstadoPerry::IDLE;
	}

	if (estadoPerry != estadoAnterior) {
		if (estadoPerry == EstadoPerry::CAMINANDO) {
			std::cout << "Perry cambio a CAMINANDO" << std::endl;
		}
		else {
			std::cout << "Perry cambio a IDLE" << std::endl;
		}

		estadoAnterior = estadoPerry;
	}

	// =====================
	// CAMBIO ENTRE PRIMERA Y TERCERA PERSONA
	// Tecla V o botón LB del control
	// =====================

	bool teclaV = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;
	bool botonLBControl = false;


	if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1) &&
		glfwGetGamepadState(GLFW_JOYSTICK_1, &gamepad)) {

		if (gamepad.buttons[GLFW_GAMEPAD_BUTTON_LEFT_BUMPER] == GLFW_PRESS) {
			botonLBControl = true;
		}
	}

	bool botonCambiarCamara = teclaV || botonLBControl;

	if (botonCambiarCamara && !teclaCamaraPresionada) {
		camaraPrimeraPersona = !camaraPrimeraPersona;
		teclaCamaraPresionada = true;

		if (camaraPrimeraPersona) {
			std::cout << "Camara: primera persona" << std::endl;
		}
		else {
			std::cout << "Camara: tercera persona" << std::endl;
		}
	}

	if (!botonCambiarCamara) {
		teclaCamaraPresionada = false;
	}

	// Debug opcional de posición
	static double ultimoDebugPerry = 0.0;

	if (currTime - ultimoDebugPerry > 1.0) {
		std::cout << "Perry -> X: " << posicionPerry.x
				  << " Y: " << posicionPerry.y
				  << " Z: " << posicionPerry.z
				  << " Suelo: " << sueloActualPerry
				  << std::endl;

		ultimoDebugPerry = currTime;
	}

	// IMPORTANTE:
	// Aquí ya NO hacemos camera->setPosition().
	// La vista se calcula en obtenerVistaCamaraPerry().
}

// glm::mat4 matrizPerryBase() {
// 	glm::mat4 model = glm::mat4(1.0f);

// 	model = glm::translate(model, posicionPerry);

// 	model = glm::rotate(
// 		model,
// 		glm::radians(rotacionPerryY),
// 		glm::vec3(0.0f, 1.0f, 0.0f)
// 	);

// 	model = glm::scale(model, glm::vec3(0.025f));

// 	return model;
// }
glm::mat4 matrizPerryBase() {
	// =====================
	// NORMAL DEL TERRENO
	// =====================

	glm::vec3 ejeY = glm::normalize(
		terrain.getNormalTerrain(posicionPerry.x, posicionPerry.z)
	);

	// =====================
	// DIRECCION HACIA DONDE MIRA PERRY
	// =====================

	float anguloY = glm::radians(rotacionPerryY);

	glm::vec3 direccionFrente = glm::normalize(glm::vec3(
		sin(anguloY),
		0.0f,
		cos(anguloY)
	));

	// =====================
	// EJES ADAPTADOS A LA PENDIENTE
	// =====================

	glm::vec3 ejeX = glm::normalize(glm::cross(ejeY, direccionFrente));
	glm::vec3 ejeZ = glm::normalize(glm::cross(ejeX, ejeY));

	// =====================
	// MATRIZ DE ORIENTACION
	// =====================

	glm::mat4 model = glm::mat4(1.0f);

	model[0] = glm::vec4(ejeX, 0.0f);
	model[1] = glm::vec4(ejeY, 0.0f);
	model[2] = glm::vec4(ejeZ, 0.0f);
	model[3] = glm::vec4(posicionPerry, 1.0f);

	model = glm::scale(model, glm::vec3(0.025f));

	return model;
}

void renderPerryIdle() {
	glm::mat4 modelIdle = matrizPerryBase();

	shaderMulLighting.setMatrix4(
		"model",
		1,
		false,
		glm::value_ptr(modelIdle)
	);

	PerryIdle.render(modelIdle);
}

void renderPerryCaminar() {
	glm::mat4 modelCaminar = matrizPerryBase();

	// PRUEBA VISUAL TEMPORAL:
	// Esto sube tantito el modelo de walking.
	// Si al caminar Perry se sube, significa que SÍ estás renderizando PerryCaminar.
	// Después lo quitamos.
	modelCaminar = glm::translate(modelCaminar, glm::vec3(0.0f, 0.0f, 0.0f));

	shaderMulLighting.setMatrix4(
		"model",
		1,
		false,
		glm::value_ptr(modelCaminar)
	);

	PerryCaminar.render(modelCaminar);
}

void renderPerry() {
	if (!modeloPerryCargado) {
		return;
	}

	shaderMulLighting.turnOn();

	static EstadoPerry ultimoEstadoRenderizado = EstadoPerry::IDLE;

	if (estadoPerry != ultimoEstadoRenderizado) {
		if (estadoPerry == EstadoPerry::CAMINANDO) {
			std::cout << "Render cambiando a PerryCaminar" << std::endl;
		}
		else {
			std::cout << "Render cambiando a PerryIdle" << std::endl;
		}

		ultimoEstadoRenderizado = estadoPerry;
	}

	if (estadoPerry == EstadoPerry::CAMINANDO) {
		renderPerryCaminar();
	}
	else {
		renderPerryIdle();
	}
}


void actualizarDiaNoche(double dt) {
	tiempoCicloDiaNoche += static_cast<float>(dt);

	if (tiempoCicloDiaNoche > duracionCicloDiaNoche) {
		tiempoCicloDiaNoche = 0.0f;
	}

	float t = tiempoCicloDiaNoche / duracionCicloDiaNoche;

	// Onda suave:
	// 0 = día, 1 = noche, 0 = día
	factorDiaNoche = (sin(t * 2.0f * 3.14159265f - 1.57079632f) + 1.0f) * 0.5f;
}

void renderSkyboxDiaNoche(const glm::mat4 &view) {
	shaderSkybox.turnOn();

	shaderSkybox.setMatrix4(
		"view",
		1,
		false,
		glm::value_ptr(glm::mat4(glm::mat3(view)))
	);

	shaderSkybox.setFloat("factorDiaNoche", factorDiaNoche);

	GLint oldCullFaceMode;
	GLint oldDepthFuncMode;

	glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
	glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxDiaTextureID);
	shaderSkybox.setInt("skyboxDia", 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxNocheTextureID);
	shaderSkybox.setInt("skyboxNoche", 1);

	skyboxSphere.render();

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	glActiveTexture(GL_TEXTURE0);

	glCullFace(oldCullFaceMode);
	glDepthFunc(oldDepthFuncMode);
}

void configurarLucesCentro() {
	shaderMulLighting.turnOn();

	shaderMulLighting.setVectorFloat3(
		"viewPos",
		glm::value_ptr(camera->getPosition())
	);

	// =====================
	// CICLO DIA / NOCHE
	// =====================
	// factorDiaNoche:
	// 0.0 = día
	// 1.0 = noche

	float noche = factorDiaNoche;
	float dia = 1.0f - factorDiaNoche;

	// =====================
	// LUZ GENERAL DEL CENTRO
	// =====================
	// De día: visible y cálida.
	// De noche: muy oscura, azul tenue.

	glm::vec3 ambienteDia = glm::vec3(0.45f, 0.42f, 0.35f);
	glm::vec3 ambienteNoche = glm::vec3(0.010f, 0.015f, 0.030f);

	glm::vec3 difusaDia = glm::vec3(0.90f, 0.82f, 0.65f);
	glm::vec3 difusaNoche = glm::vec3(0.020f, 0.030f, 0.055f);

	glm::vec3 specularDia = glm::vec3(0.45f, 0.42f, 0.35f);
	glm::vec3 specularNoche = glm::vec3(0.020f, 0.025f, 0.045f);

	glm::vec3 ambienteFinal =
		ambienteDia * dia +
		ambienteNoche * noche;

	glm::vec3 difusaFinal =
		difusaDia * dia +
		difusaNoche * noche;

	glm::vec3 specularFinal =
		specularDia * dia +
		specularNoche * noche;

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.ambient",
		glm::value_ptr(ambienteFinal)
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.diffuse",
		glm::value_ptr(difusaFinal)
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.specular",
		glm::value_ptr(specularFinal)
	);

	// Dirección tipo sol/luna
	shaderMulLighting.setVectorFloat3(
		"directionalLight.direction",
		glm::value_ptr(glm::vec3(-0.4f, -1.0f, -0.3f))
	);

	// =====================
	// FAROLES DEL CENTRO
	// =====================
	// De día prácticamente apagados.
	// De noche iluminan zonas puntuales.

	shaderMulLighting.setInt("pointLightCount", 4);

	glm::vec3 colorFarol = glm::vec3(1.0f, 0.58f, 0.22f);

	float intensidadFarol = noche * 0.75f;

	glm::vec3 ambienteFarol = colorFarol * 0.03f * intensidadFarol;
	glm::vec3 difusaFarol = colorFarol * 0.75f * intensidadFarol;
	glm::vec3 specularFarol = colorFarol * 0.35f * intensidadFarol;

	float alturaFarol = 3.2f;

	// Ajusta estos valores a las esquinas reales del centro
	float esquinaX = 7.0f;
	float esquinaZ = 7.0f;

	// =====================
	// FAROL 1: esquina frontal derecha
	// =====================

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].position",
		glm::value_ptr(glm::vec3(esquinaX, alturaFarol, esquinaZ))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.ambient",
		glm::value_ptr(ambienteFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.diffuse",
		glm::value_ptr(difusaFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.specular",
		glm::value_ptr(specularFarol)
	);

	shaderMulLighting.setFloat("pointLights[0].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[0].linear", 0.09f);
	shaderMulLighting.setFloat("pointLights[0].quadratic", 0.032f);

	// =====================
	// FAROL 2: esquina frontal izquierda
	// =====================

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].position",
		glm::value_ptr(glm::vec3(-esquinaX, alturaFarol, esquinaZ))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.ambient",
		glm::value_ptr(ambienteFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.diffuse",
		glm::value_ptr(difusaFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.specular",
		glm::value_ptr(specularFarol)
	);

	shaderMulLighting.setFloat("pointLights[1].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[1].linear", 0.09f);
	shaderMulLighting.setFloat("pointLights[1].quadratic", 0.032f);

	// =====================
	// FAROL 3: esquina trasera derecha
	// =====================

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].position",
		glm::value_ptr(glm::vec3(esquinaX, alturaFarol, -esquinaZ))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.ambient",
		glm::value_ptr(ambienteFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.diffuse",
		glm::value_ptr(difusaFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.specular",
		glm::value_ptr(specularFarol)
	);

	shaderMulLighting.setFloat("pointLights[2].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[2].linear", 0.09f);
	shaderMulLighting.setFloat("pointLights[2].quadratic", 0.032f);

	// =====================
	// FAROL 4: esquina trasera izquierda
	// =====================

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].position",
		glm::value_ptr(glm::vec3(-esquinaX, alturaFarol, -esquinaZ))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.ambient",
		glm::value_ptr(ambienteFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.diffuse",
		glm::value_ptr(difusaFarol)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.specular",
		glm::value_ptr(specularFarol)
	);

	shaderMulLighting.setFloat("pointLights[3].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[3].linear", 0.09f);
	shaderMulLighting.setFloat("pointLights[3].quadratic", 0.032f);

	// Sin spotlights
	shaderMulLighting.setInt("spotLightCount", 0);
}

void configurarLucesCasino() {
	shaderMulLighting.turnOn();

	shaderMulLighting.setVectorFloat3(
		"viewPos",
		glm::value_ptr(camera->getPosition())
	);

	// =====================
	// AMBIENTE GENERAL OSCURO
	// =====================
	// Luz base baja para que no se vea plano,
	// pero mantenga un mood medio dark.

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.ambient",
		glm::value_ptr(glm::vec3(0.10f, 0.08f, 0.10f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.diffuse",
		glm::value_ptr(glm::vec3(0.18f, 0.14f, 0.16f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.specular",
		glm::value_ptr(glm::vec3(0.25f, 0.20f, 0.22f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.direction",
		glm::value_ptr(glm::vec3(-0.2f, -1.0f, -0.3f))
	);

	// =====================
	// LUCES FIJAS DEL CASINO
	// =====================
	// Modelo cuadrado: ponemos una luz cálida al centro
	// y cuatro luces suaves cerca de las esquinas.

	shaderMulLighting.setInt("pointLightCount", 5);
	shaderMulLighting.setInt("spotLightCount", 0);

	glm::vec3 luzCentral = glm::vec3(1.0f, 0.55f, 0.25f);
	glm::vec3 luzRoja = glm::vec3(0.85f, 0.10f, 0.08f);
	glm::vec3 luzAzul = glm::vec3(0.10f, 0.25f, 0.90f);
	glm::vec3 luzMorada = glm::vec3(0.55f, 0.10f, 0.85f);
	glm::vec3 luzVerde = glm::vec3(0.10f, 0.65f, 0.35f);

	// Centro cálido
	shaderMulLighting.setVectorFloat3(
		"pointLights[0].position",
		glm::value_ptr(glm::vec3(0.0f, 4.5f, 0.0f))
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.ambient",
		glm::value_ptr(luzCentral * 0.05f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.diffuse",
		glm::value_ptr(luzCentral * 0.85f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.specular",
		glm::value_ptr(luzCentral * 0.45f)
	);
	shaderMulLighting.setFloat("pointLights[0].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[0].linear", 0.045f);
	shaderMulLighting.setFloat("pointLights[0].quadratic", 0.012f);

	// Esquina frontal derecha
	shaderMulLighting.setVectorFloat3(
		"pointLights[1].position",
		glm::value_ptr(glm::vec3(10.0f, 3.2f, 10.0f))
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.ambient",
		glm::value_ptr(luzRoja * 0.03f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.diffuse",
		glm::value_ptr(luzRoja * 0.55f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.specular",
		glm::value_ptr(luzRoja * 0.30f)
	);
	shaderMulLighting.setFloat("pointLights[1].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[1].linear", 0.07f);
	shaderMulLighting.setFloat("pointLights[1].quadratic", 0.017f);

	// Esquina frontal izquierda
	shaderMulLighting.setVectorFloat3(
		"pointLights[2].position",
		glm::value_ptr(glm::vec3(-10.0f, 3.2f, 10.0f))
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.ambient",
		glm::value_ptr(luzAzul * 0.03f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.diffuse",
		glm::value_ptr(luzAzul * 0.55f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.specular",
		glm::value_ptr(luzAzul * 0.30f)
	);
	shaderMulLighting.setFloat("pointLights[2].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[2].linear", 0.07f);
	shaderMulLighting.setFloat("pointLights[2].quadratic", 0.017f);

	// Esquina trasera derecha
	shaderMulLighting.setVectorFloat3(
		"pointLights[3].position",
		glm::value_ptr(glm::vec3(10.0f, 3.2f, -10.0f))
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.ambient",
		glm::value_ptr(luzMorada * 0.03f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.diffuse",
		glm::value_ptr(luzMorada * 0.55f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.specular",
		glm::value_ptr(luzMorada * 0.30f)
	);
	shaderMulLighting.setFloat("pointLights[3].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[3].linear", 0.07f);
	shaderMulLighting.setFloat("pointLights[3].quadratic", 0.017f);

	// Esquina trasera izquierda
	shaderMulLighting.setVectorFloat3(
		"pointLights[4].position",
		glm::value_ptr(glm::vec3(-10.0f, 3.2f, -10.0f))
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[4].light.ambient",
		glm::value_ptr(luzVerde * 0.03f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[4].light.diffuse",
		glm::value_ptr(luzVerde * 0.55f)
	);
	shaderMulLighting.setVectorFloat3(
		"pointLights[4].light.specular",
		glm::value_ptr(luzVerde * 0.30f)
	);
	shaderMulLighting.setFloat("pointLights[4].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[4].linear", 0.07f);
	shaderMulLighting.setFloat("pointLights[4].quadratic", 0.017f);
}


void configurarLucesBoliche() {
	shaderMulLighting.turnOn();

	shaderMulLighting.setVectorFloat3(
		"viewPos",
		glm::value_ptr(camera->getPosition())
	);

	// =====================
	// TIEMPO PARA LUCES DINÁMICAS
	// =====================
	float t = static_cast<float>(glfwGetTime());

	// Parpadeo suave tipo luces de boliche / pánico
	float pulsoRojo   = 0.65f + 0.35f * std::sin(t * 3.0f);
	float pulsoAzul   = 0.65f + 0.35f * std::sin(t * 2.4f + 1.5f);
	float pulsoMorado = 0.65f + 0.35f * std::sin(t * 2.8f + 3.0f);
	float pulsoVerde  = 0.65f + 0.35f * std::sin(t * 2.1f + 4.0f);

	// Movimiento lateral para que parezca iluminación viva
	float movimientoLuz = std::sin(t * 1.2f) * 2.5f;

	// =====================
	// LUZ AMBIENTE GENERAL
	// =====================
	// Muy oscura para que el boliche parezca de pánico.
	// No la pongas en 0 porque si no se pierden texturas.
	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.ambient",
		glm::value_ptr(glm::vec3(0.025f, 0.020f, 0.045f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.diffuse",
		glm::value_ptr(glm::vec3(0.055f, 0.045f, 0.080f))
	);

	shaderMulLighting.setVectorFloat3(
		"directionalLight.light.specular",
		glm::value_ptr(glm::vec3(0.18f, 0.14f, 0.25f))
	);

	// Luz desde arriba, como luz general del techo
	shaderMulLighting.setVectorFloat3(
		"directionalLight.direction",
		glm::value_ptr(glm::vec3(0.0f, -1.0f, -0.15f))
	);

	// =====================
	// LUCES DEL BOLICHE
	// =====================
	// Distribución pensada para:
	// - Mesas en lado largo izquierdo
	// - Mesas en lado largo derecho
	// - Mesas en lado corto trasero
	// - Tres líneas/pistas de boliche al frente
	//
	// Ajusta X/Z si el modelo quedó rotado.
	// X = izquierda/derecha
	// Z = frente/fondo

	shaderMulLighting.setInt("pointLightCount", 8);
	shaderMulLighting.setInt("spotLightCount", 0);

	glm::vec3 rojoPanic   = glm::vec3(1.0f, 0.05f, 0.03f);
	glm::vec3 azulNeon    = glm::vec3(0.05f, 0.30f, 1.0f);
	glm::vec3 moradoNeon  = glm::vec3(0.75f, 0.10f, 1.0f);
	glm::vec3 rosaNeon    = glm::vec3(1.0f, 0.05f, 0.55f);
	glm::vec3 verdeToxico = glm::vec3(0.05f, 1.0f, 0.35f);
	glm::vec3 cianNeon    = glm::vec3(0.0f, 0.90f, 1.0f);
	glm::vec3 amarillo    = glm::vec3(1.0f, 0.72f, 0.18f);

	// Altura aproximada de luces del techo
	float alturaTecho = 5.0f;

	// =====================================================
	// 0) LUZ CENTRAL MORADA - corazón del boliche
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[0].position",
		glm::value_ptr(glm::vec3(0.0f + movimientoLuz, alturaTecho, 0.0f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.ambient",
		glm::value_ptr(moradoNeon * 0.035f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.diffuse",
		glm::value_ptr(moradoNeon * 0.95f * pulsoMorado)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[0].light.specular",
		glm::value_ptr(moradoNeon * 0.80f * pulsoMorado)
	);

	shaderMulLighting.setFloat("pointLights[0].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[0].linear", 0.045f);
	shaderMulLighting.setFloat("pointLights[0].quadratic", 0.012f);

	// =====================================================
	// 1) MESAS LADO LARGO IZQUIERDO
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[1].position",
		glm::value_ptr(glm::vec3(-8.5f, 3.2f, 0.0f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.ambient",
		glm::value_ptr(azulNeon * 0.025f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.diffuse",
		glm::value_ptr(azulNeon * 0.75f * pulsoAzul)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[1].light.specular",
		glm::value_ptr(azulNeon * 0.60f * pulsoAzul)
	);

	shaderMulLighting.setFloat("pointLights[1].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[1].linear", 0.07f);
	shaderMulLighting.setFloat("pointLights[1].quadratic", 0.017f);

	// =====================================================
	// 2) MESAS LADO LARGO DERECHO
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[2].position",
		glm::value_ptr(glm::vec3(8.5f, 3.2f, 0.0f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.ambient",
		glm::value_ptr(rosaNeon * 0.025f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.diffuse",
		glm::value_ptr(rosaNeon * 0.75f * pulsoRojo)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[2].light.specular",
		glm::value_ptr(rosaNeon * 0.60f * pulsoRojo)
	);

	shaderMulLighting.setFloat("pointLights[2].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[2].linear", 0.07f);
	shaderMulLighting.setFloat("pointLights[2].quadratic", 0.017f);

	// =====================================================
	// 3) MESAS DEL LADO CORTO TRASERO
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[3].position",
		glm::value_ptr(glm::vec3(0.0f, 3.5f, -8.5f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.ambient",
		glm::value_ptr(verdeToxico * 0.020f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.diffuse",
		glm::value_ptr(verdeToxico * 0.65f * pulsoVerde)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[3].light.specular",
		glm::value_ptr(verdeToxico * 0.50f * pulsoVerde)
	);

	shaderMulLighting.setFloat("pointLights[3].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[3].linear", 0.08f);
	shaderMulLighting.setFloat("pointLights[3].quadratic", 0.020f);

	// =====================================================
	// 4) PISTA DE BOLICHE 1
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[4].position",
		glm::value_ptr(glm::vec3(-3.5f, 2.8f, 7.5f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[4].light.ambient",
		glm::value_ptr(cianNeon * 0.020f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[4].light.diffuse",
		glm::value_ptr(cianNeon * 0.80f * pulsoAzul)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[4].light.specular",
		glm::value_ptr(cianNeon * 0.60f * pulsoAzul)
	);

	shaderMulLighting.setFloat("pointLights[4].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[4].linear", 0.06f);
	shaderMulLighting.setFloat("pointLights[4].quadratic", 0.015f);

	// =====================================================
	// 5) PISTA DE BOLICHE 2
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[5].position",
		glm::value_ptr(glm::vec3(0.0f, 2.8f, 7.5f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[5].light.ambient",
		glm::value_ptr(amarillo * 0.020f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[5].light.diffuse",
		glm::value_ptr(amarillo * 0.70f * pulsoMorado)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[5].light.specular",
		glm::value_ptr(amarillo * 0.55f * pulsoMorado)
	);

	shaderMulLighting.setFloat("pointLights[5].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[5].linear", 0.06f);
	shaderMulLighting.setFloat("pointLights[5].quadratic", 0.015f);

	// =====================================================
	// 6) PISTA DE BOLICHE 3
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[6].position",
		glm::value_ptr(glm::vec3(3.5f, 2.8f, 7.5f))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[6].light.ambient",
		glm::value_ptr(rojoPanic * 0.020f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[6].light.diffuse",
		glm::value_ptr(rojoPanic * 0.85f * pulsoRojo)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[6].light.specular",
		glm::value_ptr(rojoPanic * 0.70f * pulsoRojo)
	);

	shaderMulLighting.setFloat("pointLights[6].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[6].linear", 0.06f);
	shaderMulLighting.setFloat("pointLights[6].quadratic", 0.015f);

	// =====================================================
	// 7) LUZ DE FONDO / PÁNICO ROJA
	// =====================================================
	shaderMulLighting.setVectorFloat3(
		"pointLights[7].position",
		glm::value_ptr(glm::vec3(0.0f, 4.2f, -3.5f + movimientoLuz))
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[7].light.ambient",
		glm::value_ptr(rojoPanic * 0.020f)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[7].light.diffuse",
		glm::value_ptr(rojoPanic * 0.95f * pulsoRojo)
	);

	shaderMulLighting.setVectorFloat3(
		"pointLights[7].light.specular",
		glm::value_ptr(rojoPanic * 0.80f * pulsoRojo)
	);

	shaderMulLighting.setFloat("pointLights[7].constant", 1.0f);
	shaderMulLighting.setFloat("pointLights[7].linear", 0.075f);
	shaderMulLighting.setFloat("pointLights[7].quadratic", 0.020f);
}

// =====================
// AUDIO BOLICHE
// =====================

Mix_Music* musicaBoliche = nullptr;
bool audioBolicheReproduciendo = false;

void cargarAudioBoliche() {
	std::cout << "Cargando audio del boliche..." << std::endl;

	musicaBoliche = Mix_LoadMUS("../AssetsPfinal/Music/Boliche/Boliche.mp3");

	if (musicaBoliche == nullptr) {
		std::cerr << "No se pudo cargar Boliche.mp3: "
			<< Mix_GetError() << std::endl;
	}
	else {
		std::cout << "Boliche.mp3 cargado correctamente." << std::endl;
	}

	audioBolicheReproduciendo = false;
}
// =====================
// AUDIO Boliche
// =====================
void reproducirAudioBoliche() {
	if (audioBolicheReproduciendo) {
		return;
	}

	std::cout << "Reproduciendo audio del boliche..." << std::endl;

	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	detenerAudioFeria();
	detenerAudioCasino();

	if (musicaBoliche != nullptr) {
		Mix_VolumeMusic(90);

		if (Mix_PlayMusic(musicaBoliche, -1) == -1) {
			std::cerr << "ERROR reproduciendo Boliche.mp3: "
				<< Mix_GetError() << std::endl;
		}
		else {
			audioBolicheReproduciendo = true;
			std::cout << "OK reproduciendo Boliche.mp3" << std::endl;
		}
	}
	else {
		std::cerr << "musicaBoliche es nullptr. No se puede reproducir." << std::endl;
	}
}

void detenerAudioBoliche() {
	if (audioBolicheReproduciendo) {
		Mix_HaltMusic();
		audioBolicheReproduciendo = false;
	}
}

void destruirAudioBoliche() {
	if (musicaBoliche != nullptr) {
		Mix_FreeMusic(musicaBoliche);
		musicaBoliche = nullptr;
	}

	audioBolicheReproduciendo = false;
}
// =====================
// AUDIO MENU CON SDL_MIXER
// =====================

void inicializarAudio() {
	SDL_SetMainReady();

	if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
		std::cerr << "Error inicializando SDL audio: " << SDL_GetError() << std::endl;
		return;
	}

	int flags = MIX_INIT_MP3 | MIX_INIT_OGG;
	int iniciado = Mix_Init(flags);

	if ((iniciado & flags) != flags) {
		std::cerr << "Advertencia Mix_Init: " << Mix_GetError() << std::endl;
	}

	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		std::cerr << "Error abriendo SDL_mixer: " << Mix_GetError() << std::endl;
		return;
	}

	Mix_VolumeMusic(45);

	std::cout << "Audio inicializado correctamente con SDL_mixer." << std::endl;
}

void cargarMusicaMenu() {
	std::cout << "Cargando musica del menu..." << std::endl;

	musicaMenu = Mix_LoadMUS("../AssetsPfinal/Music/Musics/The Last of Us Soundtrack 20 - All Gone (No Escape).mp3");

	if (musicaMenu == nullptr) {
		std::cerr << "No se pudo cargar musica del menu: " << Mix_GetError() << std::endl;
		return;
	}

	musicaMenuReproduciendo = false;

	std::cout << "Musica del menu cargada correctamente." << std::endl;
}

void reproducirMusicaMenu() {
	if (musicaMenu == nullptr) {
		return;
	}

	if (!Mix_PlayingMusic()) {
		if (Mix_PlayMusic(musicaMenu, -1) == -1) {
			std::cerr << "No se pudo reproducir musica del menu: " << Mix_GetError() << std::endl;
			return;
		}

		musicaMenuReproduciendo = true;
	}
}

void detenerMusicaMenu() {
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	musicaMenuReproduciendo = false;

	std::cout << "Musica del menu detenida." << std::endl;
}
// =====================
// AUDIO VICTORIA
// =====================

Mix_Music* musicaVictoria = nullptr;
bool audioVictoriaReproduciendo = false;

void cargarAudioVictoria() {
	std::cout << "Cargando audio de victoria..." << std::endl;

	musicaVictoria = Mix_LoadMUS("../AssetsPfinal/Music/Interfaz/Victoria.mp3");

	if (musicaVictoria == nullptr) {
		std::cerr << "No se pudo cargar Victoria.mp3: "
			<< Mix_GetError() << std::endl;
	}
	else {
		std::cout << "Victoria.mp3 cargada correctamente." << std::endl;
	}

	audioVictoriaReproduciendo = false;
}

void reproducirAudioVictoria() {
	if (audioVictoriaReproduciendo) {
		return;
	}

	std::cout << "Reproduciendo audio de victoria..." << std::endl;

	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	detenerAudioFeria();
	detenerAudioCasino();

	if (musicaVictoria != nullptr) {
		Mix_VolumeMusic(90);

		if (Mix_PlayMusic(musicaVictoria, 0) == -1) {
			std::cerr << "ERROR reproduciendo Victoria.mp3: "
				<< Mix_GetError() << std::endl;
		}
		else {
			audioVictoriaReproduciendo = true;
			std::cout << "OK reproduciendo Victoria.mp3" << std::endl;
		}
	}
	else {
		std::cerr << "musicaVictoria es nullptr. No se puede reproducir." << std::endl;
	}
}

void destruirAudioVictoria() {
	if (musicaVictoria != nullptr) {
		Mix_FreeMusic(musicaVictoria);
		musicaVictoria = nullptr;
	}

	audioVictoriaReproduciendo = false;
}

void destruirAudio() {
	if (musicaMenu != nullptr) {
		Mix_FreeMusic(musicaMenu);
		musicaMenu = nullptr;
	}
	if (sonidoMisionAgenteP != nullptr) {
	Mix_FreeChunk(sonidoMisionAgenteP);
	sonidoMisionAgenteP = nullptr;
	}

	Mix_CloseAudio();
	Mix_Quit();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	musicaMenuReproduciendo = false;

	std::cout << "Audio destruido correctamente." << std::endl;
}

void cargarSonidoMisionAgenteP() {
	std::cout << "Cargando sonido Mision Agente P..." << std::endl;

	sonidoMisionAgenteP = Mix_LoadWAV("../AssetsPfinal/Music/Effects/PrimeraMision.mp3");

	if (sonidoMisionAgenteP == nullptr) {
		std::cerr << "No se pudo cargar sonido Mision Agente P: "
				  << Mix_GetError() << std::endl;
		return;
	}

	// Volumen del efecto: 0 a 128
	Mix_VolumeChunk(sonidoMisionAgenteP, 90);

	std::cout << "Sonido Mision Agente P cargado correctamente." << std::endl;
}



void reproducirSonidoMisionAgenteP() {
	if (sonidoMisionAgenteP == nullptr) {
		return;
	}

	// Canal -1 = SDL elige un canal libre
	// Loops 0 = suena una sola vez
	if (Mix_PlayChannel(-1, sonidoMisionAgenteP, 0) == -1) {
		std::cerr << "No se pudo reproducir sonido Mision Agente P: "
				  << Mix_GetError() << std::endl;
	}
}

// =====================
// SONIDO ENTRADA BOLICHE
// =====================

Mix_Chunk* sonidoEntradaBoliche = nullptr;

void cargarSonidoEntradaBoliche() {
	std::cout << "Cargando sonido de entrada al boliche..." << std::endl;

	sonidoEntradaBoliche = Mix_LoadWAV("../AssetsPfinal/Music/Boliche/EntradaBoliche.mp3");

	if (sonidoEntradaBoliche == nullptr) {
		std::cerr << "No se pudo cargar EntradaBoliche.mp3: "
			<< Mix_GetError() << std::endl;
	}
	else {
		std::cout << "EntradaBoliche.mp3 cargado correctamente." << std::endl;
	}
}

void reproducirSonidoEntradaBoliche() {
	if (sonidoEntradaBoliche != nullptr) {
		Mix_VolumeChunk(sonidoEntradaBoliche, 100);

		if (Mix_PlayChannel(-1, sonidoEntradaBoliche, 0) == -1) {
			std::cerr << "ERROR reproduciendo EntradaBoliche.mp3: "
				<< Mix_GetError() << std::endl;
		}
	}
	else {
		std::cerr << "sonidoEntradaBoliche es nullptr. No se puede reproducir." << std::endl;
	}
}

void destruirSonidoEntradaBoliche() {
	if (sonidoEntradaBoliche != nullptr) {
		Mix_FreeChunk(sonidoEntradaBoliche);
		sonidoEntradaBoliche = nullptr;
	}
}

// =====================
// AUDIO FERIA
// =====================

void cargarAudioFeria() {
	std::cout << "Cargando audio de feria..." << std::endl;

	// Música principal de fondo
	musicaFeria = Mix_LoadMUS("../AssetsPfinal/Music/Feria/MusicaFeria.mp3");

	if (musicaFeria == nullptr) {
		std::cerr << "No se pudo cargar MusicaFeria.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		std::cout << "MusicaFeria.mp3 cargada correctamente." << std::endl;
	}

	// Ambiente 1
	ambienteFeria1 = Mix_LoadWAV("../AssetsPfinal/Music/Feria/AmbienteFeria1.mp3");

	if (ambienteFeria1 == nullptr) {
		std::cerr << "No se pudo cargar AmbienteFeria1.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		// Volumen bajo para que no tape la música
		Mix_VolumeChunk(ambienteFeria1, 30);
		std::cout << "AmbienteFeria1.mp3 cargado correctamente." << std::endl;
	}

	// Ambiente 2
	ambienteFeria2 = Mix_LoadWAV("../AssetsPfinal/Music/Feria/AmbienteFeria2.mp3");

	if (ambienteFeria2 == nullptr) {
		std::cerr << "No se pudo cargar AmbienteFeria2.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		// Un poco más bajo
		Mix_VolumeChunk(ambienteFeria2, 25);
		std::cout << "AmbienteFeria2.mp3 cargado correctamente." << std::endl;
	}

	// Ambiente 3
	ambienteFeria3 = Mix_LoadWAV("../AssetsPfinal/Music/Feria/AmbienteFeria3.mp3");

	if (ambienteFeria3 == nullptr) {
		std::cerr << "No se pudo cargar AmbienteFeria3.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		// Más sutil para que solo rellene
		Mix_VolumeChunk(ambienteFeria3, 20);
		std::cout << "AmbienteFeria3.mp3 cargado correctamente." << std::endl;
	}

	audioFeriaReproduciendo = false;
}

void reproducirAudioFeria() {
	if (audioFeriaReproduciendo) {
		return;
	}

	std::cout << "==============================" << std::endl;
	std::cout << "Reproduciendo audio de feria..." << std::endl;

	bool algoSono = false;

	// Detiene cualquier música previa.
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	// Música principal
	if (musicaFeria != nullptr) {
		Mix_VolumeMusic(100);

		if (Mix_PlayMusic(musicaFeria, -1) == -1) {
			std::cerr << "ERROR reproduciendo MusicaFeria.mp3: "
					  << Mix_GetError() << std::endl;
		}
		else {
			std::cout << "OK reproduciendo MusicaFeria.mp3" << std::endl;
			algoSono = true;
		}
	}
	else {
		std::cerr << "musicaFeria es nullptr. No se puede reproducir." << std::endl;
	}

	// Ambiente 1
	if (ambienteFeria1 != nullptr) {
		canalAmbienteFeria1 = Mix_PlayChannel(-1, ambienteFeria1, -1);

		if (canalAmbienteFeria1 == -1) {
			std::cerr << "ERROR reproduciendo AmbienteFeria1: "
					  << Mix_GetError() << std::endl;
		}
		else {
			Mix_Volume(canalAmbienteFeria1, 80);
			std::cout << "OK AmbienteFeria1 en canal " << canalAmbienteFeria1 << std::endl;
			algoSono = true;
		}
	}
	else {
		std::cerr << "ambienteFeria1 es nullptr." << std::endl;
	}

	// Ambiente 2
	if (ambienteFeria2 != nullptr) {
		canalAmbienteFeria2 = Mix_PlayChannel(-1, ambienteFeria2, -1);

		if (canalAmbienteFeria2 == -1) {
			std::cerr << "ERROR reproduciendo AmbienteFeria2: "
					  << Mix_GetError() << std::endl;
		}
		else {
			Mix_Volume(canalAmbienteFeria2, 70);
			std::cout << "OK AmbienteFeria2 en canal " << canalAmbienteFeria2 << std::endl;
			algoSono = true;
		}
	}
	else {
		std::cerr << "ambienteFeria2 es nullptr." << std::endl;
	}

	// Ambiente 3
	if (ambienteFeria3 != nullptr) {
		canalAmbienteFeria3 = Mix_PlayChannel(-1, ambienteFeria3, -1);

		if (canalAmbienteFeria3 == -1) {
			std::cerr << "ERROR reproduciendo AmbienteFeria3: "
					  << Mix_GetError() << std::endl;
		}
		else {
			Mix_Volume(canalAmbienteFeria3, 60);
			std::cout << "OK AmbienteFeria3 en canal " << canalAmbienteFeria3 << std::endl;
			algoSono = true;
		}
	}
	else {
		std::cerr << "ambienteFeria3 es nullptr." << std::endl;
	}

	audioFeriaReproduciendo = algoSono;

	if (algoSono) {
		std::cout << "Audio feria activo." << std::endl;
	}
	else {
		std::cout << "No se activo audio de feria." << std::endl;
	}

	std::cout << "==============================" << std::endl;
}

void detenerAudioFeria() {
	if (!audioFeriaReproduciendo) {
		return;
	}

	std::cout << "Deteniendo audio de feria..." << std::endl;

	// Detener música principal
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	// Detener ambientes
	if (canalAmbienteFeria1 != -1) {
		Mix_HaltChannel(canalAmbienteFeria1);
		canalAmbienteFeria1 = -1;
	}

	if (canalAmbienteFeria2 != -1) {
		Mix_HaltChannel(canalAmbienteFeria2);
		canalAmbienteFeria2 = -1;
	}

	if (canalAmbienteFeria3 != -1) {
		Mix_HaltChannel(canalAmbienteFeria3);
		canalAmbienteFeria3 = -1;
	}

	audioFeriaReproduciendo = false;
}

void destruirAudioFeria() {
	detenerAudioFeria();

	if (musicaFeria != nullptr) {
		Mix_FreeMusic(musicaFeria);
		musicaFeria = nullptr;
	}

	if (ambienteFeria1 != nullptr) {
		Mix_FreeChunk(ambienteFeria1);
		ambienteFeria1 = nullptr;
	}

	if (ambienteFeria2 != nullptr) {
		Mix_FreeChunk(ambienteFeria2);
		ambienteFeria2 = nullptr;
	}

	if (ambienteFeria3 != nullptr) {
		Mix_FreeChunk(ambienteFeria3);
		ambienteFeria3 = nullptr;
	}

	audioFeriaReproduciendo = false;
}

// =====================
// AUDIO CASINO
// =====================

void cargarAudioCasino() {
	std::cout << "Cargando audio de casino..." << std::endl;

	// Música principal del casino
	musicaCasino = Mix_LoadMUS("../AssetsPfinal/Music/Casino/MusicaCasino.mp3");

	if (musicaCasino == nullptr) {
		std::cerr << "No se pudo cargar MusicaCasino.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		std::cout << "MusicaCasino.mp3 cargada correctamente." << std::endl;
	}

	// Ambiente del casino
	ambienteCasino = Mix_LoadWAV("../AssetsPfinal/Music/Casino/AmbienteCasino.mp3");

	if (ambienteCasino == nullptr) {
		std::cerr << "No se pudo cargar AmbienteCasino.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		Mix_VolumeChunk(ambienteCasino, 90);
		std::cout << "AmbienteCasino.mp3 cargado correctamente." << std::endl;
	}

	// Sonido de misión del casino
	sonidoMisionCasino = Mix_LoadWAV("../AssetsPfinal/Music/Casino/MisionCasino.mp3");

	if (sonidoMisionCasino == nullptr) {
		std::cerr << "No se pudo cargar MisionCasino.mp3: "
				  << Mix_GetError() << std::endl;
	}
	else {
		Mix_VolumeChunk(sonidoMisionCasino, 100);
		std::cout << "MisionCasino.mp3 cargado correctamente." << std::endl;
	}

	audioCasinoReproduciendo = false;
}

void reproducirAudioCasino() {
	if (audioCasinoReproduciendo) {
		return;
	}

	std::cout << "==============================" << std::endl;
	std::cout << "Reproduciendo audio de casino..." << std::endl;

	bool algoSono = false;

	// Detiene cualquier música anterior
	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	// Música principal
	if (musicaCasino != nullptr) {
		Mix_VolumeMusic(19);

		if (Mix_PlayMusic(musicaCasino, -1) == -1) {
			std::cerr << "ERROR reproduciendo MusicaCasino.mp3: "
					  << Mix_GetError() << std::endl;
		}
		else {
			std::cout << "OK reproduciendo MusicaCasino.mp3" << std::endl;
			algoSono = true;
		}
	}
	else {
		std::cerr << "musicaCasino es nullptr. No se puede reproducir." << std::endl;
	}

	// Ambiente en loop
	if (ambienteCasino != nullptr) {
		canalAmbienteCasino = Mix_PlayChannel(-1, ambienteCasino, -1);

		if (canalAmbienteCasino == -1) {
			std::cerr << "ERROR reproduciendo AmbienteCasino.mp3: "
					  << Mix_GetError() << std::endl;
		}
		else {
			Mix_Volume(canalAmbienteCasino, 55);
			std::cout << "OK AmbienteCasino en canal "
					  << canalAmbienteCasino
					  << std::endl;
			algoSono = true;
		}
	}
	else {
		std::cerr << "ambienteCasino es nullptr." << std::endl;
	}

	audioCasinoReproduciendo = algoSono;

	if (algoSono) {
		std::cout << "Audio casino activo." << std::endl;
	}
	else {
		std::cout << "No se activo audio casino." << std::endl;
	}

	std::cout << "==============================" << std::endl;
}

void detenerAudioCasino() {
	if (!audioCasinoReproduciendo) {
		return;
	}

	std::cout << "Deteniendo audio de casino..." << std::endl;

	if (Mix_PlayingMusic()) {
		Mix_HaltMusic();
	}

	if (canalAmbienteCasino != -1) {
		Mix_HaltChannel(canalAmbienteCasino);
		canalAmbienteCasino = -1;
	}

	audioCasinoReproduciendo = false;
}

void destruirAudioCasino() {
	detenerAudioCasino();

	if (musicaCasino != nullptr) {
		Mix_FreeMusic(musicaCasino);
		musicaCasino = nullptr;
	}

	if (ambienteCasino != nullptr) {
		Mix_FreeChunk(ambienteCasino);
		ambienteCasino = nullptr;
	}

	if (sonidoMisionCasino != nullptr) {
		Mix_FreeChunk(sonidoMisionCasino);
		sonidoMisionCasino = nullptr;
	}

	audioCasinoReproduciendo = false;
}

void reproducirSonidoMisionCasino() {
	if (sonidoMisionCasino == nullptr) {
		std::cerr << "sonidoMisionCasino es nullptr." << std::endl;
		return;
	}

	if (Mix_PlayChannel(-1, sonidoMisionCasino, 0) == -1) {
		std::cerr << "No se pudo reproducir MisionCasino.mp3: "
				  << Mix_GetError()
				  << std::endl;
	}
}

// =====================================================
// COLISIONES HIBRIDAS
// BoxCollider para casas / iglesia / portales
// MeshCollider solo para ModeloCentro
// =====================================================

struct TrianguloCollider {
	glm::vec3 a;
	glm::vec3 b;
	glm::vec3 c;
	glm::vec3 normal;
};

struct MeshColliderEstatico {
	std::string nombre;
	std::vector<TrianguloCollider> triangulos;
	bool solido;
	bool piso;
};

struct BoxColliderEstatico {
	std::string nombre;
	glm::vec3 centro;
	glm::vec3 mitad;
	bool solido;
};

static std::vector<MeshColliderEstatico> meshCollidersEstaticos;
static std::vector<BoxColliderEstatico> boxCollidersEstaticos;

static GLuint debugColliderVAO = 0;
static GLuint debugColliderVBO = 0;
static GLuint debugColliderShader = 0;
static GLsizei debugColliderVertexCount = 0;

// =====================================================
// AUXILIARES
// =====================================================

static glm::mat4 aiMatrixToGlm(const aiMatrix4x4& m) {
	glm::mat4 r;

	r[0][0] = m.a1; r[1][0] = m.a2; r[2][0] = m.a3; r[3][0] = m.a4;
	r[0][1] = m.b1; r[1][1] = m.b2; r[2][1] = m.b3; r[3][1] = m.b4;
	r[0][2] = m.c1; r[1][2] = m.c2; r[2][2] = m.c3; r[3][2] = m.c4;
	r[0][3] = m.d1; r[1][3] = m.d2; r[2][3] = m.d3; r[3][3] = m.d4;

	return r;
}

static glm::vec3 calcularNormalTriangulo(
	const glm::vec3& a,
	const glm::vec3& b,
	const glm::vec3& c
) {
	glm::vec3 n = glm::cross(b - a, c - a);

	if (glm::length(n) < 0.000001f) {
		return glm::vec3(0.0f, 1.0f, 0.0f);
	}

	return glm::normalize(n);
}

void limpiarMeshColliders() {
	meshCollidersEstaticos.clear();
	boxCollidersEstaticos.clear();

	if (debugColliderVBO != 0) {
		glDeleteBuffers(1, &debugColliderVBO);
		debugColliderVBO = 0;
	}

	if (debugColliderVAO != 0) {
		glDeleteVertexArrays(1, &debugColliderVAO);
		debugColliderVAO = 0;
	}

	debugColliderVertexCount = 0;
}
// =====================================================
// BOX COLLIDERS
// =====================================================

void registrarBoxCollider(
	const std::string& nombre,
	const glm::vec3& centro,
	const glm::vec3& mitad,
	bool solido
) {
	BoxColliderEstatico collider;
	collider.nombre = nombre;
	collider.centro = centro;
	collider.mitad = mitad;
	collider.solido = solido;

	boxCollidersEstaticos.push_back(collider);
}

// =====================================================
// MESH COLLIDER SOLO PARA EL CENTRO
// =====================================================

static void extraerTriangulosNodoAssimp(
	const aiScene* scene,
	aiNode* node,
	const glm::mat4& transformPadre,
	const glm::mat4& transformModelo,
	MeshColliderEstatico& collider
) {
	glm::mat4 transformNodo = transformPadre * aiMatrixToGlm(node->mTransformation);

	for (unsigned int i = 0; i < node->mNumMeshes; i++) {
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		for (unsigned int f = 0; f < mesh->mNumFaces; f++) {
			const aiFace& face = mesh->mFaces[f];

			if (face.mNumIndices != 3) {
				continue;
			}

			const aiVector3D& v0 = mesh->mVertices[face.mIndices[0]];
			const aiVector3D& v1 = mesh->mVertices[face.mIndices[1]];
			const aiVector3D& v2 = mesh->mVertices[face.mIndices[2]];

			glm::vec3 p0 = glm::vec3(
				transformModelo * glm::vec4(v0.x, v0.y, v0.z, 1.0f)
			);

			glm::vec3 p1 = glm::vec3(
				transformModelo * glm::vec4(v1.x, v1.y, v1.z, 1.0f)
			);

			glm::vec3 p2 = glm::vec3(
				transformModelo * glm::vec4(v2.x, v2.y, v2.z, 1.0f)
			);

			TrianguloCollider tri;
			tri.a = p0;
			tri.b = p1;
			tri.c = p2;
			tri.normal = calcularNormalTriangulo(p0, p1, p2);

			collider.triangulos.push_back(tri);
		}
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++) {
		extraerTriangulosNodoAssimp(
			scene,
			node->mChildren[i],
			transformNodo,
			transformModelo,
			collider
		);
	}
}

void registrarMeshColliderDesdeArchivo(
	const std::string& nombre,
	const std::string& rutaModelo,
	const glm::mat4& matrizMundo,
	bool solido,
	bool piso
) {
	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(
		resolverRutaArchivo(rutaModelo),
		aiProcess_Triangulate |
		aiProcess_GenNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_PreTransformVertices |
		aiProcess_ImproveCacheLocality
	);

	if (!scene || !scene->mRootNode) {
		std::cerr << "No se pudo cargar MeshCollider: "
				  << nombre
				  << " | "
				  << importer.GetErrorString()
				  << std::endl;
		return;
	}

	MeshColliderEstatico collider;
	collider.nombre = nombre;
	collider.solido = solido;
	collider.piso = piso;

	extraerTriangulosNodoAssimp(
		scene,
		scene->mRootNode,
		glm::mat4(1.0f),
		matrizMundo,
		collider
	);

	if (!collider.triangulos.empty()) {
		meshCollidersEstaticos.push_back(collider);
	}
}

// =====================================================
// REGISTRO DE COLLIDERS DEL PUEBLO
// =====================================================

void actualizarCollidersCentro() {
	limpiarMeshColliders();

	// =================================================
	// BOX COLLIDERS PARA CONSTRUCCIONES GRANDES
	// =================================================
	// Centros alineados con las posiciones reales usadas en renderConstrucciones4().
	// Las posiciones originales se multiplican por ESCALA_PUEBLO igual que matrizPuebloXYZ().

	registrarBoxCollider(
		"Iglesia",
		glm::vec3(0.0f, 38.0f, -280.0f) * ESCALA_PUEBLO,
		glm::vec3(7.0f, 4.5f, 5.5f),
		true
	);

	registrarBoxCollider(
		"Portales Izquierdos",
		glm::vec3(180.0f, 53.0f, -20.0f) * ESCALA_PUEBLO,
		glm::vec3(5.0f, 4.0f, 8.5f),
		true
	);

	registrarBoxCollider(
		"Portales Derechos",
		glm::vec3(-180.0f, 53.0f, -20.0f) * ESCALA_PUEBLO,
		glm::vec3(5.0f, 4.0f, 8.5f),
		true
	);

	registrarBoxCollider(
		"Portales Chicos",
		glm::vec3(0.0f, 15.0f, 240.0f) * ESCALA_PUEBLO,
		glm::vec3(9.0f, 3.5f, 5.0f),
		true
	);

	registrarBoxCollider(
		"Casa Roja",
		glm::vec3(215.0f, 46.0f, 240.0f) * ESCALA_PUEBLO,
		glm::vec3(5.0f, 4.0f, 5.0f),
		true
	);

	registrarBoxCollider(
		"Casa Morada",
		glm::vec3(-215.0f, 29.0f, 240.0f) * ESCALA_PUEBLO,
		glm::vec3(5.0f, 4.0f, 5.0f),
		true
	);

	registrarBoxCollider(
		"Casa Rosa",
		glm::vec3(215.0f, 29.0f, -275.0f) * ESCALA_PUEBLO,
		glm::vec3(5.0f, 4.0f, 5.0f),
		true
	);

	registrarBoxCollider(
		"Casa Aqua",
		glm::vec3(-215.0f, 49.0f, -275.0f) * ESCALA_PUEBLO,
		glm::vec3(5.0f, 4.0f, 5.0f),
		true
	);

	// =================================================
	// MESH COLLIDER SOLO PARA MODELOCENTRO
	// =================================================

	glm::mat4 modelCentro = matrizPuebloXYZ(
		glm::vec3(-3.0f, 4.0f, -3.0f),
		glm::vec3(0.030f, 0.030f, 0.030f),
		glm::vec3(0.0f, 180.0f, 0.0f)
	);

	registrarMeshColliderDesdeArchivo(
		"Centro",
		"../AssetsPfinal/Casas/ModeloCentro/ModeloCentro.fbx",
		modelCentro,
		true,
		true
	);

	size_t totalTriangulos = 0;

	for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
		totalTriangulos += collider.triangulos.size();
	}

	std::cout << "Colliders pueblo cargados -> Box: "
			<< boxCollidersEstaticos.size()
			<< " | Mesh: "
			<< meshCollidersEstaticos.size()
			<< " | Triangulos mesh: "
			<< totalTriangulos
			<< std::endl;

	reconstruirCollidersDebug();
	

}

void actualizarCollidersCasino() {
	limpiarMeshColliders();

	glm::mat4 modelCasino = matrizPuebloXYZ(
		glm::vec3(0.0f, -0.0f, 0.0f),
		glm::vec3(0.15f,0.10f, 0.15f),
		glm::vec3(0.0f, 0.0f, 0.0f)
	);

	registrarMeshColliderDesdeArchivo(
		"Casino",
		"../AssetsPfinal/Casino/EscenarioCasino.fbx",
		modelCasino,
		true,
		true
	);

	size_t totalTriangulos = 0;

	for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
		totalTriangulos += collider.triangulos.size();
	}

	std::cout << "Colliders casino cargados -> Box: "
			  << boxCollidersEstaticos.size()
			  << " | Mesh: "
			  << meshCollidersEstaticos.size()
			  << " | Triangulos mesh: "
			  << totalTriangulos
			  << std::endl;

	reconstruirCollidersDebug();
}
void actualizarCollidersBolos() {
	limpiarMeshColliders();

	glm::mat4 modelBoliche = matrizPuebloXYZ(
		glm::vec3(-0.0f, -0.0f, -0.0f),
		glm::vec3(0.50f, 0.50f, 0.50f),
		glm::vec3(0.0f, 0.0f, 0.0f)
	);

	registrarMeshColliderDesdeArchivo(
		"Boliche",
		"../AssetsPfinal/Boliche/boliche.fbx",
		modelBoliche,
		true,
		true
	);

	size_t totalTriangulos = 0;

	for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
		totalTriangulos += collider.triangulos.size();
	}

	std::cout << "Colliders boliche cargados -> Box: "
			  << boxCollidersEstaticos.size()
			  << " | Mesh: "
			  << meshCollidersEstaticos.size()
			  << " | Triangulos mesh: "
			  << totalTriangulos
			  << std::endl;

	reconstruirCollidersDebug();
}

// =====================================================
// RAYCAST CONTRA TRIANGULO
// =====================================================

static bool raycastTriangulo(
	const glm::vec3& origen,
	const glm::vec3& direccion,
	const TrianguloCollider& tri,
	float& distancia
) {
	const float EPSILON = 0.0000001f;

	glm::vec3 edge1 = tri.b - tri.a;
	glm::vec3 edge2 = tri.c - tri.a;

	glm::vec3 h = glm::cross(direccion, edge2);
	float det = glm::dot(edge1, h);

	if (det > -EPSILON && det < EPSILON) {
		return false;
	}

	float invDet = 1.0f / det;

	glm::vec3 s = origen - tri.a;
	float u = invDet * glm::dot(s, h);

	if (u < 0.0f || u > 1.0f) {
		return false;
	}

	glm::vec3 q = glm::cross(s, edge1);
	float v = invDet * glm::dot(direccion, q);

	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}

	float t = invDet * glm::dot(edge2, q);

	if (t <= EPSILON) {
		return false;
	}

	distancia = t;
	return true;
}

//enemigo casino
bool hayObstaculoVisionEntrePuntos(
    const glm::vec3& origen,
    const glm::vec3& destino
) {
    glm::vec3 direccion = destino - origen;
    float distanciaMaxima = glm::length(direccion);

    if (distanciaMaxima <= 0.001f) {
        return false;
    }

    direccion = glm::normalize(direccion);

    for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
        if (!collider.solido) {
            continue;
        }

        for (const TrianguloCollider& tri : collider.triangulos) {
            // Evitamos que el piso bloquee la vista.
            // Si no ponemos esto, el piso podría contar como obstáculo.
            if (tri.normal.y > 0.75f) {
                continue;
            }

            float distanciaImpacto = 0.0f;

            if (raycastTriangulo(origen, direccion, tri, distanciaImpacto)) {
                if (distanciaImpacto > 0.05f && distanciaImpacto < distanciaMaxima) {
                    return true;
                }
            }
        }
    }

    return false;
}

// =====================================================
// ALTURA DE SUELO
// Solo ModeloCentro usa raycast real.
// Fuera del centro, el suelo base queda en 0.
// =====================================================

float obtenerAlturaSueloPerry(const glm::vec3& posicion) {
	// Altura base tomada del mapa de alturas
	float sueloBase = terrain.getHeightTerrain(posicion.x, posicion.z);

	if (meshCollidersEstaticos.empty()) {
		return sueloBase;
	}

	glm::vec3 origen = posicion + glm::vec3(0.0f, 2.0f, 0.0f);
	glm::vec3 direccion = glm::vec3(0.0f, -1.0f, 0.0f);

	float mejorY = sueloBase;
	float menorDistancia = std::numeric_limits<float>::max();
	bool encontroPiso = false;

	for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
		if (!collider.piso) {
			continue;
		}

		for (const TrianguloCollider& tri : collider.triangulos) {
			if (tri.normal.y < 0.70f) {
				continue;
			}

			float distancia = 0.0f;

			if (raycastTriangulo(origen, direccion, tri, distancia)) {
				float yImpacto = origen.y - distancia;

				if (yImpacto > posicion.y + 0.75f) {
					continue;
				}

				if (yImpacto < posicion.y - 3.0f) {
					continue;
				}

				if (distancia < menorDistancia) {
					menorDistancia = distancia;
					mejorY = yImpacto;
					encontroPiso = true;
				}
			}
		}
	}

	if (!encontroPiso) {
		return sueloBase;
	}

	return mejorY;
}

// =====================================================
// ESFERA VS BOX
// =====================================================

static bool esferaContraBox(
	const glm::vec3& centroEsfera,
	float radio,
	const BoxColliderEstatico& box
) {
	glm::vec3 minBox = box.centro - box.mitad;
	glm::vec3 maxBox = box.centro + box.mitad;

	glm::vec3 puntoCercano = glm::clamp(
		centroEsfera,
		minBox,
		maxBox
	);

	float distancia2 = glm::length2(centroEsfera - puntoCercano);

	return distancia2 <= radio * radio;
}

// =====================================================
// PUNTO MAS CERCANO EN TRIANGULO
// =====================================================

static glm::vec3 puntoMasCercanoEnTriangulo(
	const glm::vec3& p,
	const glm::vec3& a,
	const glm::vec3& b,
	const glm::vec3& c
) {
	glm::vec3 ab = b - a;
	glm::vec3 ac = c - a;
	glm::vec3 ap = p - a;

	float d1 = glm::dot(ab, ap);
	float d2 = glm::dot(ac, ap);

	if (d1 <= 0.0f && d2 <= 0.0f) {
		return a;
	}

	glm::vec3 bp = p - b;
	float d3 = glm::dot(ab, bp);
	float d4 = glm::dot(ac, bp);

	if (d3 >= 0.0f && d4 <= d3) {
		return b;
	}

	float vc = d1 * d4 - d3 * d2;

	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		float v = d1 / (d1 - d3);
		return a + v * ab;
	}

	glm::vec3 cp = p - c;
	float d5 = glm::dot(ab, cp);
	float d6 = glm::dot(ac, cp);

	if (d6 >= 0.0f && d5 <= d6) {
		return c;
	}

	float vb = d5 * d2 - d1 * d6;

	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		float w = d2 / (d2 - d6);
		return a + w * ac;
	}

	float va = d3 * d6 - d5 * d4;

	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		float w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return b + w * (c - b);
	}

	float denom = 1.0f / (va + vb + vc);
	float v = vb * denom;
	float w = vc * denom;

	return a + ab * v + ac * w;
}

// =====================================================
// ESFERA VS TRIANGULO
// =====================================================

static bool esferaContraTriangulo(
	const glm::vec3& centroEsfera,
	float radio,
	const TrianguloCollider& tri
) {
	glm::vec3 punto = puntoMasCercanoEnTriangulo(
		centroEsfera,
		tri.a,
		tri.b,
		tri.c
	);

	float distancia2 = glm::length2(centroEsfera - punto);

	return distancia2 <= radio * radio;
}

// =====================================================
// COLISION DE PERRY
// BoxCollider para construcciones.
// MeshCollider solo para centro.
// =====================================================

bool hayColisionPerryConEscena() {
	const float radioPerry = 0.50f;

	glm::vec3 puntosCapsula[4] = {
		posicionPerry + glm::vec3(0.0f, 0.30f, 0.0f),
		posicionPerry + glm::vec3(0.0f, 0.70f, 0.0f),
		posicionPerry + glm::vec3(0.0f, 1.10f, 0.0f),
		posicionPerry + glm::vec3(0.0f, 1.45f, 0.0f)
	};

	// Primero revisamos boxes.
	// Esto es rápido.
	for (const BoxColliderEstatico& box : boxCollidersEstaticos) {
		if (!box.solido) {
			continue;
		}

		for (int i = 0; i < 4; i++) {
			if (esferaContraBox(puntosCapsula[i], radioPerry, box)) {
				return true;
			}
		}
	}

	// Luego revisamos solo el mesh del centro.
	for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
		if (!collider.solido) {
			continue;
		}

		for (const TrianguloCollider& tri : collider.triangulos) {
			// Evitamos que el piso bloquee horizontalmente.
			if (tri.normal.y > 0.75f) {
				continue;
			}

			for (int i = 0; i < 4; i++) {
				if (esferaContraTriangulo(puntosCapsula[i], radioPerry, tri)) {
					return true;
				}
			}
		}
	}

	return false;
}


// =====================================================
// DEBUG VISUAL DE COLLIDERS
// BoxColliders completos + MeshCollider limitado
// =====================================================

static GLuint compilarShaderDebug(GLenum tipo, const char* fuente) {
	GLuint shaderID = glCreateShader(tipo);
	glShaderSource(shaderID, 1, &fuente, nullptr);
	glCompileShader(shaderID);

	GLint compilado = GL_FALSE;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compilado);

	if (!compilado) {
		GLchar log[1024];
		glGetShaderInfoLog(shaderID, 1024, nullptr, log);
		std::cerr << "Error shader debug collider: " << log << std::endl;
		glDeleteShader(shaderID);
		return 0;
	}

	return shaderID;
}

static void crearShaderDebugColliders() {
	if (debugColliderShader != 0) {
		return;
	}

	const char* vertexSrc = R"(
		#version 330 core
		layout(location = 0) in vec3 position;
		uniform mat4 projection;
		uniform mat4 view;

		void main() {
			gl_Position = projection * view * vec4(position, 1.0);
		}
	)";

	const char* fragmentSrc = R"(
		#version 330 core
		out vec4 FragColor;

		void main() {
			FragColor = vec4(1.0, 0.0, 0.0, 1.0);
		}
	)";

	GLuint vs = compilarShaderDebug(GL_VERTEX_SHADER, vertexSrc);
	GLuint fs = compilarShaderDebug(GL_FRAGMENT_SHADER, fragmentSrc);

	if (vs == 0 || fs == 0) {
		return;
	}

	debugColliderShader = glCreateProgram();
	glAttachShader(debugColliderShader, vs);
	glAttachShader(debugColliderShader, fs);
	glLinkProgram(debugColliderShader);

	GLint linkeado = GL_FALSE;
	glGetProgramiv(debugColliderShader, GL_LINK_STATUS, &linkeado);

	if (!linkeado) {
		GLchar log[1024];
		glGetProgramInfoLog(debugColliderShader, 1024, nullptr, log);
		std::cerr << "Error link shader debug collider: " << log << std::endl;
		glDeleteProgram(debugColliderShader);
		debugColliderShader = 0;
	}

	glDeleteShader(vs);
	glDeleteShader(fs);
}

static void agregarLinea(
	std::vector<glm::vec3>& vertices,
	const glm::vec3& a,
	const glm::vec3& b
) {
	vertices.push_back(a);
	vertices.push_back(b);
}

static void agregarBoxDebug(
	std::vector<glm::vec3>& vertices,
	const BoxColliderEstatico& box
) {
	glm::vec3 minP = box.centro - box.mitad;
	glm::vec3 maxP = box.centro + box.mitad;

	glm::vec3 p000(minP.x, minP.y, minP.z);
	glm::vec3 p001(minP.x, minP.y, maxP.z);
	glm::vec3 p010(minP.x, maxP.y, minP.z);
	glm::vec3 p011(minP.x, maxP.y, maxP.z);

	glm::vec3 p100(maxP.x, minP.y, minP.z);
	glm::vec3 p101(maxP.x, minP.y, maxP.z);
	glm::vec3 p110(maxP.x, maxP.y, minP.z);
	glm::vec3 p111(maxP.x, maxP.y, maxP.z);

	agregarLinea(vertices, p000, p001);
	agregarLinea(vertices, p001, p101);
	agregarLinea(vertices, p101, p100);
	agregarLinea(vertices, p100, p000);

	agregarLinea(vertices, p010, p011);
	agregarLinea(vertices, p011, p111);
	agregarLinea(vertices, p111, p110);
	agregarLinea(vertices, p110, p010);

	agregarLinea(vertices, p000, p010);
	agregarLinea(vertices, p001, p011);
	agregarLinea(vertices, p100, p110);
	agregarLinea(vertices, p101, p111);
}

void reconstruirCollidersDebug() {
	std::vector<glm::vec3> vertices;

	for (const BoxColliderEstatico& box : boxCollidersEstaticos) {
		agregarBoxDebug(vertices, box);
	}

	const size_t maxTriangulosDebug = 47611;
	size_t triangulosAgregados = 0;

	for (const MeshColliderEstatico& collider : meshCollidersEstaticos) {
		for (const TrianguloCollider& tri : collider.triangulos) {
			if (triangulosAgregados >= maxTriangulosDebug) {
				break;
			}

			agregarLinea(vertices, tri.a, tri.b);
			agregarLinea(vertices, tri.b, tri.c);
			agregarLinea(vertices, tri.c, tri.a);

			triangulosAgregados++;
		}
	}

	debugColliderVertexCount = static_cast<GLsizei>(vertices.size());

	if (debugColliderVertexCount == 0) {
		std::cout << "Debug colliders: no hay vertices." << std::endl;
		return;
	}

	crearShaderDebugColliders();

	if (debugColliderShader == 0) {
		return;
	}

	if (debugColliderVAO == 0) {
		glGenVertexArrays(1, &debugColliderVAO);
	}

	if (debugColliderVBO == 0) {
		glGenBuffers(1, &debugColliderVBO);
	}

	glBindVertexArray(debugColliderVAO);

	glBindBuffer(GL_ARRAY_BUFFER, debugColliderVBO);
	glBufferData(
		GL_ARRAY_BUFFER,
		vertices.size() * sizeof(glm::vec3),
		vertices.data(),
		GL_STATIC_DRAW
	);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec3),
		(void*)0
	);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	std::cout << "Debug colliders cargado -> vertices: "
			  << debugColliderVertexCount
			  << " | boxes: "
			  << boxCollidersEstaticos.size()
			  << " | triangulos mesh debug: "
			  << triangulosAgregados
			  << std::endl;
}

void renderCollidersDebug(
	const glm::mat4& projection,
	const glm::mat4& view
) {
	if (debugColliderShader == 0 ||
		debugColliderVAO == 0 ||
		debugColliderVertexCount == 0) {
		return;
	}

	GLboolean depthActivo = glIsEnabled(GL_DEPTH_TEST);
	GLboolean cullActivo = glIsEnabled(GL_CULL_FACE);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(debugColliderShader);

	GLint projectionLoc = glGetUniformLocation(debugColliderShader, "projection");
	GLint viewLoc = glGetUniformLocation(debugColliderShader, "view");

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(debugColliderVAO);

	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, debugColliderVertexCount);
	glLineWidth(1.0f);

	glBindVertexArray(0);
	glUseProgram(0);

	if (depthActivo) {
		glEnable(GL_DEPTH_TEST);
	}

	if (cullActivo) {
		glEnable(GL_CULL_FACE);
	}
}

void destruirCollidersDebug() {
	if (debugColliderVBO != 0) {
		glDeleteBuffers(1, &debugColliderVBO);
		debugColliderVBO = 0;
	}

	if (debugColliderVAO != 0) {
		glDeleteVertexArrays(1, &debugColliderVAO);
		debugColliderVAO = 0;
	}

	if (debugColliderShader != 0) {
		glDeleteProgram(debugColliderShader);
		debugColliderShader = 0;
	}

	debugColliderVertexCount = 0;
}

void renderColliderPerryDebug(
	const glm::mat4& projection,
	const glm::mat4& view
) {
	if (debugColliderShader == 0) {
		crearShaderDebugColliders();
	}

	if (debugColliderShader == 0) {
		return;
	}

	const float radioPerry = 0.50f;

	const glm::vec3 centro =
		posicionPerry + glm::vec3(0.0f, 0.85f, 0.0f);

	const glm::vec3 mitad =
		glm::vec3(radioPerry, 0.85f, radioPerry);

	glm::vec3 minP = centro - mitad;
	glm::vec3 maxP = centro + mitad;

	glm::vec3 p000(minP.x, minP.y, minP.z);
	glm::vec3 p001(minP.x, minP.y, maxP.z);
	glm::vec3 p010(minP.x, maxP.y, minP.z);
	glm::vec3 p011(minP.x, maxP.y, maxP.z);

	glm::vec3 p100(maxP.x, minP.y, minP.z);
	glm::vec3 p101(maxP.x, minP.y, maxP.z);
	glm::vec3 p110(maxP.x, maxP.y, minP.z);
	glm::vec3 p111(maxP.x, maxP.y, maxP.z);

	glm::vec3 vertices[] = {
		p000, p001,
		p001, p101,
		p101, p100,
		p100, p000,

		p010, p011,
		p011, p111,
		p111, p110,
		p110, p010,

		p000, p010,
		p001, p011,
		p100, p110,
		p101, p111
	};

	GLuint vao = 0;
	GLuint vbo = 0;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(vertices),
		vertices,
		GL_DYNAMIC_DRAW
	);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(glm::vec3),
		(void*)0
	);

	GLboolean depthActivo = glIsEnabled(GL_DEPTH_TEST);
	GLboolean cullActivo = glIsEnabled(GL_CULL_FACE);

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glUseProgram(debugColliderShader);

	GLint projectionLoc = glGetUniformLocation(debugColliderShader, "projection");
	GLint viewLoc = glGetUniformLocation(debugColliderShader, "view");

	glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	glBindVertexArray(vao);

	glLineWidth(3.0f);
	glDrawArrays(GL_LINES, 0, 24);
	glLineWidth(1.0f);

	glBindVertexArray(0);
	glUseProgram(0);

	if (depthActivo) {
		glEnable(GL_DEPTH_TEST);
	}

	if (cullActivo) {
		glEnable(GL_CULL_FACE);
	}

	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);
}

// ===================== RECOLECTABLES =====================

void cargarModeloRecolectable1() {
    if (modeloRecolectable1Cargado) {
        return;
    }

    std::cout << "Cargando modelo Recolectable 1..." << std::endl;

    Recolectable1.loadModel(
        resolverRutaArchivo("../AssetsPfinal/Objetos/Objeto01/Objeto01.fbx")
    );

    // AQUÍ SE LE ASIGNA EL SHADER FRESNEL
    Recolectable1.setShader(&shaderFresnel);

    Recolectable1.setAnimationIndex(0);

    modeloRecolectable1Cargado = true;

    std::cout << "Modelo Recolectable 1 cargado con shader Fresnel." << std::endl;
}

void cargarModeloRecolectable2() {
    if (modeloRecolectable2Cargado) {
        return;
    }

    std::cout << "Cargando modelo Recolectable 2..." << std::endl;

    Recolectable2.loadModel(
        resolverRutaArchivo("../AssetsPfinal/Objetos/Objeto03/Objeto3.fbx")
    );

    // AQUÍ SE LE ASIGNA EL SHADER FRESNEL
    Recolectable2.setShader(&shaderFresnel);

    Recolectable2.setAnimationIndex(0);

    modeloRecolectable2Cargado = true;

    std::cout << "Modelo Recolectable 2 cargado con shader Fresnel." << std::endl;
}
void cargarModeloRecolectable3() {
    if (modeloRecolectable3Cargado) {
        return;
    }

    std::cout << "Cargando modelo Recolectable 3..." << std::endl;

    Recolectable3.loadModel(
        resolverRutaArchivo("../AssetsPfinal/Objetos/Objeto02/Objeto02.fbx")
    );

    Recolectable3.setShader(&shaderFresnel);
    Recolectable3.setAnimationIndex(0);

    modeloRecolectable3Cargado = true;

    std::cout << "Modelo Recolectable 3 cargado con shader Fresnel." << std::endl;
}