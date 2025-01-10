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
project(oglCubeExamples)

find_package(glm CONFIG)

option(IMGUIZMO_USES_GLM "Use GLM instead of internal vgMath" OFF)
# glm_FOUND OR 
if(glm_FOUND OR IMGUIZMO_USES_GLM)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DVGIZMO_USES_GLM")
endif()

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL)

if(OPENGL_FOUND)
    message(STATUS "OPENGL_INCLUDE_DIRS: ${OPENGL_INCLUDE_DIRS}")
    message(STATUS "OPENGL_LIBRARY: ${OPENGL_LIBRARY}")

    include_directories(${OPENGL_INCLUDE_DIRS})
else ()
    message (FATAL_ERROR "OPENGL not found... REQUIRED!!!!")
endif(OPENGL_FOUND)

find_package(glfw3 CONFIG)
find_package(SDL2 CONFIG)
include_directories(${SDL2_INCLUDE_DIRS})


set(SRC          ${CMAKE_SOURCE_DIR})
set(COMMONS_DIR  ${SRC}/../commons)
set(TOOLS_DIR    ${CMAKE_SOURCE_DIR}/../../libs)
set(VGIZMO_PARENT_DIR ${CMAKE_SOURCE_DIR}/../..)
set(VGIZMO_DIR ${VGIZMO_PARENT_DIR}/vGizmo3D)

include_directories(${TOOLS_DIR})
include_directories(${VGIZMO_PARENT_DIR})
include_directories(${SRC}/../commons)

set(SOURCE_FILES
        ${SRC}/oglDebug.cpp
        ${SRC}/oglDebug.h
        ${COMMONS_DIR}/shadersAndModel.h
        ${TOOLS_DIR}/glad/glad.cpp
        ${TOOLS_DIR}/glad/glad.h)


file( GLOB APP_SOURCES ${SRC}/oglCube*.cpp )
foreach( oglCubeSourceFile ${APP_SOURCES} )
    get_filename_component( oglCubeName ${oglCubeSourceFile} NAME_WE )      # Cut off the file extension and directory path
    project(${oglCubeName})

    if(${oglCubeName} MATCHES "SDL")
        add_executable( ${PROJECT_NAME} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_SDL} ${COMMONS_UI_FILES})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    else()
        add_executable( ${PROJECT_NAME} ${oglCubeSourceFile} ${SOURCE_FILES} ${SOURCE_FILES_GLFW} ${COMMONS_UI_FILES})
        target_link_libraries(${PROJECT_NAME} glfw)
    endif()
    
    target_link_libraries(${oglCubeName} ${CMAKE_DL_LIBS} ${OPENGL_LIBRARY})
endforeach( oglCubeSourceFile ${APP_SOURCES} )

