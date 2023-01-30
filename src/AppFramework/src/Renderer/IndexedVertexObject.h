//
// Created by -Ry on 27/01/2023.
//

#ifndef MAZEVISUALISATION_INDEXEDVERTEXOBJECT_H
#define MAZEVISUALISATION_INDEXEDVERTEXOBJECT_H

#include "Renderer/Buffer.h"
#include "Renderer/VertexArrayObject.h"

#include <unordered_map>
#include <algorithm>

namespace app {

    //############################################################################//
    // | Simple Indexed Vertex Buffer Object |
    //############################################################################//

    using VertexBuffer = StaticFloatArrBuffer;
    using IndexBuffer = StaticUIntElementBuffer;

    template<ComponentCount Count>
    struct IndexedVertexObject {
        VertexArrayObject            vao{};
        VertexBuffer                 vertex_positions{};
        FloatAttribPointer<0, Count> vertex_attrib{};
        IndexBuffer                  index_buffer{};

        void init() {
            vao.init();
            vertex_positions.init();
            index_buffer.init();
        }

        void bind() {
            vao.bind();
            vertex_positions.bind();
            vertex_attrib.enable();
            index_buffer.bind();
        }

        void unbind() {
            index_buffer.unbind();
            vertex_positions.unbind();
            vertex_attrib.disable();
            vao.unbind();
        }

        void set_vertex_positions(float* buffer, size_t count) {
            vertex_positions.bind();
            vertex_positions.set_data(buffer, count);
            vertex_attrib.create();
        }

        void set_index_buffer(unsigned int* buffer, size_t count) {
            index_buffer.bind();
            index_buffer.set_data(buffer, count);
        }

        void init_bind_set(
                float* vertex_pos_buffer,
                size_t vertex_count,
                unsigned int* indices,
                size_t index_count
        ) {
            init();
            bind();
            set_vertex_positions(vertex_pos_buffer, vertex_count);
            set_index_buffer(indices, index_count);
        }
    };

    //############################################################################//
    // | DEFAULT TEMPLATE SPECIALISATIONS |
    //############################################################################//

    using Indexed1DVertexObject = IndexedVertexObject<ComponentCount::ONE>;
    using Indexed2DVertexObject = IndexedVertexObject<ComponentCount::TWO>;
    using Indexed3DVertexObject = IndexedVertexObject<ComponentCount::THREE>;
    using Indexed4DVertexObject = IndexedVertexObject<ComponentCount::FOUR>;

}

#endif //MAZEVISUALISATION_INDEXEDVERTEXOBJECT_H
