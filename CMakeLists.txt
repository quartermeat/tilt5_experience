cmake_minimum_required(VERSION 3.20)

project(TiltFiveFirstLight CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tilt Five NDK paths
set(T5_NDK_DIR "${CMAKE_SOURCE_DIR}/third_party/TiltFive")
set(T5_INCLUDE_DIR "${T5_NDK_DIR}/include")
set(T5_LIB_DIR "${T5_NDK_DIR}/lib/win/x86_64")

# Local dependencies
set(GLAD_DIR "${CMAKE_SOURCE_DIR}/glad/OpenGL_4_6_Core")
set(GLFW_DIR "${CMAKE_SOURCE_DIR}/glfw-3.3.8.bin.WIN64")
set(GLM_DIR "${CMAKE_SOURCE_DIR}/glm")
set(GLFW_LIB_DIR "${GLFW_DIR}/lib-vc2022")

function(require_dir path label)
    if (NOT EXISTS "${path}")
        message(FATAL_ERROR "${label} not found at ${path}. Run scripts/setup.ps1 or follow README.md.")
    endif()
endfunction()

require_dir("${T5_INCLUDE_DIR}" "Tilt Five NDK headers")
require_dir("${GLAD_DIR}" "GLAD (generated)")
require_dir("${GLFW_DIR}" "GLFW (prebuilt)")
require_dir("${GLFW_LIB_DIR}" "GLFW libs (vc2022)")
require_dir("${GLM_DIR}" "GLM (header-only)")
require_dir("${T5_LIB_DIR}" "Tilt Five NDK libs")

# Find OpenGL
find_package(OpenGL REQUIRED)

# Main executable (will be populated with HelloT5Cube source)
add_executable(TiltFiveApp
    src/main.cpp
    glad/OpenGL_4_6_Core/src/glad.c
    glwrapper/src/Framebuffer.cpp
    glwrapper/src/Shader.cpp
    glwrapper/src/VertexArray.cpp
    glapplication/src/Application.cpp
    glapplication/src/Transform.cpp
    t5wrapper/src/Context.cpp
    t5wrapper/src/Glasses.cpp
)

target_include_directories(TiltFiveApp PRIVATE
    ${T5_INCLUDE_DIR}
    ${GLAD_DIR}/include
    ${GLFW_DIR}/include
    ${GLM_DIR}
    ${CMAKE_SOURCE_DIR}/glwrapper
    ${CMAKE_SOURCE_DIR}/glapplication
    ${CMAKE_SOURCE_DIR}/t5wrapper
    ${CMAKE_SOURCE_DIR}/util
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(TiltFiveApp PRIVATE
    ${OPENGL_LIBRARIES}
    ${T5_LIB_DIR}/TiltFiveNative.dll.if.lib
    ${GLFW_LIB_DIR}/glfw3dll.lib
)

# Copy DLLs to output directory (Windows)
if(WIN32)
    target_compile_definitions(TiltFiveApp PRIVATE GLFW_DLL)
    add_custom_command(TARGET TiltFiveApp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${T5_LIB_DIR}/TiltFiveNative.dll"
            "$<TARGET_FILE_DIR:TiltFiveApp>/TiltFiveNative.dll"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${GLFW_LIB_DIR}/glfw3.dll"
            "$<TARGET_FILE_DIR:TiltFiveApp>/glfw3.dll"
    )
endif()
