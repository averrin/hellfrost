cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

project(hellfrost)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXE_NAME ${PROJECT_NAME} )

add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_PATH}/include/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
include_directories(
  "${PROJECT_PATH}/imgui/imgui"
  "${PROJECT_PATH}/imgui/include"
  "${PROJECT_PATH}/imgui/imgui/examples/libs/glfw/include"
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/include/imgui-sfml"
  )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(imgui)
set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS CONFIG REQUIRED system window graphics)

file(GLOB IMLIB_SFML_SOURCE "${PROJECT_PATH}/include/imgui-sfml/*.cpp" "${PROJECT_PATH}/include/imgui-sfml/*.h")
add_library(imgui-sfml ${IMLIB_SFML_SOURCE})

set(OpenGL_GL_PREFERENCE "GLVND")
# set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
# include_directories(${SFML_INCLUDE_DIR})
include_directories("imgui")

file(GLOB SOURCE "src/*.cpp")
file(GLOB FONTS "${PROJECT_PATH}/include/app/fonts/*.cpp" "${PROJECT_PATH}/include/app/fonts/*.h")
add_executable(${EXE_NAME}
  ${IMLIB_SFML_SOURCE}

  ${PROJECT_PATH}/include/EventBus/EventBus.cpp
  ${PROJECT_PATH}/include/EventBus/EventRegisttration.cpp
  ${PROJECT_PATH}/include/micropather/micropather.cpp

  ${SOURCE}
  ${PROJECT_PATH}/include/app/style/theme.cpp

  ${PROJECT_PATH}/src/game/randomTools.cpp
  ${PROJECT_PATH}/src/game/fov.cpp
  ${PROJECT_PATH}/src/game/cell.cpp
  # ${PROJECT_PATH}/src/game/terrain.cpp
  ${PROJECT_PATH}/src/game/location.cpp
  ${PROJECT_PATH}/src/game/object.cpp
  ${PROJECT_PATH}/src/game/door.cpp
  ${PROJECT_PATH}/src/game/effect.cpp
  ${PROJECT_PATH}/src/game/item.cpp
  ${PROJECT_PATH}/src/game/damageSpec.cpp
  ${PROJECT_PATH}/src/game/spell.cpp
  ${PROJECT_PATH}/src/game/enemy.cpp
  ${PROJECT_PATH}/src/game/creature.cpp
  ${PROJECT_PATH}/src/game/equipment.cpp
  ${PROJECT_PATH}/src/game/slot.cpp
  ${PROJECT_PATH}/src/generator/generator.cpp
  ${PROJECT_PATH}/src/game/gameManager.cpp

  ${PROJECT_PATH}/include/app/style/theme.cpp

  ${FONTS}
  )

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(${EXE_NAME} PRIVATE imgui-sfml fmt
  OpenGL
  stdc++fs
  imgui ${CONAN_LIBS})
else()
target_link_libraries(${EXE_NAME} PRIVATE imgui-sfml fmt
  OpenGL
  imgui ${CONAN_LIBS})
endif()
