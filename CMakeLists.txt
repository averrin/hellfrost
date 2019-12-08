cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_TOOLCHAIN_FILE "../vcpkg/scripts/buildsystems/vcpkg.cmake")
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

project(hellfrost)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set( EXE_NAME ${PROJECT_NAME} )

add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_PATH}/include/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
include_directories(
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/include/imgui-sfml"
  )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_subdirectory(imgui)
# find_package(fmt CONFIG REQUIRED)
# find_package(imgui CONFIG REQUIRED)
# find_package(SFML 2 COMPONENTS CONFIG REQUIRED system window graphics)
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

# find_path(IMLIB_SOURCE imgui.h)
# include_directories(${IMLIB_SOURCE})

file(GLOB SOURCE "src/*.cpp")
add_executable(${EXE_NAME}
  ${IMLIB_SFML_SOURCE}

  ${PROJECT_PATH}/include/EventBus/EventBus.cpp
  ${PROJECT_PATH}/include/EventBus/EventRegisttration.cpp
  ${PROJECT_PATH}/include/micropather/micropather.cpp

  ${PROJECT_PATH}/src/application.cpp
  ${PROJECT_PATH}/src/main.cpp

  ${PROJECT_PATH}/src/game/randomTools.cpp
  ${PROJECT_PATH}/src/game/fov.cpp
  ${PROJECT_PATH}/src/game/cell.cpp
  # ${PROJECT_PATH}/src/game/terrain.cpp
  ${PROJECT_PATH}/src/game/location.cpp
  ${PROJECT_PATH}/src/game/object.cpp
  ${PROJECT_PATH}/src/game/door.cpp
  ${PROJECT_PATH}/src/generator/generator.cpp

  ${PROJECT_PATH}/include/app/style/theme.cpp

  ${PROJECT_PATH}/include/app/fonts/fonts.h
  ${PROJECT_PATH}/include/app/fonts/Roboto-Black.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-BlackItalic.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-Bold.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-BoldItalic.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-Italic.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-Light.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-LightItalic.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-Medium.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-MediumItalic.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-Regular.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-Thin.cpp
  ${PROJECT_PATH}/include/app/fonts/Roboto-ThinItalic.cpp
  ${PROJECT_PATH}/include/app/fonts/Inconsolata-Regular.cpp
  ${PROJECT_PATH}/include/app/fonts/Inconsolata-Bold.cpp
  ${PROJECT_PATH}/include/app/fonts/material_design_icons.cpp
  ${PROJECT_PATH}/include/app/fonts/material_design_icons.h
  )

target_link_libraries(${EXE_NAME} PRIVATE imgui-sfml fmt
  OpenGL GL imgui ${CONAN_LIBS})
