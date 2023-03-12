cmake_minimum_required(VERSION 3.23)
project(MazeVisualisation VERSION 1.0.0 DESCRIPTION "")
set(CMAKE_CXX_STANDARD 20)

# Header Files & Source Files
set(PROJECT_NAME "MazeVisualisation")
set(MazeVisualisation_HEADER_FILES
        src/MazeManager.h
        src/MazeConstructs.h src/MazeWall.h)
set(MazeVisualisation_SOURCE_FILES
        "src/Main.cpp")

# Project Executable/Library
add_executable(MazeVisualisation ${MazeVisualisation_HEADER_FILES} ${MazeVisualisation_SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} PRIVATE AppFramework)

file(COPY Res DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})