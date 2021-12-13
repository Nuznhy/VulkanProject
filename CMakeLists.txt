cmake_minimum_required(VERSION 3.20 FATAL_ERROR)
cmake_policy(VERSION 3.20)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 20)
set(NAME VulkanTestProject)

project(${NAME})

include_directories("engine")
include_directories("src/shaders")
include_directories("shaders")

set(RESOURCE_INSTALL_DIR "" CACHE PATH "Path to install resources to (leave empty for running uninstalled)")

message(STATUS "Using module to find Vulkan")
find_package(Vulkan REQUIRED COMPONENTS glslc)
message(STATUS "Using module to find GLFW")
find_package(glfw3 CONFIG REQUIRED)
message(STATUS "Using module to find glm")
find_package(glm CONFIG REQUIRED)
message(STATUS "Using module to find freetype")
find_package(freetype CONFIG REQUIRED)
message(STATUS "Using module to find imgui")
find_package(imgui CONFIG REQUIRED)

find_path(STB_INCLUDE_DIRS "stb.h")

function(add_shader TARGET SHADER)
    find_program(glslc_executable NAMES glslc HINTS Vulkan::glslc)

    set(current-shader-path ${CMAKE_SOURCE_DIR}/src/shaders/${SHADER})
    set(current-output-path ${CMAKE_SOURCE_DIR}/shaders/${SHADER}.spv)

    # Add a custom command to compile GLSL to SPIR-V.
    get_filename_component(current-output-dir ${current-output-path} DIRECTORY)
    file(MAKE_DIRECTORY ${current-output-dir})

    add_custom_command(
            OUTPUT ${current-output-path}
            COMMAND ${glslc_executable} -o ${current-output-path} ${current-shader-path}
            DEPENDS ${current-shader-path}
            IMPLICIT_DEPENDS CXX ${current-shader-path}
            VERBATIM)

    # Make sure our build depends on this output.
    set_source_files_properties(${current-output-path} PROPERTIES GENERATED TRUE)
    target_sources(${TARGET} PRIVATE ${current-output-path})
endfunction(add_shader)


set(HEADER_FILES
        engine/lvk_window.hpp
        engine/app.hpp
        engine/lvk_pipeline.hpp
        engine/lvk_device.hpp
        engine/lvk_swap_chain.hpp
        engine/lvk_model.hpp
        engine/lvk_game_object.hpp)
set(CPP_FILES
        engine/lvk_window.cpp
        engine/app.cpp
        engine/lvk_pipeline.cpp
        engine/lvk_device.cpp
        engine/lvk_swap_chain.cpp
        engine/lvk_model.cpp)

add_executable(newexec main.cpp ${CPP_FILES} ${HEADER_FILES})
add_shader(newexec shader.frag)
add_shader(newexec shader.vert)
# COMPILE SHADERS
#


#add_shader(newexec shader.frag)
#add_shader(newexec shader.vert)

target_include_directories(newexec
        PRIVATE
        ${STB_INCLUDE_DIRS}
        "${CMAKE_CURRENT_LIST_DIR}/external"
        )
target_link_libraries(
        newexec
        PRIVATE
        freetype
        glfw
        glm::glm
        imgui::imgui
)
