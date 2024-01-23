cmake_minimum_required(VERSION 3.17)

project(mcstructure VERSION 0.0.1.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

# ----------------------------------
# Build Options
# ----------------------------------
option(MCSTRUCTURE_BUILD_STATIC "Build static libraries" OFF)
option(MCSTRUCTURE_BUILD_TESTS "Build test cases" ON)
option(MCSTRUCTURE_INSTALL "Install library" ON)

# ----------------------------------
# CMake Settings
# ----------------------------------
if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /manifest:no")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /manifest:no")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")

    if(NOT DEFINED CMAKE_DEBUG_POSTFIX)
        set(CMAKE_DEBUG_POSTFIX "d")
    endif()
endif()

if(MCSTRUCTURE_INSTALL)
    include(GNUInstallDirs)
    include(CMakePackageConfigHelpers)
endif()

# ----------------------------------
# Project Variables
# ----------------------------------
set(MCSTRUCTURE_VERSION ${PROJECT_VERSION})
set(MCSTRUCTURE_NAME ${PROJECT_NAME})
set(MCSTRUCTURE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# ----------------------------------
# Main Project
# ----------------------------------
if(MCSTRUCTURE_BUILD_STATIC)
    set(_type STATIC)
else()
    set(_type SHARED)
endif()

set(_src
        src/BlockState.cpp
        src/Structure.cpp)

add_library(${MCSTRUCTURE_NAME} ${_src})
target_link_libraries(${MCSTRUCTURE_NAME} PUBLIC nbt++)
target_include_directories(${MCSTRUCTURE_NAME} PUBLIC include ${CMAKE_CURRENT_BINARY_DIR})

# ----------------------------------
# Libraries
# ----------------------------------
add_subdirectory(lib)

# ----------------------------------
# Build Tests
# ----------------------------------
if(MCSTRUCTURE_BUILD_TESTS)
    add_subdirectory(tests)
endif()