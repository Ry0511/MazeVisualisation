//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLECUBE_H
#define MAZEVISUALISATION_RENDERABLECUBE_H

#include "Renderer/RenderableEntity.h"

namespace maze {

    namespace cube_obj {
        static constexpr std::array<float, 24> s_VertexPositions = {
                -1.0, 1.0, 1.0,
                -1.0, -1.0, 1.0,
                -1.0, 1.0, -1.0,
                -1.0, -1.0, -1.0,
                1.0, 1.0, 1.0,
                1.0, -1.0, 1.0,
                1.0, 1.0, -1.0,
                1.0, -1.0, -1.0
        };

        static constexpr std::array<unsigned int, 36> s_Indices = {
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

        static constexpr std::array<float, 36> s_Colours = {
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
    }

    class RenderableCube : public app::RenderableEntity {

    private:
        inline static size_t s_InstanceCount = 0;

    private:
        float m_Theta = 0.0F;

    public:
        RenderableCube() : app::RenderableEntity() {
            ++s_InstanceCount;
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
        }

    public:
        virtual void update(float ts) override {
            m_Theta += ts;
            m_Transform.rotate = { m_Theta, m_Theta, m_Theta };
        }

    };
}

#endif //MAZEVISUALISATION_RENDERABLECUBE_H
