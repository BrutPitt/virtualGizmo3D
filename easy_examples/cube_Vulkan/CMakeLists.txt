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
cmake_minimum_required(VERSION 3.16)
project(vGizmo3D_vkCube)

# To use SDL2 | SDL3 as backend framework (instead of GLFW) type:
#       cmake -DUSE_SDL2=ON
#   or
#       cmake -DUSE_SDL3=ON
# obviously is necessary to have SDL2/SDL3 (+devel package) installed
option(USE_SDL2 "use SDL2 instead of GLFW (default) framework" OFF)
option(USE_SDL3 "use SDL3 instead of GLFW (default) framework" OFF)

# use negative Z in clipMatrix ==> view at the end of vkCube.h
# also settable via/in code    ==> view at the end of vkCube.h
option(APP_HAS_NEG_Z "use negative Z in clipMatrix ==> view at the end of vkCube.h" OFF)
if(APP_HAS_NEG_Z)
    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_HAS_NEG_Z")
endif()

# Debug Validation Layer is enabled in ONLY Debug mode
#       cmake -DFORCE_VALIDATION_LAYER=ON
# force dbgVL anyway also in Release build
option(FORCE_VALIDATION_LAYER "force Debug ValidationLayer also in Release build" OFF)

set(CMAKE_INCLUDE_DIRECTORIES_BEFORE, ON)

set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Release")
  message(STATUS "CMAKE_BUILD_TYPE not specified: use Release by default...")
endif(NOT CMAKE_BUILD_TYPE)

set(SRC          ${CMAKE_SOURCE_DIR})
set(COMMONS_DIR  ${SRC}/../commons)
set(TOOLS_DIR    ${SRC}/../../libs)
set(VGIZMO_PARENT_DIR ${SRC}/../..)
set(VGIZMO_DIR ${VGIZMO_PARENT_DIR}/vGizmo3D)

include_directories(${TOOLS_DIR})
#include_directories(${VGIZMO_PARENT_DIR}) # CLion: when I add follow line COMMONS_DIR and VGIZMO_DIR are removed from project and relative contained files: "not belong to project" message
include_directories(${COMMONS_DIR})
include_directories(${VGIZMO_DIR})

set(SRC "${CMAKE_SOURCE_DIR}")

    set(SOURCE_FILES
        ${SRC}/vkCube.cpp
        ${SRC}/vkCube.h
        ${SRC}/framework.h
        ${SRC}/framework.cpp
        ${SRC}/dbgValidationLayer.h
        ${VGIZMO_DIR}/vGizmo3D.h
        ${VGIZMO_DIR}/vGizmo3D_config.h
        ${VGIZMO_DIR}/vgMath.h
        ${VGIZMO_DIR}/vgMath_config.h
        ${COMMONS_DIR}/shadersAndModel.h
    )

    set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLFW_INCLUDE_NONE -DGLAPP_NO_OGL_DSA")
    if(USE_SDL2)
        find_package(SDL2 REQUIRED)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL2")
        add_executable(${PROJECT_NAME} ${SOURCE_FILES})
        include_directories(${SDL2_INCLUDE_DIRS})
        target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
    elseif(USE_SDL3)
            find_package(SDL3 REQUIRED)
            set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DAPP_USES_SDL3")
            add_executable(${PROJECT_NAME} ${SOURCE_FILES})
            include_directories(${SDL3_INCLUDE_DIRS})
            target_link_libraries(${PROJECT_NAME} ${SDL3_LIBRARIES})
        else()
            find_package(glfw3 REQUIRED)
            add_executable(${PROJECT_NAME} ${SOURCE_FILES})
            target_link_libraries(${PROJECT_NAME} glfw)
    endif(USE_SDL2)

    if(${CMAKE_BUILD_TYPE} MATCHES "Debug" OR FORCE_VALIDATION_LAYER)
        set(M_GLOBAL_FLAGS "${M_GLOBAL_FLAGS} -DENABLE_VALIDATION_LAYER")
    endif()


target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} ${MORE_LIBS} ${TARGET_LIBS})

find_package(Vulkan REQUIRED)
message(STATUS "Vulkan found in: ${SHADERC_LIB}")

# it's necessary only to compile shaders from code
find_library(SHADERC_LIB shaderc_combined $ENV{VULKAN_SDK}/lib)
message(STATUS "Shaderc found in: ${SHADERC_LIB}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${M_GLOBAL_FLAGS}")
#target_compile_options(${PROJECT_NAME} PRIVATE -Wno-deprecated-declarations -fpermissive)
#if(EXISTS $ENV{RAMDISK}) #my RAMDISK env
#    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $ENV{RAMDISK}/${PROJECT_NAME})
#else()
#    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
#endif()

if(VULKAN_FOUND)
    target_include_directories(${PROJECT_NAME} PUBLIC $ENV{VULKAN_SDK}/include)
    target_link_libraries (${PROJECT_NAME} ${Vulkan_LIBRARIES} ${CMAKE_DL_LIBS}  ${SHADERC_LIB})
endif (VULKAN_FOUND)

#file(WRITE ${CMAKE_SOURCE_DIR}/.idea/.name ${PROJECT_NAME}) # used to rename a Project in clion (run once)
