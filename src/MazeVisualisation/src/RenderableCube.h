//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLECUBE_H
#define MAZEVISUALISATION_RENDERABLECUBE_H

#include "Renderer/Shader.h"
#include "Renderer/StandardComponents.h"
#include "CommonModelFileReaders.h"

namespace maze {

    //############################################################################//
    // | ORIGINAL CUBE VERTEX DATA |
    //############################################################################//

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

    //############################################################################//
    // | NAMESPACES & ALIAS'S |
    //############################################################################//

    using namespace app;
    using namespace components;
    struct Cube {};

    //############################################################################//
    // | CUBE MANAGER CLASS |
    //############################################################################//

    class CubeManager {

    private:
        inline static std::string s_CubeObjFile        = "Res/Models/Cube.obj";
        inline static std::string s_TexturedCube       = "Res/Models/TexturedCube.obj";
        inline static std::string s_CubeVertexShader   = "Res/Shaders/MazeVertexShader.glsl";
        inline static std::string s_CubeFragmentShader = "Res/Shaders/FragmentShader.glsl";

    private:
        app::Mutable3DModel m_CubeModel   = {};
        app::Vao            m_CubeVao     = {};
        app::Shader         m_CubeShader  = {};
        size_t              m_EntityCount = 0;
        size_t              m_VertexCount = 0;
        float               m_Theta       = 0.0F;
        glm::mat4           m_Rotate      = glm::mat4{ 1 };
        glm::mat4           m_Scale       = glm::scale(glm::mat4{ 1 }, glm::vec3{ 0.25 });

    public:
        CubeManager() = default;
        CubeManager(const CubeManager&) = delete;
        CubeManager(CubeManager&&) = delete;

        //############################################################################//
        // | INITIALISE |
        //############################################################################//

    public:
        void init(app::Application* app) {

            // Load Shader
            m_CubeShader.compile_and_link(s_CubeVertexShader, s_CubeFragmentShader);

            // Load Model
            m_CubeModel.clear();
            app::model_file::read_wavefront_file(s_TexturedCube, m_CubeModel);
            std::vector<glm::vec3> vertex_data = m_CubeModel.flatten_vertex_data();
            m_VertexCount = vertex_data.size();

            m_CubeVao.init();
            m_CubeVao.bind();

            // Vertex: Position, Normal, Texture
            m_CubeVao.add_buffer<FloatAttribLayout333>(
                    init_array_buffer<glm::vec3>(
                            vertex_data.data(),
                            vertex_data.size()
                    ),
                    0U
            );

            // Colours
            m_CubeVao.add_buffer<Vec3Attribute>(
                    init_array_buffer(
                            maze::cube_obj::s_Colours.data(),
                            maze::cube_obj::s_Colours.size()
                    ),
                    3U
            );

            // Translates
            std::vector<glm::vec3> translates{};
            app->get_group<Cube, Position>().each([&](const glm::vec3& pos) {
                translates.push_back(pos);
                ++m_EntityCount;
            });

            // Add Translate Buffer
            m_CubeVao.add_buffer<Vec3Attribute>(
                    init_array_buffer(translates.data(), translates.size()),
                    4U, 1
            );

            m_CubeVao.unbind();
        }

        //############################################################################//
        // | UPDATE |
        //############################################################################//

        void update(float delta, app::Application* app) {
            m_Theta += delta;
            float t = m_Theta;

            m_Rotate = glm::rotate(glm::mat4{ 1 }, t * 1.1F, { 1, 0, 0 })
                       * glm::rotate(glm::mat4{ 1 }, t * 0.85F, { 0, 1, 0 })
                       * glm::rotate(glm::mat4{ 1 }, t * 1.65F, { 0, 0, 1 });
        }

        //############################################################################//
        // | RENDER |
        //############################################################################//

        void render(app::Application* app) {

            // Enable
            m_CubeShader.enable();
            m_CubeVao.bind_all();

            // Push Uniforms
            m_CubeShader.set_uniform(Shader::s_ProjectionMatrixUniform, app->get_proj_matrix());
            m_CubeShader.set_uniform(Shader::s_ViewMatrixUniform, app->get_camera_matrix());
            m_CubeShader.set_uniform(Shader::s_RotateMatrixUniform, m_Rotate);
            m_CubeShader.set_uniform(Shader::s_ScaleMatrixUniform, m_Scale);

            // Instance Render
            // app->draw_elements_instanced(app::DrawMode::TRIANGLES, 36, m_EntityCount);
            app->draw_buffer_instanced(
                    app::DrawMode::TRIANGLES,
                    0,
                    m_VertexCount,
                    m_EntityCount
            );

            // Disable
            m_CubeVao.unbind();
            m_CubeShader.disable();
        }
    };
}

#endif //MAZEVISUALISATION_RENDERABLECUBE_H
