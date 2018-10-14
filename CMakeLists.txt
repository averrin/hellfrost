cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)

project(siuis)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set( EXE_NAME ${PROJECT_NAME} )
include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_PATH}/include/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
include_directories(
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/include/imgui"
  "${PROJECT_PATH}/include/imgui-sfml"
  )

file(GLOB IMLIB_SFML_SOURCE "${PROJECT_PATH}/include/imgui-sfml/*.cpp" "${PROJECT_PATH}/include/imgui-sfml/*.h")
add_library(imgui-sfml ${IMLIB_SFML_SOURCE})

set(OpenGL_GL_PREFERENCE "GLVND")
# set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${SFML_INCLUDE_DIR})

file(GLOB SOURCE "src/*.cpp")
add_executable(${EXE_NAME}
  ${IMLIB_SOURCE}
  ${IMLIB_SFML_SOURCE}

  ${PROJECT_PATH}/src/application.cpp
  ${PROJECT_PATH}/src/main.cpp
  )
target_link_libraries(${EXE_NAME} PRIVATE ${OPENGL_LIBRARIES} ${CONAN_LIBS} imgui-sfml)
