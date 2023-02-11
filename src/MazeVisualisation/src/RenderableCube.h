//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_RENDERABLECUBE_H
#define MAZEVISUALISATION_RENDERABLECUBE_H

#include "CommonModelFileReaders.h"
#include "MazeController.h"
#include "Renderer/Shader.h"
#include "Renderer/StandardComponents.h"

namespace maze {

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
        inline static unsigned int s_VertexDataIndex = 0U;
        inline static unsigned int s_CellDataIndex   = 3U;

    private:
        inline static std::string s_CubeObjFile        = "Res/Models/Cube.obj";
        inline static std::string s_TexturedCube       = "Res/Models/TexturedCube.obj";
        inline static std::string s_CubeVertexShader   = "Res/Shaders/MazeVertexShader.glsl";
        inline static std::string s_CubeFragmentShader = "Res/Shaders/FragmentShader.glsl";

    private:
        app::Mutable3DModel  m_CubeModel          = {};
        app::Vao             m_CubeVao            = {};
        app::Shader          m_CubeShader         = {};
        maze::MazePtr        m_Maze               = { nullptr };
        maze::MazeFactoryPtr m_MazeGenerator      = { nullptr };
        size_t               m_EntityCount        = 0;
        size_t               m_VertexCount        = 0;
        float                m_MazeGeneratorTimer = 0.0F;
        float                m_CamRotateTheta     = 0.0F;
        glm::mat4            m_Rotate             = glm::mat4{ 1 };
        glm::mat4            m_Scale              = glm::scale(glm::mat4{ 1 }, glm::vec3{ 0.25 });

    public:
        CubeManager() = default;
        CubeManager(const CubeManager&) = delete;
        CubeManager(CubeManager&&) = delete;

        //############################################################################//
        // | INITIALISE |
        //############################################################################//

    public:
        void init(app::Application* app, MazeCell rows, MazeCell cols) {

            m_Maze          = std::move(std::make_unique<MutableMaze>(rows, cols));
            m_MazeGenerator = std::move(std::make_unique<TrulyRandomMazeImpl>());
            m_EntityCount   = m_Maze->get_cell_count();

            //############################################################################//
            // | LOAD VAO OBJECT & SHADERS |
            //############################################################################//

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
                    s_VertexDataIndex, 0
            );

            // Translates & Colours (Combined Buffer)
            std::vector<float> pos_and_colours{};
            m_Maze->for_each([&](size_t r, size_t c, MazeCell cell) {
                pos_and_colours.insert(
                        pos_and_colours.end(),
                        { (float) r, 0.F, (float) c, 0.25F, 0.75F, 0.25F }
                );
            });
            m_CubeVao.add_buffer<FloatAttribLayout33>(
                    init_array_buffer<float, BufferAllocUsage::DYNAMIC_DRAW>(
                            pos_and_colours.data(),
                            pos_and_colours.size()
                    ),
                    s_CellDataIndex, 1
            );

            m_CubeVao.unbind();
        }

        //############################################################################//
        // | UPDATE |
        //############################################################################//

        void update(float delta, app::Application* app) {
            m_MazeGeneratorTimer += delta;
            m_CamRotateTheta += delta;

            if (m_MazeGeneratorTimer > 0.0F && !m_MazeGenerator->is_complete()) {
                auto index = m_MazeGenerator->step(*m_Maze);
                m_MazeGeneratorTimer = 0.0;

                m_CubeVao.bind();

                m_EntityCount = 0;
                std::vector<float> pos_and_colours{};
                m_Maze->for_each([&](size_t r, size_t c, MazeCell& cell) {
                    if (!check_flag<MazeCellFlags::EMPTY>(cell)) {

                        // Calculate Cell Colour
                        float red{}, green{}, blue{};
                        if (check_flag<MazeCellFlags::RED>(cell)) red     = 1.0F;
                        if (check_flag<MazeCellFlags::GREEN>(cell)) green = 1.0F;
                        if (check_flag<MazeCellFlags::BLUE>(cell)) blue   = 1.0F;

                        pos_and_colours.insert(
                                pos_and_colours.end(),
                                { (float) r, 0, (float) c, red, green, blue }
                        );

                        ++m_EntityCount;
                    }
                });
                auto& [cell_buffer, translate_attrib] = m_CubeVao.get_buffer(s_CellDataIndex);
                cell_buffer.bind();
                cell_buffer.set_range<float>(0, pos_and_colours.data(), pos_and_colours.size());

                m_CubeVao.unbind();

            } else if (m_MazeGenerator->is_complete()) {
                float t = m_MazeGeneratorTimer;
                m_Rotate = glm::rotate(glm::mat4{ 1 }, t * 1.1F, { 1, 0, 0 })
                           * glm::rotate(glm::mat4{ 1 }, t * 0.85F, { 0, 1, 0 })
                           * glm::rotate(glm::mat4{ 1 }, t * 1.65F, { 0, 0, 1 });
            }

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
