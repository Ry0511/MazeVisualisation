//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLECUBE_H
#define MAZEVISUALISATION_RENDERABLECUBE_H

#include "Renderer/Shader.h"

namespace maze {

    using namespace app;

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
        Entity id;
    };

    struct CubeManager {

        app::Vao  cube_vao;
        float     theta         = 0.0F;
        size_t    entity_count  = 0;
        glm::vec3 global_rotate = glm::vec3{ 0 };

        glm::mat4 rotate = glm::mat4{ 1 };
        glm::mat4 scale  = glm::scale(glm::mat4{ 1 }, glm::vec3{ 0.25 });

        void init(app::Application* app) {

            auto group = app->get_group<Cube, Position>();

            cube_vao.bind_all();
            auto& [buffer, attrib] = cube_vao.get_buffer(2);

            size_t i = 0;

            std::vector<glm::vec3> positions{};
            buffer.bind();
            for (auto id : group) {
                Position& pos = group.get<Position>(id);

                buffer.set_range<glm::vec3>(i, &pos.position, 1);
                ++i;
                entity_count = i;
            }
            cube_vao.unbind();

        }

        void update(float delta, app::Application* app) {
            theta += delta;
            float t = theta;
            global_rotate = glm::vec3{ t * 1.1, t * 0.85, t * 1.65 };

            rotate = glm::rotate(glm::mat4{ 1 }, global_rotate.x, { 1, 0, 0 })
                     * glm::rotate(glm::mat4{ 1 }, global_rotate.y, { 0, 1, 0 })
                     * glm::rotate(glm::mat4{ 1 }, global_rotate.z, { 0, 0, 1 });
        }

        void render(app::Application* app, app::Shader* shader) {
            cube_vao.bind_all();
            shader->set_uniform("u_RotateMatrix", rotate);
            shader->set_uniform("u_ScaleMatrix", scale);
            app->draw_elements_instanced(app::DrawMode::TRIANGLES, 36, entity_count);
            cube_vao.unbind();
        }
    };
}

#endif //MAZEVISUALISATION_RENDERABLECUBE_H
