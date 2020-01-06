cmake_minimum_required(VERSION 3.11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_BUILD_TYPE Debug)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

project(hellfrost)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXE_NAME ${PROJECT_NAME} )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(LIBS "${PROJECT_PATH}/libs" )
include_directories(
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/src"
  "${LIBS}"
  "${LIBS}/3rdparty"
  "${LIBS}/3rdparty/duktape"
  "${LIBS}/3rdparty/imgui"
  "${LIBS}/3rdparty/imgui/imgui"
  "${LIBS}/3rdparty/imgui/include"
  "${LIBS}/3rdparty/imgui/imgui/examples/libs/glfw/include"
  "${LIBS}/3rdparty/imgui-sfml"
)

set(SFML_STATIC_LIBRARIES TRUE)
find_package(SFML 2.5 COMPONENTS CONFIG REQUIRED system window graphics)

add_subdirectory(libs/3rdparty/fmt EXCLUDE_FROM_ALL)

add_subdirectory("${LIBS}/3rdparty/imgui")
add_definitions(-DIMGUI_USER_CONFIG="${LIBS}/3rdparty/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
file(GLOB IMLIB_SFML_SOURCE "${LIBS}/3rdparty/imgui-sfml/*.cpp" "${LIBS}/3rdparty/imgui-sfml/*.h")
add_library(imgui-sfml ${IMLIB_SFML_SOURCE})

set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

include(${PROJECT_PATH}/cmake/hellfrost.cmake)

file(GLOB FONTS "${PROJECT_PATH}/src/fonts/*.cpp" "${PROJECT_PATH}/src/fonts/*.h")
add_executable(${EXE_NAME}
  ${IMLIB_SFML_SOURCE}
  ${LIBS}/3rdparty/duktape/duktape.c
  ${LIBS}/3rdparty/imguicolortextedit/TextEditor.cpp

  ${LIBS}/3rdparty/micropather/micropather.cpp

  ${FONTS}
  ${PROJECT_PATH}/src/style/theme.cpp
  ${PROJECT_PATH}/libs/imgui-etc/imgui-stl.cpp

  ${HELLFROST_SRC}

  ${PROJECT_PATH}/src/editor/duk_console.cpp
  ${PROJECT_PATH}/src/editor/console.cpp
  ${PROJECT_PATH}/src/editor/editor.cpp

  ${PROJECT_PATH}/src/application.cpp
  ${PROJECT_PATH}/src/main.cpp
)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
target_link_libraries(${EXE_NAME} PRIVATE imgui-sfml fmt
  OpenGL
  stdc++fs
  fmt::fmt-header-only
  tbb
  imgui ${CONAN_LIBS})
else()
target_link_libraries(${EXE_NAME} PRIVATE imgui-sfml fmt
  OpenGL
  imgui ${CONAN_LIBS})
endif()
