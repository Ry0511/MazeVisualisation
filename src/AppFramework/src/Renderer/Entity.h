//
// Created by -Ry on 10/03/2023.
//

#ifndef MAZEVISUALISATION_ENTITY_H
#define MAZEVISUALISATION_ENTITY_H

#include "Renderer/VertexObjectBinding.h"
#include "Renderer/Shader.h"

#include <memory>
#include <functional>

#include <glm/glm.hpp>
#include <stack>
#include <glm/ext/matrix_transform.hpp>

namespace app {

    class RenderGroup;

    struct Transform {
        glm::vec3 pos{ 0, 0, 0 }, rotate{ 0, 0, 0 }, scale{ 1, 1, 1 };
    };

    struct RenderAttributes {
        glm::vec3 colour;
    };

    class Entity {

    public:
        using Functor = std::function<bool(Entity&, RenderGroup&)>;

    private:
        Transform            m_Transform;
        RenderAttributes     m_RenderAttributes;
        std::vector<Functor> m_Handles;
        std::stack<Functor>  m_HandlesToAdd;
        mutable bool         m_IsDirty;
        mutable glm::mat4    m_Matrix;

    public:
        Entity(

        ) : m_Transform(),
            m_RenderAttributes(),
            m_Handles(),
            m_HandlesToAdd(),
            m_IsDirty(true),
            m_Matrix(glm::mat4{ 1 }) {
        }

        Entity(const Entity& o) : m_Transform(o.m_Transform),
                                  m_RenderAttributes(o.m_RenderAttributes),
                                  m_Handles(o.m_Handles),
                                  m_HandlesToAdd(o.m_HandlesToAdd),
                                  m_IsDirty(o.m_IsDirty),
                                  m_Matrix(o.m_Matrix) {
            //HINFO("[ENTITY]", " # Copied...");
        }

        Entity(Entity&& o) : m_Transform(o.m_Transform),
                             m_RenderAttributes(o.m_RenderAttributes),
                             m_Handles(std::move(o.m_Handles)),
                             m_HandlesToAdd(std::move(o.m_HandlesToAdd)),
                             m_IsDirty(o.m_IsDirty),
                             m_Matrix(std::move(o.m_Matrix)) {
            //HINFO("[ENTITY]", " # Moved...");
        }

        Entity& operator =(const Entity& o) {
            m_Transform        = (o.m_Transform);
            m_RenderAttributes = (o.m_RenderAttributes);
            m_Handles          = (o.m_Handles);
            m_HandlesToAdd     = (o.m_HandlesToAdd);
            m_IsDirty          = (o.m_IsDirty);
            m_Matrix           = (o.m_Matrix);
            //HINFO("[ENTITY]", " # Copy Assigned...");
            return *this;
        }

        Entity& operator =(Entity&& o) {
            m_Transform        = o.m_Transform;
            m_RenderAttributes = o.m_RenderAttributes;
            m_Handles          = std::move(o.m_Handles);
            m_HandlesToAdd     = std::move(o.m_HandlesToAdd);
            m_IsDirty          = o.m_IsDirty;
            m_Matrix           = std::move(o.m_Matrix);
            //HINFO("[ENTITY]", " # Move Assigned...");
            return *this;
        }

    public:

        void update(RenderGroup& group) {

            // Add any pending functors
            while (!m_HandlesToAdd.empty()) {
                m_Handles.push_back(m_HandlesToAdd.top());
                m_HandlesToAdd.pop();
            }

            // Update functors (also erases functors that return false on update)
            for (auto i = m_Handles.begin(); i != m_Handles.end(); ++i) {
                auto fn = *i;
                if (!fn(*this, group)) {
                    i = m_Handles.erase(i);

                    [[unlikely]]
                    if (i == m_Handles.end()) break;
                }
            }
        }

    public:

        Transform& get_transform() {
            return m_Transform;
        }

        RenderAttributes& get_render_attributes() {
            return m_RenderAttributes;
        }

        bool is_dirty() const {
            return m_IsDirty;
        }

        const glm::mat4& get_matrix() const {
            if (m_IsDirty) {
                set_dirty(false);
                const auto& t = m_Transform;
                m_Matrix = glm::translate(glm::mat4{ 1 }, t.pos)
                           * glm::rotate(glm::mat4{ 1 }, t.rotate.x, glm::vec3{ 1, 0, 0 })
                           * glm::rotate(glm::mat4{ 1 }, t.rotate.y, glm::vec3{ 0, 1, 0 })
                           * glm::rotate(glm::mat4{ 1 }, t.rotate.z, glm::vec3{ 0, 0, 1 })
                           * glm::scale(glm::mat4{ 1 }, t.scale);
            }

            return m_Matrix;
        }

        void add_functor(Functor fn) {
            m_HandlesToAdd.push(fn);
        }

        void set_dirty(bool is_dirty = true) const {
            m_IsDirty = is_dirty;
        }
    };
}

#endif //MAZEVISUALISATION_ENTITY_H
