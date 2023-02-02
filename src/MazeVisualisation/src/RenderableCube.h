//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLECUBE_H
#define MAZEVISUALISATION_RENDERABLECUBE_H

#include "Renderer/RenderableEntity.h"

namespace maze {

    class RenderableCube : public app::RenderableEntity {

    private:
        inline static app::IndexedVertexObject* s_CubeVertexObject = nullptr;
        inline static size_t s_InstanceCount = 0;

    private:
        float m_Theta = 0.0F;

    public:
        RenderableCube() : app::RenderableEntity() {
            ++s_InstanceCount;
            if (s_CubeVertexObject == nullptr) {
                s_CubeVertexObject = new app::IndexedVertexObject{};
                s_CubeVertexObject->init();
                s_CubeVertexObject->bind();

                {
                    float vertex_positions[24] = {
                            -1.0, 1.0, 1.0,
                            -1.0, -1.0, 1.0,
                            -1.0, 1.0, -1.0,
                            -1.0, -1.0, -1.0,
                            1.0, 1.0, 1.0,
                            1.0, -1.0, 1.0,
                            1.0, 1.0, -1.0,
                            1.0, -1.0, -1.0
                    };

                    app::FloatAttribPtr vertex_pos_attrib{ 0, 3 };
                    s_CubeVertexObject->add_vertex_buffer(vertex_positions, 24, vertex_pos_attrib);
                }

                {
                    unsigned int indices[36] = {
                            5 - 1, 3 - 1, 1 - 1,
                            3 - 1, 8 - 1, 4 - 1,
                            7 - 1, 6 - 1, 8 - 1,
                            2 - 1, 8 - 1, 6 - 1,
                            1 - 1, 4 - 1, 2 - 1,
                            5 - 1, 2 - 1, 6 - 1,
                            5 - 1, 7 - 1, 3 - 1,
                            3 - 1, 7 - 1, 8 - 1,
                            7 - 1, 5 - 1, 6 - 1,
                            2 - 1, 4 - 1, 8 - 1,
                            1 - 1, 3 - 1, 4 - 1,
                            5 - 1, 1 - 1, 2 - 1
                    };
                    s_CubeVertexObject->set_index_buffer(indices, 36);
                }

                {
                    float               vertex_colours[36]{
                            0.0, 1.0, 0.0,
                            0.0, 0.0, 1.0,
                            1.0, 0.0, 0.0,
                            0.0, 1.0, 0.0,
                            0.0, 0.0, 1.0,
                            0.0, 1.0, 0.0,
                            1.0, 0.0, 0.0,
                            0.0, 0.0, 1.0,
                            1.0, 0.0, 0.0,
                            0.0, 1.0, 0.0,
                            1.0, 0.0, 1.0,
                            0.0, 1.0, 0.0,
                    };
                    app::FloatAttribPtr color_attrib{ 1, 3 };
                    s_CubeVertexObject->add_vertex_buffer(vertex_colours, 36, color_attrib);
                }
            }
        }

        RenderableCube(const RenderableCube& c) {
            m_Transform = c.m_Transform;
            ++s_InstanceCount;
        }

        RenderableCube(RenderableCube&& c) {
            m_Transform = c.m_Transform;
            ++s_InstanceCount;
        }

        ~RenderableCube() {
            --s_InstanceCount;
            if (s_InstanceCount == 0) delete s_CubeVertexObject;
        }

    public:
        virtual void update(float ts) override {
            m_Theta += ts;
            m_Transform.rotate = {m_Theta, m_Theta, m_Theta};
        }

        virtual void render_singular(app::Renderer& renderer, app::Shader& shader) override {
            ASSERT(s_CubeVertexObject != nullptr, "Underlying Cube vertex buffer is null...");
            s_CubeVertexObject->bind();
            shader.set_uniform("u_ModelMatrix", get_model_matrix());
            renderer.draw_elements(app::DrawMode::TRIANGLES, 36);
            s_CubeVertexObject->unbind();
        }

    };
}

#endif //MAZEVISUALISATION_RENDERABLECUBE_H
