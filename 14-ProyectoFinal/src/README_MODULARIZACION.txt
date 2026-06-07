ESTRUCTURA MODULAR PROPUESTA

Copia estos archivos en la carpeta donde estaba tu main.cpp original.
No muevas tus carpetas Headers, Shaders, Textures ni AssetsPfinal.

Archivos principales:

main.cpp
- Solo arranca la app.

Application.h / Application.cpp
- Inicializa GLFW, GLEW, shaders, cámara, modelos y texturas.
- Contiene el loop principal, callbacks, input y destroy.

Globals.h / Globals.cpp
- Variables globales compartidas: shaders, cámara, modelos, texturas, ventana, tiempos.

Scene.h
- Clase base de escena, estilo Unity: onEnter, onExit, update, render.

SceneManager.h / SceneManager.cpp
- Administra qué escena está activa.
- Cambia entre escenas.

PuebloScene.h / PuebloScene.cpp
- Escena donde se renderiza el pueblo completo.

EscenaVacia.h / EscenaVacia.cpp
- Escena de prueba. Dibuja piso y skybox.

RenderUtils.h / RenderUtils.cpp
- Funciones comunes de render y carga:
  cargarModelosPueblo
  cargarSkybox
  cargarTexturaCesped
  renderPiso
  renderSkybox
  renderConstrucciones4
  configurarLucesBasicas

Cambio de escenas:
- Tecla 1: PuebloScene
- Tecla 2: EscenaVacia

Para crear otra escena:
1. Crea MiEscena.h y MiEscena.cpp.
2. Hereda de Scene.
3. Implementa onEnter y render.
4. En Application.cpp agrega:
   #include "MiEscena.h"
   sceneManager.addScene(std::make_shared<MiEscena>());
5. En keyCallback agrega una tecla para sceneManager.setScene(indice).
