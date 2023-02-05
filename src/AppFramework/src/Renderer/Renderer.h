//
// Header File: Renderer.h
// Date       : 21/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_RENDERER_H
#define MAZEVISUALISATION_RENDERER_H

#include "Logging.h"
#include "Renderer/VertexObjectBinding.h"

#include <glm/glm.hpp>
#include <array>

namespace app {

    enum class DrawMode : GLenum {
        POINTS    = GL_POINTS,
        LINES     = GL_LINES,
        TRIANGLES = GL_TRIANGLES,
        POLYGONS  = GL_POLYGON
    };

    class Renderer {

    private:
        glm::mat4 m_ProjectionMatrix{ 1 };

    public:
        explicit Renderer() = default;
        ~Renderer() = default;

    public:
        void clear();
        void set_clear_colour(glm::vec4 color);
        void set_viewport(int x, int y, int w, int h);

    public:
        void draw_buffer(DrawMode mode, GLsizei first, GLsizei count);

        void draw_elements(
                DrawMode mode,
                GLsizei count,
                PrimitiveType type = PrimitiveType::UINT,
                const GLvoid* indices = (const GLvoid*) 0
        );

        void draw_elements_instanced(
                DrawMode mode,
                GLsizei count,
                GLsizei instance_count,
                PrimitiveType type = PrimitiveType::UINT,
                const GLvoid* = (const GLvoid*) 0
        );
    };

} // app

#endif
