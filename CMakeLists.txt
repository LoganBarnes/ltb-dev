# ##############################################################################
# Copyright (c) 2021 Logan Barnes - All Rights Reserved
# ##############################################################################
cmake_minimum_required(VERSION 3.21)
project(LtbDev)

# Prevent cyclic dependencies even for static libs
set(GLOBAL_DEPENDS_NO_CYCLES ON)
# Prevent cmake from creating build artifacts in the source directory
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

#############################
### CMake Package Manager ###
#############################
set(CPM_DOWNLOAD_VERSION 0.35.0)

if (CPM_SOURCE_CACHE)
    set(CPM_DOWNLOAD_LOCATION "${CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
elseif (DEFINED ENV{CPM_SOURCE_CACHE})
    set(CPM_DOWNLOAD_LOCATION "$ENV{CPM_SOURCE_CACHE}/cpm/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
else ()
    set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")
endif ()

if (NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake to ${CPM_DOWNLOAD_LOCATION}")
    file(DOWNLOAD
         https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake
         ${CPM_DOWNLOAD_LOCATION}
         )
endif ()

include(${CPM_DOWNLOAD_LOCATION})

############################
### Development Settings ###
############################
add_library(ltb-dev-settings INTERFACE)
target_compile_options(ltb-dev-settings
                       INTERFACE
                       # Strict warnings/errors with gcc and clang
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wall>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wextra>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Werror>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wpedantic>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wunused>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-pedantic-errors>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Winit-self>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wold-style-cast>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Woverloaded-virtual>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wsign-conversion>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wshadow>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wmissing-declarations>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wmissing-include-dirs>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang>:-Wno-unknown-pragmas>
                       $<$<COMPILE_LANG_AND_ID:CXX,Clang>:-Wimplicit-float-conversion>
                       $<$<COMPILE_LANG_AND_ID:CXX,GNU>:-Wfloat-conversion>
                       # Strict warnings/errors with msvc
                       $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:W0>

                       # CUDA
                       $<$<COMPILE_LANGUAGE:CUDA>:--expt-extended-lambda>
                       $<$<COMPILE_LANGUAGE:CUDA>:--expt-relaxed-constexpr>

                       # Ignore any headers using angle brackets on windows
                       $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/experimental:external>
                       $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:anglebrackets>
                       $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/external:W0>
                       )

set(LTB_BUILD_ALL_APPS OFF)
set(LTB_ENABLE_TESTING ON)

include(CTest)

set(CPM_LtbCMake_SOURCE ${CMAKE_CURRENT_LIST_DIR}/ltb-cmake)
set(CPM_LtbUtil_SOURCE ${CMAKE_CURRENT_LIST_DIR}/ltb-util)
set(CPM_LtbGeom_SOURCE ${CMAKE_CURRENT_LIST_DIR}/ltb-geom)
set(CPM_LtbApp_SOURCE ${CMAKE_CURRENT_LIST_DIR}/ltb-app-tmp)

CPMAddPackage(NAME LtbUtil)
CPMAddPackage(NAME LtbGeom)
