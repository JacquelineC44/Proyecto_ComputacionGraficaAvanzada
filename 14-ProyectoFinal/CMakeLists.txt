add_definitions(-DBUILDING_DLL)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/CGALib/include
        ${CMAKE_SOURCE_DIR}/external/assimp/include
        ${CMAKE_SOURCE_DIR}/external/glfw/include
        ${CMAKE_SOURCE_DIR}/external/glm
        ${CMAKE_SOURCE_DIR}/external/glew/include
        ${CMAKE_SOURCE_DIR}/external/FreeImage/include
        ${CMAKE_SOURCE_DIR}/external/OpenAL/include
        ${CMAKE_SOURCE_DIR}/external/Freetype/include
        ${CMAKE_SOURCE_DIR}/external/freealut/include

        # SDL2 y SDL2_mixer desde MSYS2
        C:/msys64/mingw64/include
    )

    link_directories(
        ${CMAKE_SOURCE_DIR}/external/assimp/lib
        ${CMAKE_SOURCE_DIR}/external/glfw/lib
        ${CMAKE_SOURCE_DIR}/external/Freetype/lib
        ${CMAKE_SOURCE_DIR}/external/FreeImage/lib
        ${CMAKE_SOURCE_DIR}/external/glew/lib
        ${CMAKE_SOURCE_DIR}/external/freealut/lib
        ${CMAKE_SOURCE_DIR}/external/OpenAL/lib/x64

        # SDL2 y SDL2_mixer desde MSYS2
        C:/msys64/mingw64/lib
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/CGALib/include
    )
endif()

add_executable(ProyectoFinal
    ./src/main.cpp
    ./src/Application.cpp
    ./src/Globals.cpp
    ./src/SceneManager.cpp
    ./src/PuebloScene.cpp
    ./src/CasinoScene.cpp
    ./src/BolosScene.cpp
    ./src/RenderUtils.cpp
)

set_target_properties(ProyectoFinal
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "../../bin"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(
        ProyectoFinal
        opengl32.lib
        libglfw3dll.dll.a
        libglew32.dll.a
        libassimp.dll.a
        libfreetype.dll.a
        libalut.dll.a
        OpenAL32.lib

        # SDL2 / SDL2_mixer
        libSDL2.dll.a
        libSDL2_mixer.dll.a

        CGALib
    )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_link_libraries(
        ProyectoFinal
        glut
        glfw
        GLEW
        GLU
        GL
        freeimage
        assimp
        freetype
        alut
        openal
        SDL2
        SDL2_mixer
        CGALib
    )
endif()