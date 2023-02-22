//
// Created by -Ry on 05/02/2023.
//

#ifndef MAZEVISUALISATION_STANDARDCOMPONENTS_H
#define MAZEVISUALISATION_STANDARDCOMPONENTS_H

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace app::components {

    //############################################################################//
    // | UTILITY CACHED MATRIX |
    //############################################################################//

    class CachedMatrix {

    protected:
        bool      m_IsDirty      = true;
        glm::mat4 m_CachedMatrix = glm::mat4{ 1 };

    public:

        virtual bool is_dirty() const {
            return m_IsDirty;
        }

        virtual void set_dirty(const bool is_dirty = true) {
            m_IsDirty = is_dirty;
        }

        virtual const glm::mat4& get_matrix() {
            if (m_IsDirty) {
                recalc_matrix();
                m_IsDirty = false;
            }
            return m_CachedMatrix;
        }

    private:
        virtual void recalc_matrix() = 0;
    };

    //############################################################################//
    // | POSITIONAL COMPONENTS |
    //############################################################################//

    inline static constexpr glm::mat4 s_Identity = glm::mat4{ 1 };

    class Transform : public CachedMatrix {

    private:
        glm::vec3 m_Position = glm::vec3{ 0 };
        glm::vec3 m_Rotation = glm::vec3{ 0 };
        glm::vec3 m_Scale    = glm::vec3{ 1 };

    public:
        Transform(
                glm::vec3 pos = glm::vec3{ 0 },
                glm::vec3 rot = glm::vec3{ 0 },
                glm::vec3 scale = glm::vec3{ 1 }
        ) : m_Position(pos),
            m_Rotation(rot),
            m_Scale(scale) {

        }

    public:
        glm::vec3& get_pos() {
            return m_Position;
        }

        void set_pos(const glm::vec3& pos) {
            m_Position = pos;
            set_dirty();
        }

        glm::vec3& get_rotation() {
            return m_Rotation;
        }

        void set_rotation(const glm::vec3& rotate) {
            m_Rotation = rotate;
            set_dirty();
        }

        glm::vec3& get_scale() {
            return m_Scale;
        }

        void set_scale(const glm::vec3& scale) {
            m_Scale = scale;
            set_dirty();
        }

        virtual void recalc_matrix() {
            m_CachedMatrix = glm::translate(glm::mat4{ 1 }, m_Position)
                             * glm::rotate(glm::mat4{ 1 }, m_Rotation.x, glm::vec3{ 1, 0, 0 })
                             * glm::rotate(glm::mat4{ 1 }, m_Rotation.y, glm::vec3{ 0, 1, 0 })
                             * glm::rotate(glm::mat4{ 1 }, m_Rotation.z, glm::vec3{ 0, 0, 1 })
                             * glm::scale(glm::mat4{ 1 }, m_Scale);
        }
    };

    //############################################################################//
    // | COLLISION COMPONENTS |
    //############################################################################//

    struct BoxCollider {
        glm::vec3 bl;
        glm::vec3 tr;

        bool collides(const BoxCollider& o) {
            return bl.x >= o.bl.x
                   && bl.y >= o.bl.y
                   && bl.z >= o.bl.z
                   && tr.x <= o.tr.x
                   && tr.y <= o.tr.y
                   && tr.z <= o.tr.z;
        }
    };

    //############################################################################//
    // | MATERIAL SPECIFICS |
    //############################################################################//

    struct RenderAttributes {
        glm::vec3 colour;
    };

    struct Lighting {
        glm::vec3 ambient, diffuse, specular, emissive;
        float shininess;
    };

    //############################################################################//
    // | ABSTRACT COMPONENTS |
    //############################################################################//

    struct Initialisable {
        virtual void init(app::Application* app) = 0;
    };

    struct Updatable {
        virtual void update(app::Application* app) = 0;
    };

}

#endif //MAZEVISUALISATION_STANDARDCOMPONENTS_H
