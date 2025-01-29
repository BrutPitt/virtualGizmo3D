#------------------------------------------------------------------------------
#  Copyright (c) 2018-2025 Michele Morrone
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
cmake_minimum_required(VERSION 3.27)

option(IMGUIZMO_USES_GLM "Use GLM instead of internal vgMath" OFF)

set(CMAKE_CXX_STANDARD 17)

set(SRC          ${CMAKE_SOURCE_DIR})
set(COMMONS_DIR  ${SRC}/../commons)
set(VGIZMO_PARENT_DIR ${SRC}/../../..)
set(TOOLS_DIR  ${VGIZMO_PARENT_DIR}/libs)
set(VGIZMO_DIR ${VGIZMO_PARENT_DIR}/vGizmo3D)
set(ASSETS_DIR ${SRC}/../../assets)

include_directories(${TOOLS_DIR})
#include_directories(${VGIZMO_PARENT_DIR}) # CLion: when I add follow line COMMONS_DIR and VGIZMO_DIR are removed from project and relative contained files: "not belong to project" message
include_directories(${COMMONS_DIR})
include_directories(${VGIZMO_DIR})
include_directories(${ASSETS_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise

set(SOURCE_FILES
        ${SRC}/oglCube.h
        ${COMMONS_DIR}/oglDebug.cpp
        ${COMMONS_DIR}/oglDebug.h
        ${ASSETS_DIR}/shadersAndModel.h
        ${VGIZMO_DIR}/vGizmo3D.h
        ${VGIZMO_DIR}/vgMath.h
        ${VGIZMO_DIR}/vGizmo3D_config.h
        ${VGIZMO_DIR}/vgMath_config.h
)

set(SOURCE_FILES ${SOURCE_FILES}
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h
)

set(vkSHADERS_RELATIVE_DIR ../../Shaders/lightCube)
add_compile_definitions("-DAPP_SHADERS_DIR=${vkSHADERS_RELATIVE_DIR}")  # Shaders RELATIVE_DIR is acquired from CMake and passed to code:
include_directories(${vkSHADERS_RELATIVE_DIR}) # CLion trik: to "include" (show) shaders (outside project folder) into Project tool window: not necessary otherwise

#find_package(glm CONFIG)
#if(glm_FOUND OR IMGUIZMO_USES_GLM)
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVGIZMO_USES_GLM")
#endif()

find_package(OpenGL)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

file( GLOB APP_SOURCES ${SRC}/oglCube*.cpp )
foreach( oglCubeSourceFile ${APP_SOURCES} )
    get_filename_component( oglCubeName ${oglCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
        project(${oglCubeName})

    if(${oglCubeName} MATCHES "SDL")
        find_package(SDL2 CONFIG)
        include_directories(${SDL2_INCLUDE_DIRS})

        add_executable( ${PROJECT_NAME} ${oglCubeSourceFile} ${SOURCE_FILES})
        target_link_libraries(${oglCubeName} ${SDL2_LIBRARIES})
    else()
        find_package(glfw3 CONFIG)

        add_executable( ${PROJECT_NAME} ${oglCubeSourceFile} ${SOURCE_FILES})
        target_link_libraries(${oglCubeName} glfw)
    endif()

    target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARY})
endforeach( oglCubeSourceFile ${APP_SOURCES} )

