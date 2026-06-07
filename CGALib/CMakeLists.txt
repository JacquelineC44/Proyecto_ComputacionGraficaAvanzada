add_definitions(-DBUILDING_DLL)

file(GLOB CGALib_SRC
     "./include/Headers/*.h"
     "./src/*.cpp"
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     include_directories(
          CGALib
          "./include/"
          "../external/assimp/include"
          "../external/glfw/include"
          "../external/glm"
          "../external/glew/include"
          "../external/FreeImage/include"
          "../external/OpenAL/include"
          "../external/Freetype/include"
          "../external/freealut/include"
          "C:/msys64/mingw64/include"
     )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
     include_directories(
          CGALib
          "./include/"
          "/usr/include/freetype2"
     )
endif()

add_library(CGALib SHARED ${CGALib_SRC})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
     set_target_properties(CGALib
          PROPERTIES
          RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
     )

     target_link_libraries(CGALib
          opengl32
          gdi32
          glu32

          ${CMAKE_SOURCE_DIR}/external/glfw/lib/libglfw3dll.a
          ${CMAKE_SOURCE_DIR}/external/glew/lib/libglew32.dll.a
          ${CMAKE_SOURCE_DIR}/external/assimp/lib/libassimp.dll.a
          ${CMAKE_SOURCE_DIR}/external/Freetype/lib/libfreetype.dll.a
     )

elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
     target_link_libraries(CGALib
          glut
          glfw
          GLEW
          GLU
          GL
          freeimage
          assimp
          freetype
     )
endif()