//
// Created by -Ry on 12/03/2023.
//

#ifndef MAZEVISUALISATION_RENDERERHANDLERS_H
#define MAZEVISUALISATION_RENDERERHANDLERS_H

namespace app {

    //############################################################################//
    // | FORWARD DECLARE DEPENDENTS |
    //############################################################################//

    class Entity;
    class RenderGroup;
    class Vao;
    class Shader;

    //############################################################################//
    // | ABSTRACT RENDER GROUP HANDLER |
    //############################################################################//

    class GroupHandler {
    public:
        virtual ~GroupHandler() = default;
        virtual bool is_enabled(RenderGroup&) = 0;
        virtual bool update(RenderGroup&, Vao&, Shader&) = 0;
    };

    //############################################################################//
    // | ABSTRACT ENTITY HANDLER |
    //############################################################################//

    class EntityHandler {
    public:
        virtual ~EntityHandler() = default;
        virtual bool is_enabled(Entity&) = 0;
        virtual bool update(Entity&, RenderGroup&, float) = 0;
    };
}

#endif //MAZEVISUALISATION_RENDERERHANDLERS_H
