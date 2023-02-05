//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLECUBE_H
#define MAZEVISUALISATION_RENDERABLECUBE_H

#include "Renderer/Shader.h"
#include "Renderer/StandardComponents.h"

namespace maze {

    using namespace app;
    using namespace components;

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

    struct Cube {

    };

    class CubeManager {

    private:
        app::Vao  m_CubeVao;
        float     m_Theta        = 0.0F;
        size_t    m_EntityCount  = 0;
        glm::vec3 m_GlobalRotate = glm::vec3{ 0 };
        glm::mat4 m_Rotate       = glm::mat4{ 1 };
        glm::mat4 m_Scale        = glm::scale(glm::mat4{ 1 }, glm::vec3{ 0.25 });

    public:

        CubeManager(app::Vao vao) : m_CubeVao(vao) {
            INFO("[CUBE_MAN_CREATE]");
        };

    public:
        void init(app::Application* app) {

            m_CubeVao.bind_all();
            auto& [buffer, attrib] = m_CubeVao.get_buffer(2);

            size_t i = 0;

            std::vector<glm::vec3> positions{};
            buffer.bind();
            app->get_group<Cube, Position>().each([&](Position& pos) {
                buffer.set_range<glm::vec3>(i, &pos, 1);
                ++i;
                m_EntityCount = i;
            });

            m_CubeVao.unbind();

            HINFO("[CUBE_MAN_INIT]", " # Cube Count={}", m_EntityCount);
        }

         void update(float delta, app::Application* app)  {
            m_Theta += delta;
            float t = m_Theta;
            m_GlobalRotate = glm::vec3{ t * 1.1, t * 0.85, t * 1.65 };

            m_Rotate = glm::rotate(glm::mat4{ 1 }, m_GlobalRotate.x, { 1, 0, 0 })
                       * glm::rotate(glm::mat4{ 1 }, m_GlobalRotate.y, { 0, 1, 0 })
                       * glm::rotate(glm::mat4{ 1 }, m_GlobalRotate.z, { 0, 0, 1 });
        }

        void render(app::Application* app, app::Shader* shader)  {
            m_CubeVao.bind_all();
            shader->set_uniform("u_RotateMatrix", m_Rotate);
            shader->set_uniform("u_ScaleMatrix", m_Scale);
            app->draw_elements_instanced(app::DrawMode::TRIANGLES, 36, m_EntityCount);
            m_CubeVao.unbind();
        }
    };
}

#endif //MAZEVISUALISATION_RENDERABLECUBE_H
