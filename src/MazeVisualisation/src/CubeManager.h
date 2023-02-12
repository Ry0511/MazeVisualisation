//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_CUBEMANAGER_H
#define MAZEVISUALISATION_CUBEMANAGER_H

#include "CommonModelFileReaders.h"
#include "MazeConstructs.h"
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
        app::Mutable3DModel m_CubeModel     = {};
        app::Vao            m_CubeVao       = {};
        app::Shader         m_CubeShader    = {};
        maze::Maze2D        m_Maze          = Maze2D{ 1, 1 };
        maze::MazeGenerator m_MazeGenerator = { nullptr };
        size_t              m_EntityCount   = 0;
        size_t              m_VertexCount   = 0;
        glm::mat4           m_Rotate        = glm::mat4{ 1 };
        glm::mat4           m_Scale         = glm::scale(glm::mat4{ 1 }, glm::vec3{ 0.5 });

    private:
        float m_MazeGeneratorTimer = 0.0F;

    public:
        CubeManager() = default;
        CubeManager(const CubeManager&) = delete;
        CubeManager(CubeManager&&) = delete;

        //############################################################################//
        // | INITIALISE |
        //############################################################################//

    public:
        void init(app::Application* app, Index rows, Index cols) {

            m_Maze          = std::move(Maze2D{ rows, cols });
            m_MazeGenerator = std::move(std::make_unique<RecursiveBacktrackImpl>());
            m_MazeGenerator->init(m_Maze);

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

            // Cubes
            std::vector<glm::vec3> cube_position_buffer{};
            std::vector<glm::mat4> cube_scale_buffer{};
            m_Maze.fill_path_vec(cube_position_buffer, cube_scale_buffer);
            m_CubeVao.add_buffer<FloatAttribLayout33>(
                    init_array_buffer<glm::vec3, BufferAllocUsage::DYNAMIC_DRAW>(
                            cube_position_buffer.data(),
                            cube_position_buffer.size()
                    ),
                    s_CellDataIndex, 1
            );

            m_CubeVao.add_buffer<FloatMat4Attrib>(
                    init_array_buffer<glm::mat4, BufferAllocUsage::DYNAMIC_DRAW>(
                            cube_scale_buffer.data(),
                            cube_scale_buffer.size()
                    ),
                    5U, 1
            );

            m_EntityCount = cube_position_buffer.size() / 2;
            m_CubeVao.unbind();
        }

        //############################################################################//
        // | UPDATE |
        //############################################################################//

        void update(float delta, app::Application* app) {
            m_MazeGeneratorTimer += delta;

            if (m_MazeGeneratorTimer > 0.005F && !m_MazeGenerator->is_complete()) {
                m_MazeGenerator->step(m_Maze);
                m_MazeGeneratorTimer = 0.0F;

                std::vector<glm::vec3> cube_position_buffer{};
                std::vector<glm::mat4> cube_scale_buffer{};
                m_Maze.fill_path_vec(cube_position_buffer, cube_scale_buffer);
                m_CubeVao.bind();
                auto& [buffer, attrib] = m_CubeVao.get_buffer(s_CellDataIndex);
                buffer.bind();
                buffer.set_range<glm::vec3>(0, cube_position_buffer.data(),
                                            cube_position_buffer.size());
                buffer.unbind();

                auto& [scale_buffer, scale_attrib] = m_CubeVao.get_buffer(5U);
                scale_buffer.bind();
                scale_buffer.set_range<glm::mat4>(0, cube_scale_buffer.data(),
                                                  cube_scale_buffer.size());
                scale_buffer.unbind();

                m_CubeVao.unbind();
                m_EntityCount = cube_position_buffer.size() / 2;
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

#endif //MAZEVISUALISATION_CUBEMANAGER_H