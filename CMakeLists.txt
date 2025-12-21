cmake_minimum_required(VERSION 3.28)
project(sfml3d LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# -----------------------------
# Fetch SFML
# -----------------------------
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.2
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# -----------------------------
# Find FFmpeg via pkg-config
# -----------------------------
find_package(PkgConfig REQUIRED)

pkg_check_modules(FFMPEG REQUIRED
    libavformat
    libavcodec
    libavutil
    libswscale
    libswresample
)

# -----------------------------
# Build executable
# -----------------------------
add_executable(sfml3d
    src/main.cpp
    src/voxel.cpp
    src/camera3d.cpp
)

target_compile_features(sfml3d PRIVATE cxx_std_17)

# SFML
target_link_libraries(sfml3d PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Network
    SFML::Audio
)

# FFmpeg include paths
target_include_directories(sfml3d PRIVATE ${FFMPEG_INCLUDE_DIRS})

# FFmpeg library paths
target_link_directories(sfml3d PRIVATE ${FFMPEG_LIBRARY_DIRS})

# FFmpeg libraries
target_link_libraries(sfml3d PRIVATE ${FFMPEG_LIBRARIES})

# Optional: suppress warnings from ffmpeg headers
target_compile_options(sfml3d PRIVATE ${FFMPEG_CFLAGS_OTHER})