##########################################################################################
# Copyright (c) 2022 Logan Barnes - All Rights Reserved
##########################################################################################
cmake_minimum_required(VERSION 3.21)
project(LtbApp VERSION 1.0.0)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/LtbCreateApp.cmake)

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

#########################
### External Packages ###
#########################
find_package(Vulkan REQUIRED)

CPMAddPackage(
        NAME LtbGeom
        GITHUB_REPOSITORY LoganBarnes/ltb-geom
)
CPMAddPackage(
        NAME range-v3
        GITHUB_REPOSITORY ericniebler/range-v3
        GIT_TAG 0.11.0
        OPTIONS "RANGES_CXX_STD 17"
)
CPMAddPackage(
        NAME GLFW
        GITHUB_REPOSITORY glfw/glfw
        GIT_TAG 3.3.6
        OPTIONS
        "GLFW_BUILD_TESTS OFF"
        "GLFW_BUILD_EXAMPLES OFF"
        "GLFW_BULID_DOCS OFF"
)
CPMAddPackage("gh:ocornut/imgui@1.87")
CPMAddPackage("gh:gabime/spdlog@1.9.2")
CPMAddPackage("gh:TheLartians/PackageProject.cmake@1.6.0")

if (range-v3_ADDED)
    target_include_directories(range-v3
                               SYSTEM INTERFACE
                               $<BUILD_INTERFACE:${range-v3_SOURCE_DIR}/include>
                               )
endif ()

if (GLFW_ADDED)
    add_library(
            glfw::glfw
            ALIAS
            glfw
    )
endif ()

if (imgui_ADDED)
    add_library(
            imgui
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
    add_library(
            ImGui::imgui
            ALIAS
            imgui
    )
    target_link_libraries(
            imgui
            PUBLIC
            Vulkan::Vulkan
            glfw::glfw
    )
    target_include_directories(
            imgui
            SYSTEM PUBLIC
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}>
            $<BUILD_INTERFACE:${imgui_SOURCE_DIR}/backends>
    )
endif ()

##########################
#### LtbApp::ltb-app ###
##########################
ltb_add_library(
        ltb-app
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/app/application_data.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/app/fwd.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/ltb/app/glfw_vulkan_imgui_app.cpp
)
add_library(
        LtbApp::ltb-app
        ALIAS
        ltb-app
)
ltb_target_link_libraries(
        ltb-app
        PUBLIC
        LtbGeom::ltb-geom
        ImGui::imgui
        spdlog::spdlog
        range-v3::range-v3
)
ltb_target_include_directories(
        ltb-app
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/src>
)
ltb_target_compile_definitions(
        ltb-app
        PUBLIC
        VULKAN_HPP_NO_EXCEPTIONS
)
ltb_target_compile_features(
        ltb-app
        PRIVATE
        cxx_std_17
)
set_target_properties(
        ltb-app
        PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON
)