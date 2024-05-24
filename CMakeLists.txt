cmake_minimum_required(VERSION 3.11)

# set(CMAKE_C_COMPILER "clang")
# set(CMAKE_CXX_COMPILER "clang++")

set(CONAN_DISABLE_CHECK_COMPILER true)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)
set(CMAKE_BUILD_TYPE Debug)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

project(hellfrost)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXE_NAME ${PROJECT_NAME} )

include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#9.1.0")
CPMAddPackage("gh:averrin/libcolor#master")
if(libcolor_ADDED)
    include_directories("${libcolor_SOURCE_DIR}/include")
endif()
CPMAddPackage("gh:averrin/libprint#master")
if(libprint_ADDED)
    include_directories("${libprint_SOURCE_DIR}/include")
endif()
CPMAddPackage("gh:averrin/liblog#master")
if(liblog_ADDED)
    include_directories("${liblog_SOURCE_DIR}/include")
endif()
# CPMAddPackage("gh:ThePhD/sol2#master")


# add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_PATH}/include/3rdparty/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
include_directories(
  "${PROJECT_PATH}/include"
  "${PROJECT_PATH}/include/3rdparty"
  "${PROJECT_PATH}/include/3rdparty/imgui"
  )

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

# add_subdirectory(imgui)
# set(SFML_STATIC_LIBRARIES TRUE)
# find_package(SFML 2.5 COMPONENTS CONFIG REQUIRED system window graphics)

# file(GLOB IMLIB_SFML_SOURCE "${PROJECT_PATH}/include/3rdparty/imgui-sfml/*.cpp" "${PROJECT_PATH}/include/3rdparty/imgui-sfml/*.h")
# add_library(imgui-sfml ${IMLIB_SFML_SOURCE})

set(OpenGL_GL_PREFERENCE "GLVND")
# set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
# include_directories(${SFML_INCLUDE_DIR})
# include_directories("imgui")

file(GLOB SOURCE "src/*.cpp")
file(GLOB FONTS "${PROJECT_PATH}/include/app/fonts/*.cpp" "${PROJECT_PATH}/include/app/fonts/*.h")
add_executable(${EXE_NAME}
  ${IMLIB_SFML_SOURCE}
  ${SOURCE}
  ${PROJECT_PATH}/include/3rdparty/imguicolortextedit/TextEditor.cpp

  ${PROJECT_PATH}/include/EventBus/EventBus.cpp
  ${PROJECT_PATH}/include/EventBus/EventRegisttration.cpp
  ${PROJECT_PATH}/include/3rdparty/micropather/micropather.cpp

  ${PROJECT_PATH}/include/app/style/theme.cpp
  ${PROJECT_PATH}/include/imgui-stl.cpp
  # ${PROJECT_PATH}/include/imgui_freetype.cpp

  ${PROJECT_PATH}/src/deps.cpp
  ${PROJECT_PATH}/src/game/gameData.cpp
  ${PROJECT_PATH}/src/game/randomTools.cpp
  ${PROJECT_PATH}/src/game/fov.cpp
  ${PROJECT_PATH}/src/game/cell.cpp
  ${PROJECT_PATH}/src/game/room.cpp
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

  ${PROJECT_PATH}/src/editor.cpp
  ${PROJECT_PATH}/src/drawEngine.cpp
  ${PROJECT_PATH}/src/layers.cpp
  ${PROJECT_PATH}/src/viewport.cpp

  ${FONTS}
  ${BACKWARD_ENABLE}

  "${PROJECT_PATH}/include/3rdparty/imgui/imgui_demo.cpp"
  )

# add_backward(${EXE_NAME})
# target_link_libraries(${EXE_NAME} PUBLIC Backward::Backward)
set(BACKWARD_HAS_DWARF 1)
set(BACKWARD_HAS_DW 1)

find_package(ImGui-SFML REQUIRED)
target_link_libraries(${EXE_NAME} PRIVATE ImGui-SFML::ImGui-SFML)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(${EXE_NAME} PRIVATE
      # ImGui-SFML
      fmt
      OpenGL
      stdc++fs
      sfml-system sfml-window sfml-graphics
      lua
      # imgui
      ${CONAN_LIBS}
  )
else()
target_link_libraries(${EXE_NAME} PRIVATE ImGui-SFML fmt
  OpenGL
  imgui ${CONAN_LIBS})
endif()
# target_compile_options(${EXE_NAME} PRIVATE -ftime-report -Q)
