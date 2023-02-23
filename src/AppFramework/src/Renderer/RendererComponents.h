//
// Created by -Ry on 22/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERERCOMPONENTS_H
#define MAZEVISUALISATION_RENDERERCOMPONENTS_H

#include "Application.h"
#include "Renderer/VertexObjectBinding.h"

namespace app::components {

    struct RendererBase {
        virtual void render(app::Application* app) = 0;
    };

    struct AbstractShaderBase {
        virtual void begin_render(app::Application* app) = 0;
        virtual void end_render(app::Application* app) = 0;
    };

    class TriangleMeshRenderer : public RendererBase {

    public:
        // Position, Normal, Texture
        static constexpr size_t       s_VertexComponentCount = 3;
        static constexpr unsigned int s_MinValidSlot         = 3U;

    private:
        std::vector<glm::vec3>              m_VertexDataMesh;
        size_t                              m_FirstIndex;
        size_t                              m_VertexCount;
        size_t                              m_InstanceCount;
        Vao                                 m_Vao;
        std::shared_ptr<AbstractShaderBase> m_Shader;

    public:
        TriangleMeshRenderer(
                std::vector<glm::vec3>&& vertex_data_mesh,
                std::shared_ptr<AbstractShaderBase> shader_base,
                size_t first_index = 0,
                size_t instance_count = 1
        ) : m_VertexDataMesh(std::move(vertex_data_mesh)),
            m_Shader(shader_base),
            m_FirstIndex(first_index),
            m_VertexCount(m_VertexDataMesh.size() / s_VertexComponentCount),
            m_InstanceCount(0),
            m_Vao({}) {
            HINFO("[CREATE]", " # Triangle Mesh Renderer...");

            m_Vao.init();
            m_Vao.bind_all();

            // Vertex Buffer (0: Position, 1: Normal, 2: Texture)
            m_Vao.add_buffer<FloatAttribLayout333>(
                    init_array_buffer<glm::vec3, BufferAllocUsage::STATIC_DRAW>(
                            m_VertexDataMesh.data(),
                            m_VertexDataMesh.size()
                    ),
                    0U, 0
            );
            m_Vao.unbind();
        }

    public:
        void set_first_index(size_t index) {
            m_FirstIndex = index;
        }

        void set_instance_count(size_t count) {
            m_InstanceCount = count;
        }

        template<class Layout, unsigned int Slot, class... Args>
        void add_buffer(SimpleBuffer buffer, Args&& ... args) {
            static_assert(Slot >= s_MinValidSlot, "The provided slot is invalid.");
            m_Vao.bind();
            m_Vao.add_buffer<Layout>(buffer, Slot, args...);
            m_Vao.unbind();
        }

        SimpleBuffer& get_buffer(unsigned int slot) {
            return m_Vao.get_buffer(slot).first;
        }

    public:
        virtual void render(app::Application* app) {
            m_Vao.bind_all();
            m_Shader->begin_render(app);
            app->draw_buffer_instanced(
                    DrawMode::TRIANGLES,
                    m_FirstIndex,
                    m_VertexCount,
                    m_InstanceCount
            );
            m_Shader->end_render(app);
            m_Vao.unbind();
        }
    };

}

#endif //MAZEVISUALISATION_RENDERERCOMPONENTS_H
