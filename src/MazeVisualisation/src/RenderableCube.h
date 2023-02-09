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

    private:
        float m_GlobalHaltTime = 0.0F;

    private:
        glm::vec3 m_CamInitialPos{};
        glm::vec3 m_TargetCamPos{};
        glm::vec3 m_CamLookAt{};
        float     m_CamTranslateProgress = 0.0F;
        bool      m_IsCamTranslating     = false;

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
            // | LOAD VAO OBJECT |
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
                    0U
            );

            // Translates
            std::vector<glm::vec3> positions{};
            m_Maze->for_each([&](size_t r, size_t c, MazeCell cell) {
                positions.push_back(glm::vec3{ r, 0, c });
            });
            m_CubeVao.add_buffer<Vec3Attribute>(
                    init_array_buffer<glm::vec3, BufferAllocUsage::DYNAMIC_DRAW>(
                            positions.data(),
                            positions.size()
                    ),
                    4U, 1
            );

            // Colours
            std::vector<glm::vec3> colours{};
            m_Maze->for_each([&](auto i, auto j, auto cell) {
                colours.emplace_back(0.0, 0.0, 0.0);
            });
            m_CubeVao.add_buffer<Vec3Attribute>(
                    init_array_buffer<glm::vec3, BufferAllocUsage::DYNAMIC_DRAW>(
                            colours.data(), colours.size()
                    ),
                    3U, 1
            );

            // Cell Flags
            m_CubeVao.add_buffer<UnaryUIntAttribute>(
                    init_array_buffer<MazeCell, BufferAllocUsage::DYNAMIC_DRAW>(
                            m_Maze->get_data(), m_Maze->get_cell_count()
                    ),
                    5U, 1
            );

            m_CubeVao.unbind();
        }

        //############################################################################//
        // | UPDATE |
        //############################################################################//

        void update(float delta, app::Application* app) {
            m_MazeGeneratorTimer += delta;
            m_CamRotateTheta += delta;

            if (m_GlobalHaltTime > 0.0F) {
                m_GlobalHaltTime -= delta;

            } else if (m_IsCamTranslating) {
                glm::vec3 lerp     = glm::mix(
                        m_CamInitialPos, m_TargetCamPos, m_CamTranslateProgress
                );
                app->Camera3D::lookat(m_CamLookAt, lerp);
                m_CamTranslateProgress += delta * 3.F;
                m_IsCamTranslating = 1.F > m_CamTranslateProgress;
                app->Camera3D::get_camera_state().cam_pos = lerp;

            } else if (m_MazeGeneratorTimer > 0.1F && !m_MazeGenerator->is_complete()) {
                auto index = m_MazeGenerator->step(*m_Maze);
                m_MazeGeneratorTimer = 0.0;

                // Look at the edited cell
                m_IsCamTranslating     = true;
                m_CamTranslateProgress = 0.0F;
                m_CamInitialPos        = app->Camera3D::get_camera_state().cam_pos;
                m_TargetCamPos         = glm::vec3(index.get_row() + 2, 2, index.get_col() + 2);
                m_CamLookAt            = glm::vec3(index.get_row(), 0.5, index.get_col());
                app->Camera3D::set_dirty_override();
                m_GlobalHaltTime = 0.1;

                m_CubeVao.bind();

                auto& [flag_buffer, flag_attrib] = m_CubeVao.get_buffer(5U);
                flag_buffer.bind();
                flag_buffer.set_range<GLuint>(0, m_Maze->get_data(), m_Maze->get_cell_count());

                size_t                 i = 0;
                glm::vec3              changed{};
                std::vector<glm::vec3> colours{};
                m_Maze->for_each([&](size_t r, size_t c, MazeCell& cell) {
                    if (maze::check_flag<maze::CellFlag::MODIFIED>(cell)) {
                        changed = { r, 0, c };
                        cell &= ~cellof<MazeCellFlags::MODIFIED>();
                        i       = r * m_Maze->get_height() + c;
                    }

                    glm::vec3 colour{ 0.3, 0.3, 0.8 };
                    if (maze::check_flag<MazeCellFlags::RED>(cell)) colour.r   = 1.0;
                    if (maze::check_flag<MazeCellFlags::GREEN>(cell)) colour.g = 1.0;
                    if (maze::check_flag<MazeCellFlags::BLUE>(cell)) colour.b  = 1.0;
                    colours.emplace_back(colour);
                });
                auto& [translate_buffer, translate_attrib] = m_CubeVao.get_buffer(4U);
                translate_buffer.bind();
                translate_buffer.set_range<glm::vec3>(i, &changed, 1);

                auto& [colour_buffer, colour_attrib] = m_CubeVao.get_buffer(3U);
                colour_buffer.bind();
                colour_buffer.set_range<glm::vec3>(0, colours.data(), colours.size());

                m_CubeVao.unbind();

            } else if (m_MazeGenerator->is_complete()) {
                float t = m_MazeGeneratorTimer;
                m_Rotate = glm::rotate(glm::mat4{ 1 }, t * 1.1F, { 1, 0, 0 })
                           * glm::rotate(glm::mat4{ 1 }, t * 0.85F, { 0, 1, 0 })
                           * glm::rotate(glm::mat4{ 1 }, t * 1.65F, { 0, 0, 1 });

                app->Camera3D::set_dirty_override(false);
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
