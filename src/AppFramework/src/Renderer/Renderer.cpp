//
// Header File: Renderer.cpp
// Date       : 21/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "Renderer.h"

namespace app {

    //############################################################################//
    // | GL STATE CONTROL |
    //############################################################################//

    void Renderer::clear() {
        GL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
    }

    void Renderer::set_clear_colour(glm::vec4 color) {
        GL(glClearColor(color.r, color.g, color.b, color.a));
    }

    void Renderer::set_viewport(int x, int y, int w, int h) {
        GL(glViewport(x, y, w, h));
    }

    //############################################################################//
    // | DRAWING |
    //############################################################################//

    void Renderer::draw_buffer(
            DrawMode mode,
            GLsizei first,
            GLsizei count
    ) {
        GL(glDrawArrays(static_cast<GLenum>(mode), first, count));
    }

    void Renderer::draw_buffer_instanced(
            DrawMode mode,
            GLsizei first,
            GLsizei count,
            GLsizei instance_count
    ) {
        GL(glDrawArraysInstanced(
                static_cast<GLenum>(mode),
                first,
                count,
                instance_count
        ));
    }

    void Renderer::draw_elements(
            DrawMode mode,
            GLsizei count,
            PrimitiveType type,
            const GLvoid* indices
    ) {
        GL(glDrawElements(
                static_cast<GLenum>(mode),
                count,
                static_cast<GLenum>(type),
                indices
        ));
    }

    void Renderer::draw_elements_instanced(
            DrawMode mode,
            GLsizei count,
            GLsizei instance_count,
            PrimitiveType type,
            const GLvoid* indices
    ) {
        GL(glDrawElementsInstanced(
                static_cast<GLenum>(mode),
                count,
                static_cast<GLenum>(type),
                indices,
                instance_count
        ));
    }

} // app