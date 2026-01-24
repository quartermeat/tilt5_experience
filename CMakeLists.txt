cmake_minimum_required(VERSION 3.20)

project(TiltFiveFirstLight CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Tilt Five NDK paths
set(T5_NDK_DIR "${CMAKE_SOURCE_DIR}/third_party/tiltfive_ndk")
set(T5_INCLUDE_DIR "${T5_NDK_DIR}/include")
set(T5_LIB_DIR "${T5_NDK_DIR}/lib")

# Find OpenGL
find_package(OpenGL REQUIRED)

# Main executable (will be populated with HelloT5Cube source)
add_executable(TiltFiveApp
    # src/main.cpp will be added here
)

target_include_directories(TiltFiveApp PRIVATE
    ${T5_INCLUDE_DIR}
    ${OPENGL_INCLUDE_DIR}
)

target_link_libraries(TiltFiveApp PRIVATE
    ${OPENGL_LIBRARIES}
    # ${T5_LIB_DIR}/TiltFiveNative.lib will be added here
)

# Copy DLLs to output directory (Windows)
if(WIN32)
    # add_custom_command will be added to copy TiltFiveNative.dll
endif()
