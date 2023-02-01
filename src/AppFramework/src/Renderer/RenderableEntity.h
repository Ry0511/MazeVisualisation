//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLEENTITY_H
#define MAZEVISUALISATION_RENDERABLEENTITY_H

#include "Application.h"
#include "Renderer/Renderer.h"
#include "Renderer/Shader.h"
#include <glm/glm.hpp>

namespace app {

    struct Transform {
        glm::vec3 translate = glm::vec3{ 0 };
        glm::vec3 scale     = glm::vec3{ 1 };
        glm::vec3 rotate    = glm::vec3{ 0 };
    };

    class RenderableEntity {

    protected:
        Transform m_Transform;

    public:
        virtual ~RenderableEntity() = default;

    public:
        inline Transform& get_transform() {
            return m_Transform;
        }

    public:
        virtual void update(float ts) = 0;
        virtual void render_singular(app::Renderer& renderer, app::Shader& shader) = 0;
    };
}

#endif //MAZEVISUALISATION_RENDERABLEENTITY_H
