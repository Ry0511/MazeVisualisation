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
        glm::vec3 pos, rotate, scale;
    };

    struct RenderAttributes {
        glm::vec3 colour;
    };

    class Entity {

    public:
        using Functor = std::function<bool(Entity&, RenderGroup&)>;
        using FunctorIter = std::deque<Functor>::iterator;

    private:
        Transform           m_Transform;
        RenderAttributes    m_RenderAttributes;
        std::deque<Functor> m_Handles;
        std::stack<Functor> m_HandlesToAdd;
        mutable bool        m_IsDirty;
        mutable glm::mat4   m_Matrix;

    public:
        Entity(

        ) : m_Transform(),
            m_RenderAttributes(),
            m_Handles(),
            m_HandlesToAdd(),
            m_IsDirty(true),
            m_Matrix(glm::mat4{ 1 }) {
        }

    public:

        void update(RenderGroup& group) {

            // Add any pending functors
            while (!m_HandlesToAdd.empty()) {
                m_Handles.push_front(m_HandlesToAdd.top());
                m_HandlesToAdd.pop();
            }

            // Update functors (also erases functors that return false on update)
            for (FunctorIter i = m_Handles.begin(); i != m_Handles.end(); ++i) {
                if (!((*i)(*this, group))) m_Handles.erase(i);
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
                m_Matrix = glm::translate(glm::mat4{ 1 }, m_Transform.pos);
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
