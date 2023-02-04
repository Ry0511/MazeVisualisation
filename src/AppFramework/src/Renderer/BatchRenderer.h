//
// Created by -Ry on 02/02/2023.
//

#ifndef MAZEVISUALISATION_BATCHRENDERER_H
#define MAZEVISUALISATION_BATCHRENDERER_H

#include "Renderer/VertexArrayObject.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace app {

    class BatchRenderer {

    public:
        inline static constexpr GLuint s_VerticesAttribIndex    = 0;
        inline static constexpr GLuint s_ModelMatrixAttribIndex = 1;

    private:
        IndexedVertexObject m_Ivo{};

    public:
        BatchRenderer() = default;

    public:
        void init() {
            m_Ivo.init();
        }

        bool is_initialised() const {
            return m_Ivo.is_initialised();
        };

        void bind_all() {
            m_Ivo.bind_all();
        }

        void unbind() {
            m_Ivo.unbind();
        }

        IndexedVertexObject& get_ivo() {
            return m_Ivo;
        }

        void set_vertex_buffer(
                const float* data,
                size_t count,
                GLint component_count = 3,
                BufferAllocUsage usage = BufferAllocUsage::DYNAMIC_DRAW
        ) {
            FloatAttribPtr ptr{ s_VerticesAttribIndex, component_count };
            if (!m_Ivo.contains_buffer(s_VerticesAttribIndex)) {
                m_Ivo.add_vertex_buffer(data, count, ptr, usage);
            } else {
                m_Ivo.update_vertex_buffer(s_VerticesAttribIndex, data, count, usage);
            }
        }

        void set_model_matrix(const glm::mat4* data, size_t count) {
            if (!m_Ivo.contains_buffer(s_ModelMatrixAttribIndex)) {
                size_t size = sizeof(glm::vec4);
                FloatAttribPtr ptra{ s_ModelMatrixAttribIndex, 4, 1 };
                ptra.stride = 4 * size;
                FloatAttribPtr ptrb{ s_ModelMatrixAttribIndex + 1, 4, 1 };
                ptrb.ptr_offset = (void*)(1*size);
                ptrb.stride = 4 * size;
                FloatAttribPtr ptrc{ s_ModelMatrixAttribIndex + 2, 4, 1 };
                ptrc.ptr_offset = (void*)(2*size);
                ptrc.stride = 4 * size;
                FloatAttribPtr ptrd{ s_ModelMatrixAttribIndex + 3, 4, 1 };
                ptrd.ptr_offset = (void*)(3*size);
                ptrd.stride = 4 * size;

                m_Ivo.add_vertex_buffer<glm::mat4>(data, count, ptra, BufferAllocUsage::DYNAMIC_DRAW);
                ptra.create_and_enable();
                ptrb.create_and_enable();
                ptrc.create_and_enable();
                ptrd.create_and_enable();
            } else {
                m_Ivo.update_vertex_buffer<glm::mat4>(s_ModelMatrixAttribIndex, data, count);
            }
        }

        void set_indices(
                const unsigned int* data,
                size_t count,
                BufferAllocUsage usage = BufferAllocUsage::DYNAMIC_DRAW
        ) {
            m_Ivo.set_index_buffer(data, count, usage);
        }

        template<class V = float>
        void set_extra_buffer(
                const V* data,
                size_t count,
                GLuint attrib_index,
                GLint component_count,
                BufferAllocUsage usage = BufferAllocUsage::STATIC_DRAW
        ) {
            ASSERT(attrib_index > 4, "Batch renderer reserves certain indices.");
            m_Ivo.add_vertex_buffer<V>(data, count, { attrib_index, component_count }, usage);
        }
    };

}

#endif //MAZEVISUALISATION_BATCHRENDERER_H
