# ##############################################################################
# Copyright (c) 2021 Logan Barnes - All Rights Reserved
# ##############################################################################
cmake_minimum_required(VERSION 3.21)
project(LtbDev)

set(ltb_sub_projects "")

macro(add_subdirectory)
    _add_subdirectory(${ARGV})
    message(STATUS: "Add project ${PROJECT_NAME}")
    list(APPEND ltb_sub_projects ${PROJECT_NAME})
endmacro()

macro(find_package)
    if (NOT "${ARG0}" IN_LIST ltb_sub_projects)
        _find_package(${ARGV})
    endif ()
endmacro()

#add_subdirectory(ltb-util)
