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
        inline static unsigned int s_VertexDataIndex      = 0U;
        inline static unsigned int s_ColourIndex          = 3U;
        inline static unsigned int s_WallModelMatrixIndex = 4U;

    private:
        inline static std::string s_CubeObjFile        = "Res/Models/Cube.obj";
        inline static std::string s_TexturedCube       = "Res/Models/TexturedCube.obj";
        inline static std::string s_CubeVertexShader   = "Res/Shaders/MazeVertexShader.glsl";
        inline static std::string s_CubeFragmentShader = "Res/Shaders/FragmentShader.glsl";

    private:
        app::Mutable3DModel m_CubeModel      = {};
        app::Vao            m_CubeVao        = {};
        app::Shader         m_CubeShader     = {};
        maze::Maze2D        m_Maze           = Maze2D{ 1, 1 };
        maze::MazeGenerator m_MazeGenerator  = { nullptr };
        size_t              m_EntityCount    = 0;
        size_t              m_VertexCount    = 0;
        size_t              m_StepsPerUpdate = 1;
        glm::mat4           m_Rotate         = glm::mat4{ 1 };
        glm::mat4           m_Scale          = glm::scale(glm::mat4{ 1 }, glm::vec3{ 1 });

    private:
        float m_MazeGeneratorTimer = 0.0F;
        bool  m_IsPaused           = true;
        bool  m_IsHeld             = false;
        bool  m_IsEHeld            = false;
        bool  m_IsQHeld            = false;

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

            auto& reg = app->get_registry();
            entt::basic_group group = reg.group<WallBase, Transform, RenderAttributes>();
            m_Maze.insert_into_ecs(app);

            // Create Model Matrix Buffer
            m_CubeVao.add_buffer<FloatMat4Attrib>(
                    init_array_buffer<glm::mat4, BufferAllocUsage::DYNAMIC_DRAW>(
                            nullptr, m_Maze.get_total_wall_count()
                    ),
                    s_WallModelMatrixIndex, 1
            );

            m_CubeVao.add_buffer<Vec3Attribute>(
                    init_array_buffer<glm::vec3, app::BufferAllocUsage::DYNAMIC_DRAW>(
                            nullptr, m_Maze.get_total_wall_count()
                    ),
                    s_ColourIndex, 1
            );

            // Fill Model Matrix and Colour buffers
            auto& [colour_buffer, colour_layout] = m_CubeVao.get_buffer(s_ColourIndex);
            auto& [model_matrix, layout]         = m_CubeVao.get_buffer(s_WallModelMatrixIndex);
            group.each([&](
                    Entity id,
                    const WallBase& base,
                    Transform& trans,
                    RenderAttributes& attrib
            ) {

                size_t index = base.get_index();

                // Model Matrix
                model_matrix.bind();
                glm::mat4 matrix = trans.get_matrix();
                model_matrix.set_range<glm::mat4>(index, &matrix, 1);
                model_matrix.unbind();

                // Colour Buffer
                colour_buffer.bind();
                colour_buffer.set_range<glm::vec3>(index, &attrib.colour, 1);
                colour_buffer.unbind();
            });
            m_CubeVao.unbind();

            // Entity Count is fixed.
            m_EntityCount = m_Maze.get_total_wall_count();
        }

        //############################################################################//
        // | UPDATE |
        //############################################################################//

        void update(float delta, app::Application* app) {
            m_MazeGeneratorTimer += delta;

            if (app->is_key_pressed(Key::SPACE) && !m_IsHeld) {
                m_IsPaused = !m_IsPaused;
                m_IsHeld   = true;

            } else if (!app->is_key_pressed(Key::SPACE)) {
                m_IsHeld = false;
            }

            if (app->is_key_pressed(Key::E) && !m_IsEHeld) {
                m_StepsPerUpdate <<= 1;
                m_IsEHeld = true;
            } else if (!app->is_key_pressed(Key::E)) {
                m_IsEHeld = false;
            }

            if (app->is_key_pressed(Key::Q) && !m_IsQHeld) {
                m_StepsPerUpdate = std::min(1ULL, m_StepsPerUpdate << 1);
                m_IsQHeld        = true;
            } else if (!app->is_key_pressed(Key::Q)) {
                m_IsQHeld = false;
            }

            if (!m_IsPaused && m_MazeGeneratorTimer > 0.005F && !m_MazeGenerator->is_complete()) {
                m_MazeGenerator->step(m_Maze, m_StepsPerUpdate);
                m_MazeGeneratorTimer = 0.0F;

                m_CubeVao.bind();
                auto& [colour_buffer, colour_layout] = m_CubeVao.get_buffer(s_ColourIndex);
                auto& [model_buffer, layout]         = m_CubeVao.get_buffer(s_WallModelMatrixIndex);

                entt::registry& reg = app->get_registry();
                auto group = reg.group<WallBase, Transform, RenderAttributes>();
                group.each([&](
                        Entity id,
                        WallBase& base,
                        Transform& trans,
                        RenderAttributes& attrib
                ) {
                    Cell cell = m_Maze.get_cell(base.get_pos());
                    if (cell == base.get_cell()) return;

                    size_t index = base.get_index();

                    base.set_cell(cell);
                    trans.set_scale(base.get_scale_vec());
                    trans.set_pos(base.get_pos_vec());
                    attrib.colour = base.get_colour();

                    model_buffer.bind();
                    glm::mat4 matrix = trans.get_matrix();
                    model_buffer.set_range<glm::mat4>(index, &matrix, 1);
                    model_buffer.unbind();

                    colour_buffer.bind();
                    colour_buffer.set_range<glm::vec3>(index, &attrib.colour, 1);
                    colour_buffer.unbind();
                });
                m_CubeVao.unbind();
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

        //############################################################################//
        // | ECS EVENTS |
        //############################################################################//

        void on_wall_create() {

        }

    };

}

#endif //MAZEVISUALISATION_CUBEMANAGER_H
