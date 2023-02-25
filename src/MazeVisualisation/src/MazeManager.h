//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_MAZEMANAGER_H
#define MAZEVISUALISATION_MAZEMANAGER_H

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
    // | MAZE PROGRAM STATE |
    //############################################################################//

    enum class MazeGameState : int {
        ALGORITHM_GENERATION = 0,
        ALGORITHM_SOLVING    = 1,
        PLAYER_PLAYING       = 2
    };

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
        glm::mat4 m_Scale  = glm::scale(glm::mat4{ 1 }, glm::vec3{ 4, 1.5, 4 });
        Entity    m_ManagerEntity;

    public:
        MazeShader(Entity manager, app::Application* app) : m_ManagerEntity(manager) {
            m_Shader.compile_and_link(s_CubeVertexShader, s_CubeFragmentShader);
            m_Shader.enable();
            m_Shader.disable();

            // Add the lighting component
            app->get_registry().emplace<Lighting>(manager);
        }

    public:
        virtual void begin_render(app::Application* app) {
            m_Shader.enable();

            // Push Uniforms
            m_Shader.set_uniform(Shader::s_ProjectionMatrixUniform, app->get_proj_matrix());
            m_Shader.set_uniform(Shader::s_ViewMatrixUniform, app->get_camera_matrix());
            m_Shader.set_uniform(Shader::s_RotateMatrixUniform, m_Rotate);
            m_Shader.set_uniform(Shader::s_ScaleMatrixUniform, m_Scale);

            // Lighting Uniforms
            const auto& lighting = app->get_registry().get<Lighting>(m_ManagerEntity);
            m_Shader.set_uniform("u_LightPos", lighting.pos);
            m_Shader.set_uniform("u_LightDir", lighting.dir);
            m_Shader.set_uniform("u_LightColour", lighting.light_colour);
            m_Shader.set_uniform("u_Ambient", lighting.ambient);
            m_Shader.set_uniform("u_Specular", lighting.specular);
            m_Shader.set_uniform("u_Shininess", lighting.shininess);
        }

        virtual void end_render(app::Application* app) {
            m_Shader.disable();
        }
    };

    //############################################################################//
    // | MAZE RENDER BUFFERS |
    //############################################################################//

    class MazeRenderBuffer {

    private:
        inline static constexpr unsigned int s_ColourIndex          = 3U;
        inline static constexpr unsigned int s_WallModelMatrixIndex = 4U;
        inline static std::string            s_CubeObjFile          = "Res/Models/Cube.obj";
        inline static std::string            s_TexturedCube         = "Res/Models/TexturedCube.obj";

    private:
        Mutable3DModel m_CubeModel{};
        Entity         m_ManagerEntity;

    public:
        MazeRenderBuffer(
                Entity manager, app::Application* app, size_t wall_count
        ) : m_ManagerEntity(manager) {
            auto& reg = app->get_registry();

            // Read Cube Model File
            app::model_file::read_wavefront_file(s_TexturedCube, m_CubeModel);

            // Cube Renderer
            auto maze_renderer = reg.emplace<TriangleMeshRenderer>(
                    m_ManagerEntity,
                    std::move(m_CubeModel.flatten_vertex_data()),
                    std::make_shared<MazeShader>(m_ManagerEntity, app),
                    0,
                    wall_count
            );

            // Model Matrix Buffer
            maze_renderer.add_buffer<FloatMat4Attrib, s_WallModelMatrixIndex>(
                    init_array_buffer<glm::mat4, app::BufferAllocUsage::DYNAMIC_DRAW>(
                            nullptr, wall_count
                    ),
                    1
            );

            // Colour Buffer
            maze_renderer.add_buffer<Vec3Attribute, s_ColourIndex>(
                    init_array_buffer<glm::vec3, app::BufferAllocUsage::DYNAMIC_DRAW>(
                            nullptr, wall_count
                    ),
                    1
            );
        }

    public:

        __forceinline TriangleMeshRenderer& get_mesh_renderer(app::Application* app) {
            return app->get_registry().get<TriangleMeshRenderer>(m_ManagerEntity);
        }

        __forceinline void update_colour_buffer(
                app::Application* app,
                glm::vec3* colour_buffer,
                size_t begin,
                size_t end
        ) {
            TriangleMeshRenderer& mesh = get_mesh_renderer(app);
            mesh.set_buffer_range<glm::vec3, s_ColourIndex>(colour_buffer, begin, end);
        }

        __forceinline void update_model_buffer(
                app::Application* app,
                glm::mat4* matrix_buffer,
                size_t begin,
                size_t end
        ) {
            TriangleMeshRenderer& mesh = get_mesh_renderer(app);
            mesh.set_buffer_range<glm::mat4, s_WallModelMatrixIndex>(matrix_buffer, begin, end);
        }
    };

    //############################################################################//
    // | CUBE MANAGER CLASS |
    //############################################################################//

    class MazeManager {

    private:
        inline static constexpr unsigned int s_InitialSize              = 12;
        inline static constexpr float        s_GeneratorUpdateTimeFrame = 0.005F;

    private:
        Entity        m_ManagerEntity = {};
        maze::Maze2D  m_Maze          = Maze2D{ s_InitialSize, s_InitialSize };
        MazeGameState m_GameState     = MazeGameState::ALGORITHM_GENERATION;
        float m_Theta = 0.0F;

        // Updating the Generator
    private:
        float               m_MazeGeneratorTimer = 0.0F;
        bool                m_IsPaused           = true;
        maze::MazeGenerator m_MazeGenerator      = { nullptr };
        size_t              m_StepsPerUpdate     = 1;

    public:
        MazeManager() = default;
        MazeManager(const MazeManager&) = delete;
        MazeManager(MazeManager&&) = delete;

        //############################################################################//
        // | INITIALISE |
        //############################################################################//

    public:
        void init(app::Application* app) {

            m_MazeGenerator = std::move(std::make_unique<RecursiveBacktrackImpl>());
            m_MazeGenerator->init(m_Maze);

            size_t wall_count = m_Maze.get_total_wall_count();
            auto& reg = app->get_registry();
            m_ManagerEntity = app->create_entity();

            auto& maze_buffer = reg.emplace<MazeRenderBuffer>(
                    m_ManagerEntity,
                    m_ManagerEntity,
                    app,
                    wall_count
            );

            entt::basic_group group = reg.group<WallBase, Transform, RenderAttributes>();
            m_Maze.insert_into_ecs(app);

            // Initialise Buffers
            std::vector<glm::mat4> model_buffer{ wall_count, glm::mat4{ 1 }};
            std::vector<glm::vec3> colour_buffer{ wall_count, glm::vec3{ 1 }};
            group.each([&](
                    Entity id,
                    const WallBase& base,
                    Transform& trans,
                    RenderAttributes& attrib
            ) {
                // Initialise Buffers
                model_buffer[base.get_index()]  = std::move(trans.get_matrix());
                colour_buffer[base.get_index()] = attrib.colour;
            });

            maze_buffer.update_colour_buffer(app, colour_buffer.data(), 0, colour_buffer.size());
            maze_buffer.update_model_buffer(app, model_buffer.data(), 0, model_buffer.size());
        }

        //############################################################################//
        // | UPDATE |
        //############################################################################//

        void update(float delta, app::Application* app) {
            m_Theta += delta;

            // Update Lighting Position & Direction
            Lighting  & lighting  = app->get_registry().get<Lighting>(m_ManagerEntity);
            const auto& cam_state = app->get_camera_state();
            lighting.pos = cam_state.cam_pos + glm::vec3{ 0.0F, 5.0F, 0.0F };
            lighting.dir = cam_state.cam_front;

            update_controls(delta, app);

            if (m_IsPaused) return;

            switch (m_GameState) {
                [[likely]] case MazeGameState::ALGORITHM_GENERATION: {
                    if (!m_MazeGenerator->is_complete()) update_generator(delta, app);
                    break;
                }
                case MazeGameState::ALGORITHM_SOLVING: {
                    HINFO("[ALGORITHM_SOLVING]", " # Not currently implemented...");
                    break;
                }
                case MazeGameState::PLAYER_PLAYING: {
                    HINFO("[PLAYER_PLAYING]", " # Not currently implemented...");
                    break;
                }
            }
        }

        //############################################################################//
        // | HANDLE CONTROLS |
        //############################################################################//

        void update_controls(float delta, app::Application* app) {
            if (app->is_key_down(Key::SPACE)) {
                m_IsPaused = !m_IsPaused;
            }

            if (app->is_key_down(Key::E)) {
                m_StepsPerUpdate <<= 1;
                m_StepsPerUpdate = std::min(1ULL << 16, m_StepsPerUpdate);
            }

            if (app->is_key_down(Key::Q)) {
                m_StepsPerUpdate >>= 1;
                m_StepsPerUpdate = std::max(1ULL, m_StepsPerUpdate);
            }

            if (app->is_key_down(Key::R)) {
                m_Maze.reset();
                m_MazeGenerator = std::move(std::make_unique<RecursiveBacktrackImpl>());
                m_MazeGenerator->init(m_Maze);
            }
        }

        //############################################################################//
        // | UPDATE / MAZE GENERATOR |
        //############################################################################//

        void update_generator(float delta, app::Application* app) {
            m_MazeGeneratorTimer += delta;
            if (m_MazeGeneratorTimer < s_GeneratorUpdateTimeFrame) return;

            m_MazeGenerator->step(m_Maze, m_StepsPerUpdate);
            m_MazeGeneratorTimer = 0.0F;

            entt::registry& reg         = app->get_registry();
            auto          & maze_buffer = reg.get<MazeRenderBuffer>(m_ManagerEntity);
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
                maze_buffer.update_colour_buffer(app, &attrib.colour, index, 1);
                maze_buffer.update_model_buffer(app, &matrix, index, 1);
            });
        }

        //############################################################################//
        // | RENDER |
        //############################################################################//

        void render(app::Application* app) {
            auto renderer = app->get_component<TriangleMeshRenderer>(m_ManagerEntity);
            renderer.render(app);
        }

    };

}

#endif //MAZEVISUALISATION_MAZEMANAGER_H
