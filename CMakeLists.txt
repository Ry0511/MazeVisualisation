cmake_minimum_required(VERSION 3.23)
project(MazeVisualisation VERSION 1.0.0 DESCRIPTION "")
set(CMAKE_CXX_STANDARD 20)

# Output Directories & Windows Symbols
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if (${BUILD_MODE} MATCHES Debug)
    add_definitions(-DLogging=1)
endif ()

# CTest
enable_testing()

# Projects
add_subdirectory(src/AppFramework)
add_subdirectory(src/MazeVisualisation)