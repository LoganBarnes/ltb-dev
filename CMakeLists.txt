# ##############################################################################
# Copyright (c) 2021 Logan Barnes - All Rights Reserved
# ##############################################################################
cmake_minimum_required(VERSION 3.21)
project(LtbDev)

add_library(ltb-global-settings INTERFACE)
target_compile_options(ltb-global-settings
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

macro(add_library)
    _add_library(${ARGV})
    if (NOT "${ARGV1}" STREQUAL "ALIAS")
        target_link_libraries(${ARGV0} PRIVATE ${ltb-global-settings})
    endif ()
endmacro()

enable_testing()

add_subdirectory(ltb-util LtbUtil)
