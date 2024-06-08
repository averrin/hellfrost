cmake_minimum_required(VERSION 3.29.3)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_VERSION 2.6.1)
set(IMGUI_VERSION 1.90.6)

set(CMAKE_EXPORT_COMPILE_COMMANDS true)
# set(CMAKE_BUILD_TYPE Debug)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

project(hellfrost)
set(PROJECT_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
set(EXE_NAME ${PROJECT_NAME} )

include(cmake/CPM.cmake)
CPMAddPackage("gh:fmtlib/fmt#10.2.1")
CPMAddPackage("gh:averrin/libcolor#master")
CPMAddPackage("gh:averrin/libprint#master")
CPMAddPackage("gh:averrin/liblog#master")
CPMAddPackage("gh:juliettef/IconFontCppHeaders#main")
#CPMAddPackage("gh:Green-Sky/imgui_entt_entity_editor#master")
# CPMAddPackage("gh:USCiLab/cereal#master")

# FetchContent_Declare(
#   cereal
#   URL "https://github.com/USCiLab/cereal/archive/refs/tags/v1.3.2.zip"
# )

FetchContent_Declare(
  imgui
  # URL "https://github.com/ocornut/imgui/archive/v${IMGUI_VERSION}.zip"
  URL "https://github.com/ocornut/imgui/archive/refs/heads/docking.zip"
)
FetchContent_MakeAvailable(imgui)
# FetchContent_MakeAvailable(cereal)

FetchContent_Declare(
  SFML
  URL "https://github.com/SFML/SFML/archive/${SFML_VERSION}.zip"
)

FetchContent_Declare(
  taskflow
  URL "https://github.com/taskflow/taskflow/archive/refs/tags/v3.7.0.zip"
)

FetchContent_Declare(
  entt
  URL "https://github.com/skypjack/entt/releases/download/v3.13.2/entt-v3.13.2.tar.gz"
)
option(SFML_BUILD_AUDIO "Build audio" OFF)
option(SFML_BUILD_NETWORK "Build network" OFF)
set(OpenGL_GL_PREFERENCE "GLVND")
# set(OpenGL_GL_PREFERENCE "LEGACY")
find_package(OpenGL REQUIRED)
FetchContent_MakeAvailable(sfml)
FetchContent_MakeAvailable(taskflow)
FetchContent_MakeAvailable(entt)

FetchContent_Declare(
  lua
  URL "https://www.lua.org/ftp/lua-5.4.6.tar.gz"
  )
FetchContent_MakeAvailable(lua)
file(GLOB LUA_SOURCE "${lua_SOURCE_DIR}/src/*.c")
list(REMOVE_ITEM LUA_SOURCE "${lua_SOURCE_DIR}/src/lua.c")
list(REMOVE_ITEM LUA_SOURCE "${lua_SOURCE_DIR}/src/luac.c")
# find_package(Lua54 REQUIRED)

# Or by tag...
FetchContent_Declare(
  imgui-sfml
  GIT_REPOSITORY https://github.com/SFML/imgui-sfml.git
  GIT_TAG        2.6.x
)
# ImGui-SFML
set(IMGUI_DIR ${imgui_SOURCE_DIR})
option(IMGUI_SFML_FIND_SFML "Use find_package to find SFML" OFF)
option(IMGUI_SFML_IMGUI_DEMO "Build imgui_demo.cpp" ON)
FetchContent_MakeAvailable(imgui-sfml)

# add_definitions(-DIMGUI_USER_CONFIG="${PROJECT_PATH}/include/3rdparty/imgui-sfml/imconfig-SFML.h")
add_definitions(-D_USE_MATH_DEFINES)
include_directories(
  "${PROJECT_PATH}/include"
)
find_package(OpenGL REQUIRED)

file(GLOB IMGUI_SOURCE "${IMGUI_DIR}/*.cpp")
file(GLOB SOURCE "src/*.cpp")
file(GLOB FONTS "${PROJECT_PATH}/include/app/fonts/*.cpp" "${PROJECT_PATH}/include/app/fonts/*.h")
add_executable(${EXE_NAME}
  # src/main.cpp
  ${SOURCE}
  ${PROJECT_PATH}/include/3rdparty/imguicolortextedit/TextEditor.cpp

  ${PROJECT_PATH}/include/EventBus/EventBus.cpp
  ${PROJECT_PATH}/include/EventBus/EventRegisttration.cpp
  ${PROJECT_PATH}/include/3rdparty/micropather/micropather.cpp

  ${PROJECT_PATH}/include/app/style/theme.cpp
  ${PROJECT_PATH}/include/imgui-stl.cpp
  # ${PROJECT_PATH}/include/imgui_freetype.cpp

  ${PROJECT_PATH}/src/deps.cpp
  ${PROJECT_PATH}/src/controls.cpp
  ${PROJECT_PATH}/src/game/gameData.cpp
  ${PROJECT_PATH}/src/game/randomTools.cpp
  ${PROJECT_PATH}/src/game/fov.cpp
  ${PROJECT_PATH}/src/game/cell.cpp
  ${PROJECT_PATH}/src/game/room.cpp
  # ${PROJECT_PATH}/src/game/terrain.cpp
  ${PROJECT_PATH}/src/game/location.cpp
  ${PROJECT_PATH}/src/game/player.cpp
  ${PROJECT_PATH}/src/game/object.cpp
  ${PROJECT_PATH}/src/game/door.cpp
  ${PROJECT_PATH}/src/game/effect.cpp
  ${PROJECT_PATH}/src/game/item.cpp
  ${PROJECT_PATH}/src/game/damageSpec.cpp
  ${PROJECT_PATH}/src/game/spell.cpp
  # ${PROJECT_PATH}/src/game/enemy.cpp
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

  # ${IMGUI_SOURCE}
  # ${IMLIB_SFML_SOURCE}
  ${LUA_SOURCE}
  )

if(libcolor_ADDED)
  target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${libcolor_SOURCE_DIR}/include"})
endif()
if(libprint_ADDED)
  target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${libprint_SOURCE_DIR}/include")
endif()
if(liblog_ADDED)
  target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${liblog_SOURCE_DIR}/include")
endif()
if(IconFontCppHeaders_ADDED)
  target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${IconFontCppHeaders_SOURCE_DIR}")
endif()
#if(imgui_entt_entity_editor_ADDED)
#  target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${imgui_entt_entity_editor_SOURCE_DIR}")
#endif()
# if(cereal_ADDED)
#   target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${cereal_SOURCE_DIR}/include")
# endif()

target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${lua_SOURCE_DIR}/src")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${PROJECT_PATH}/include/3rdparty")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${PROJECT_PATH}/include/3rdparty/cereal/include")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC ${OPENGL_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${taskflow_SOURCE_DIR}")
target_include_directories(${PROJECT_NAME} SYSTEM PUBLIC "${entt_SOURCE_DIR}/single_include")

target_compile_options(${PROJECT_NAME} PRIVATE -Wno-format-security -Wno-switch)


set(BACKWARD_HAS_DWARF 1)
set(BACKWARD_HAS_DW 1)

target_link_libraries(${EXE_NAME} PRIVATE ImGui-SFML::ImGui-SFML)


if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  target_link_libraries(${EXE_NAME} PRIVATE
      fmt
      OpenGL
      stdc++fs
      sfml-system sfml-window sfml-graphics
      lua
  )
else()
target_link_libraries(${EXE_NAME} PRIVATE
  fmt
  OpenGL
  sfml-system sfml-window sfml-graphics
)
endif()
