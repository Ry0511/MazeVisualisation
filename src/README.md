# Project

This is my Third Year dissertation project for Swansea University where I set out to learn C++,
OpenGL, and various Maze Generation algorithms.

# Building

The project was built using CMake, MSVC, and C++20 features from C++20 such as `std::format`. You
can build yourself though note that the following libraries must be linked:

1. GLFW3
2. OpenGL
3. GLM
4. GLEW

Alternatively, you can download the pre-built binaries for Windows in Releases.

# Program Structure

The project is broken into two folders/subprojects being AppFramework and MazeVisualisation. The
AppFramework or Application Framework is the backend OpenGL and GLFW3 abstractions to provide a
simple yet intuitive set of tools to show things on the screen. The entry point for an application
is as follows:

```cpp
#include "Application.h"

class App : public Application {
    
public:
    virtual void on_create() override {
        // Called once when created
    }
    
    virtual bool on_update(float delta) override {
        // Called every frame of the application
        return false; // If true then continue updating else stop
    }
};
```

The MazeVisualisation executable builds upon the above to create the actual application.

# Notes

This was my first large project in C++ and also my first project using OpenGL. I think this project
turned out quite well despite the code-base in my eyes being a disaster. But I
think if you can't reflect then have not learned anything.