cmake_minimum_required(VERSION 3.23)
project(AppFramework VERSION 1.0.0 DESCRIPTION "")
set(CMAKE_CXX_STANDARD 20)

# Header Files & Source Files
set(PROJECT_NAME "AppFramework")
set(AppFramework_HEADER_FILES
        src/Logging.h
        src/Window.h
        src/Application.h
        src/Renderer/Renderer.h
        src/Renderer/GLUtil.h
        src/Renderer/Shader.h
        src/Camera3D.h
        src/Renderer/VertexObjectBinding.h
        src/CommonModelFileReaders.h
        src/Renderer/Entity.h)

set(AppFramework_SOURCE_FILES
        src/Window.cpp
        src/Application.cpp
        src/Renderer/Renderer.cpp)

# Project Executable/Library
add_library(AppFramework SHARED ${AppFramework_HEADER_FILES} ${AppFramework_SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC src)

################################################################################
# | LIBRARIES |
################################################################################

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glfw)

find_package(glm CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC glm::glm)

find_package(GLEW REQUIRED)
target_link_libraries(${PROJECT_NAME} PUBLIC GLEW::GLEW)

find_package(OpenGL REQUIRED)