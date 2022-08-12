# ##############################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
# ##############################################################################
cmake_minimum_required(VERSION 3.21)
project(LtbApp VERSION 1.0.0)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/LtbCreateApp.cmake)

# ##############################################################################
# CMake Package Manager
# ##############################################################################
include(cmake/CPM.cmake)

# ##############################################################################
# External Packages
# ##############################################################################
find_package(Vulkan REQUIRED)

cpmaddpackage(NAME
              LtbGeom
              GITHUB_REPOSITORY
              LoganBarnes/ltb-geom
              GIT_TAG
              main
              )
cpmaddpackage(NAME
              range-v3
              GITHUB_REPOSITORY
              ericniebler/range-v3
              GIT_TAG
              0.11.0
              OPTIONS
              "RANGES_CXX_STD 17"
              # It's header only and the CMakeLists.txt file adds global flags
              # that break CUDA on windows
              DOWNLOAD_ONLY
              TRUE
              )
cpmaddpackage(NAME
              GLFW
              GITHUB_REPOSITORY
              glfw/glfw
              GIT_TAG
              3.3.8
              OPTIONS
              "GLFW_BUILD_TESTS OFF"
              "GLFW_BUILD_EXAMPLES OFF"
              "GLFW_BULID_DOCS OFF"
              )
cpmaddpackage("gh:ocornut/imgui@1.87")
cpmaddpackage("gh:gabime/spdlog@1.9.2")

if(range-v3_ADDED)
    add_library(range-v3 INTERFACE)
    add_library(range-v3::range-v3 ALIAS range-v3)

    # Ignore errors from this third party library by marking the includes as
    # system headers.
    target_include_directories(range-v3 SYSTEM
                               INTERFACE
                                   $<BUILD_INTERFACE:${range-v3_SOURCE_DIR}/include>
                               )
    target_compile_options(range-v3
                           INTERFACE
                               $<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->
                               $<$<COMPILE_LANG_AND_ID:CUDA,MSVC>:-Xcompiler=/permissive->
                           )
endif()

if(GLFW_ADDED)
    add_library(glfw::glfw ALIAS glfw)
    target_compile_options(glfw
                           PRIVATE # Ignore build warning on this third-party code
                               $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-w>
                           )
endif()

if(imgui_ADDED)
    add_library(imgui
                ${imgui_SOURCE_DIR}/imconfig.h
                ${imgui_SOURCE_DIR}/imgui.cpp
                ${imgui_SOURCE_DIR}/imgui.h
                ${imgui_SOURCE_DIR}/imgui_demo.cpp
                ${imgui_SOURCE_DIR}/imgui_draw.cpp
                ${imgui_SOURCE_DIR}/imgui_internal.h
                ${imgui_SOURCE_DIR}/imgui_tables.cpp
                ${imgui_SOURCE_DIR}/imgui_widgets.cpp
                ${imgui_SOURCE_DIR}/imstb_rectpack.h
                ${imgui_SOURCE_DIR}/imstb_textedit.h
                ${imgui_SOURCE_DIR}/imstb_truetype.h
                ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.h
                ${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
                ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.h
                ${imgui_SOURCE_DIR}/backends/imgui_impl_vulkan.cpp
                )
    add_library(imgui::imgui ALIAS imgui)
    target_link_libraries(imgui PUBLIC Vulkan::Vulkan glfw::glfw)
    target_include_directories(imgui SYSTEM
                               PUBLIC
                                   $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
                                   $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
                               )
    target_compile_options(imgui
                           PRIVATE
                               # Ignore build warning on this third-party code
                               $<$<COMPILE_LANG_AND_ID:CXX,GNU,Clang,AppleClang>:-w>
                           )
    target_compile_features(imgui PUBLIC cxx_std_11)
endif()

# ##############################################################################
# LtbApp::LtbApp
# ##############################################################################
ltb_create_default_targets(LtbApp
                           src/application_data.cpp
                           src/fwd.cpp
                           src/glfw_vulkan_imgui_app.cpp
                           )

# Public
target_link_libraries(LtbApp_deps
                      INTERFACE LtbGeom::LtbGeom
                                imgui::imgui
                                spdlog::spdlog
                                range-v3::range-v3
                      )
target_include_directories(LtbApp_deps
                           INTERFACE
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
                           )
target_compile_definitions(LtbApp_deps INTERFACE VULKAN_HPP_NO_EXCEPTIONS)

# Private
target_link_libraries(LtbApp_objs PUBLIC doctest::doctest)
target_include_directories(LtbApp_objs
                           PRIVATE
                               $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
                           )

# Testing
if(TARGET test_LtbApp)
    target_link_libraries(test_LtbApp PRIVATE doctest_with_main)
endif()
