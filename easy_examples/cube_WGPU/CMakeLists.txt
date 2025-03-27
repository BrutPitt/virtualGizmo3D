#------------------------------------------------------------------------------
#  Copyright (c) 2025 Michele Morrone
#  All rights reserved.
#
#  https://michelemorrone.eu - https://brutpitt.com
#
#  X: https://x.com/BrutPitt - GitHub: https://github.com/BrutPitt
#
#  direct mail: brutpitt(at)gmail.com - me(at)michelemorrone.eu
#
#  This software is distributed under the terms of the BSD 2-Clause license
#------------------------------------------------------------------------------

# Building for desktop (WebGPU-native) with Dawn:
#  1. git clone https://github.com/google/dawn dawn
#  2. cmake -B build -DCURRENT_DAWN_DIR=dawn
#  3. cmake --build build

# Building for Emscripten:
#  1. Install Emscripten SDK following the instructions: https://emscripten.org/docs/getting_started/downloads.html
#  2. Install Ninja build system
#  3. emcmake cmake -G Ninja -B build
#  3. cmake --build build
#  4. emrun build/wgpu_mandelbrot.html
# or
#  4. python -m http.server ... then open WGPU browser with url: http://localhost:8000/wgpu_mandelbrot.html

cmake_minimum_required(VERSION 3.16) # DAWN required
project(vGizmo3D_wgpuCube)

set(APP_NAME vGizmo3D_wgpuCube)


if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17) # Dawn requires C++17

# Libraries
if(EMSCRIPTEN)
  if(EMSCRIPTEN_VERSION VERSION_GREATER_EQUAL "3.1.57")
    set(APP_EMSCRIPTEN_GLFW3 "--use-port=contrib.glfw3")
  else()
    message( FATAL_ERROR "Is necessary a version of EMISCRIPTEN > v.3.1.57, please update" )
  endif()
  set(ENABLE_EMSCRIPTEN ON)
  set(LIBRARIES glfw)
  add_compile_options(-sDISABLE_EXCEPTION_CATCHING=1)
else()
  # Dawn wgpu desktop
  set(DAWN_FETCH_DEPENDENCIES ON)
  set(CURRENT_DAWN_DIR CACHE PATH "Path to Dawn repository")
  if (NOT CURRENT_DAWN_DIR)
    message(FATAL_ERROR "Please specify the Dawn repository by setting CURRENT_DAWN_DIR")
  endif()

  option(DAWN_FETCH_DEPENDENCIES "Use fetch_dawn_dependencies.py as an alternative to using depot_tools" ON)

  # Dawn builds many things by default - disable things we don't need
  option(DAWN_BUILD_SAMPLES "Enables building Dawn's samples" OFF)
  option(TINT_BUILD_CMD_TOOLS "Build the Tint command line tools" OFF)
  option(TINT_BUILD_DOCS "Build documentation" OFF)
  option(TINT_BUILD_TESTS "Build tests" OFF)
  if (NOT APPLE)
    option(TINT_BUILD_MSL_WRITER "Build the MSL output writer" OFF)
  endif()
  if(WIN32)
    option(TINT_BUILD_SPV_READER "Build the SPIR-V input reader" OFF)
    option(TINT_BUILD_WGSL_READER "Build the WGSL input reader" ON)
    option(TINT_BUILD_GLSL_WRITER "Build the GLSL output writer" OFF)
    option(TINT_BUILD_GLSL_VALIDATOR "Build the GLSL output validator" OFF)
    option(TINT_BUILD_SPV_WRITER "Build the SPIR-V output writer" OFF)
    option(TINT_BUILD_WGSL_WRITER "Build the WGSL output writer" ON)
  endif()

  # check if WAYLAND is the current Session Type and enable DAWN_USE_WAYLAND Wayland option @compile time
  # You can override this using: cmake -DDAWN_USE_WAYLAND=X (X = ON | OFF)
  if(UNIX)
    if ($ENV{XDG_SESSION_TYPE} MATCHES wayland)
      option(DAWN_USE_WAYLAND "Enable support for Wayland surface" OFF)
    endif()
  endif()

  set(TARGET_DAWN_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/dawn CACHE STRING "Directory where to build DAWN")
  add_subdirectory("${CURRENT_DAWN_DIR}" "${TARGET_DAWN_DIRECTORY}" EXCLUDE_FROM_ALL)

  set(LIBRARIES webgpu_dawn webgpu_cpp webgpu_glfw glfw)
endif()

set(SRC          ${CMAKE_SOURCE_DIR})
set(COMMONS_DIR  ${SRC}/../commons)
set(TOOLS_DIR    ${SRC}/../../libs)
set(VGIZMO_PARENT_DIR ${SRC}/../..)
set(VGIZMO_DIR ${VGIZMO_PARENT_DIR}/vGizmo3D)

include_directories(${TOOLS_DIR})
include_directories(${VGIZMO_PARENT_DIR}) # CLion: when I add follow line COMMONS_DIR and VGIZMO_DIR are removed from project and relative contained files not belong to project
include_directories(${COMMONS_DIR})
include_directories(${VGIZMO_DIR})

message(${VGIZMO_PARENT_DIR})

set(SOURCE_FILES
        ${SRC}/main.cpp
        ${VGIZMO_DIR}/vGizmo3D.h
        ${VGIZMO_DIR}/vgMath.h
        ${VGIZMO_DIR}/vGizmo3D_config.h
        ${VGIZMO_DIR}/vgMath_config.h
        ${COMMONS_DIR}/shadersAndModel.h)

add_executable(${APP_NAME} ${SOURCE_FILES})

target_link_libraries(${APP_NAME} LINK_PUBLIC ${LIBRARIES})

# Emscripten settings
if(EMSCRIPTEN)
  set(CMAKE_EXECUTABLE_SUFFIX ".html")

  target_compile_options(${APP_NAME} PUBLIC "${APP_EMSCRIPTEN_GLFW3}" )
  target_link_options(${APP_NAME} PRIVATE
    "-sUSE_WEBGPU=1"
    "${APP_EMSCRIPTEN_GLFW3}"
    "-sWASM=1"
    "-sASYNCIFY"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sNO_EXIT_RUNTIME=0"
    "-sASSERTIONS=1"
    "-sDISABLE_EXCEPTION_CATCHING=1"
    "-sNO_FILESYSTEM=1"
    "--shell-file=${CMAKE_SOURCE_DIR}/veryMinimal_wgpu.html"
  )
endif()

file(WRITE ${CMAKE_SOURCE_DIR}/.idea/.name ${PROJECT_NAME}) # used to rename a Project in clion (run once)