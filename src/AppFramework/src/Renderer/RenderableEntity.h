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
        virtual glm::mat4 get_model_matrix() const {
            return glm::translate(glm::mat4{ 1 }, m_Transform.translate)
                   * glm::rotate(glm::mat4{ 1 }, m_Transform.rotate.x, { 1, 0, 0 })
                   * glm::rotate(glm::mat4{ 1 }, m_Transform.rotate.y, { 0, 1, 0 })
                   * glm::rotate(glm::mat4{ 1 }, m_Transform.rotate.z, { 0, 0, 1 })
                   * glm::scale(glm::mat4{ 1 }, m_Transform.scale);
        };

        virtual void update(float ts) = 0;
    };
}

#endif //MAZEVISUALISATION_RENDERABLEENTITY_H
