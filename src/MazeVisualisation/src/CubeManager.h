//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_CUBEMANAGER_H
#define MAZEVISUALISATION_CUBEMANAGER_H

#include "CommonModelFileReaders.h"
#include "MazeConstructs.h"
#include "Renderer/Shader.h"
#include "Renderer/StandardComponents.h"
#include "Renderer/RendererComponents.h"

namespace maze {

    //############################################################################//
    // | NAMESPACES & ALIAS'S |
    //############################################################################//

    using namespace app;
    using namespace components;
    struct Cube {};

    //############################################################################//
    // | MAZE SHADER |
    //############################################################################//

    class MazeShader : public AbstractShaderBase {

    public:
        inline static std::string s_CubeVertexShader   = "Res/Shaders/MazeVertexShader.glsl";
        inline static std::string s_CubeFragmentShader = "Res/Shaders/FragmentShader.glsl";

    private:
        Shader    m_Shader = {};
        glm::mat4 m_Rotate = glm::mat4{ 1 };
        glm::mat4 m_Scale  = glm::mat4{ 1 };

    public:
        MazeShader() {
            m_Shader.compile_and_link(s_CubeVertexShader, s_CubeFragmentShader);
            m_Shader.enable();
            m_Shader.disable();
        }

    public:
        virtual void begin_render(app::Application* app) {
            m_Shader.enable();

            // Push Uniforms
            m_Shader.set_uniform(Shader::s_ProjectionMatrixUniform, app->get_proj_matrix());
            m_Shader.set_uniform(Shader::s_ViewMatrixUniform, app->get_camera_matrix());
            m_Shader.set_uniform(Shader::s_RotateMatrixUniform, m_Rotate);
            m_Shader.set_uniform(Shader::s_ScaleMatrixUniform, m_Scale);
        }

        virtual void end_render(app::Application* app) {
            m_Shader.disable();
        }
    };

    //############################################################################//
    // | CUBE MANAGER CLASS |
    //############################################################################//

    class CubeManager {

    private:
        inline static constexpr unsigned int s_ColourIndex          = 3U;
        inline static constexpr unsigned int s_WallModelMatrixIndex = 4U;

    private:
        inline static std::string s_CubeObjFile  = "Res/Models/Cube.obj";
        inline static std::string s_TexturedCube = "Res/Models/TexturedCube.obj";

    private:
        Mutable3DModel      m_CubeModel          = {};
        Entity              m_MazeRendererEntity = {};
        maze::Maze2D        m_Maze               = Maze2D{ 1, 1 };
        maze::MazeGenerator m_MazeGenerator      = { nullptr };
        size_t              m_StepsPerUpdate     = 1;

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

            m_CubeModel.clear();
            model_file::read_wavefront_file(s_TexturedCube, m_CubeModel);

            auto& reg = app->get_registry();

            Entity maze_renderer_entity = app->create_entity();
            auto   maze_renderer        = reg.emplace<TriangleMeshRenderer>(
                    maze_renderer_entity,
                    std::move(m_CubeModel.flatten_vertex_data()),
                    std::make_shared<MazeShader>(),
                    0,
                    m_Maze.get_total_wall_count()
            );

            size_t wall_buffer_size = m_Maze.get_total_wall_count();

            // Model Matrix Buffer
            maze_renderer.add_buffer<FloatMat4Attrib, s_WallModelMatrixIndex>(
                    init_array_buffer<glm::mat4, app::BufferAllocUsage::DYNAMIC_DRAW>(
                            nullptr, wall_buffer_size
                    ),
                    1
            );

            // Colour Buffer
            maze_renderer.add_buffer<Vec3Attribute, s_ColourIndex>(
                    init_array_buffer<glm::vec3, app::BufferAllocUsage::DYNAMIC_DRAW>(
                            nullptr, wall_buffer_size
                    ),
                    1
            );

            entt::basic_group group = reg.group<WallBase, Transform, RenderAttributes>();
            m_Maze.insert_into_ecs(app);

            // Fill Model Matrix and Colour buffers
            std::vector<glm::mat4> model_buffer{wall_buffer_size, glm::mat4{1}};
            std::vector<glm::vec3> colour_buffer{wall_buffer_size, glm::vec3{1}};
            group.each([&](
                    Entity id,
                    const WallBase& base,
                    Transform& trans,
                    RenderAttributes& attrib
            ) {
                // Initialise Buffers
                model_buffer[base.get_index()] = std::move(trans.get_matrix());
                colour_buffer[base.get_index()] = attrib.colour;
            });

            // Initialise Colour Buffer
            maze_renderer.set_buffer_range<glm::vec3, s_ColourIndex>(
                    colour_buffer.data(), 0, colour_buffer.size()
            );

            // Initialise Model Buffer
            maze_renderer.set_buffer_range<glm::mat4, s_WallModelMatrixIndex>(
                    model_buffer.data(), 0, model_buffer.size()
            );
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

                auto renderer = app->get_component<TriangleMeshRenderer>(m_MazeRendererEntity);

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

                    glm::mat4 matrix = trans.get_matrix();
                    renderer.set_buffer_range<glm::mat4, s_WallModelMatrixIndex>(&matrix, index, 1);
                    renderer.set_buffer_range<glm::vec3, s_ColourIndex>(&attrib.colour, index, 1);
                });
            }
        }

        //############################################################################//
        // | RENDER |
        //############################################################################//

        void render(app::Application* app) {
            auto renderer = app->get_component<TriangleMeshRenderer>(m_MazeRendererEntity);
            renderer.render(app);
        }

    };

}

#endif //MAZEVISUALISATION_CUBEMANAGER_H
