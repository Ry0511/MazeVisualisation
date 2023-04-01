# App Framework

The application framework is the backend abstractions to OpenGL, GLFW, and stb_image

# Development

Development was done using CLion and CMake complied and tested using MSVC C++20

# Dependencies

The framework relies on OpenGL, GLFW3, GLM, and stb_image

## Library Usages

|  Library  | Usage                                                            |
|:---------:|:-----------------------------------------------------------------|
|  OpenGL   | Used as a renderering context                                    |
|   GLFW3   | Used to create and mutate Windows                                |
|   GLEW    | Used to determine OpenGL versions and features                   |
|    GLM    | Used for its Vector and Matrix types and its Mathematic function |
| stb_image | Used to load Images from disk                                    |