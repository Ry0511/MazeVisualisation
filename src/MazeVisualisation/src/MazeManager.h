//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_MAZEMANAGER_H
#define MAZEVISUALISATION_MAZEMANAGER_H

#include "CommonModelFileReaders.h"
#include "MazeConstructs.h"
#include "Renderer/Shader.h"
#include "MazeWall.h"
#include "BoundingBox.h"

namespace maze {

    enum class GameState : char {
        MAZE_GENERATION,
        ALGORITHM_SOLVING,
        PLAYER_SOLVING
    };

    class MazeGeneratorUpdater {

    public:
        inline static constexpr size_t s_MinSteps = 1;
        inline static constexpr size_t s_MaxSteps = 1 << 14;

    private:
        MazeGenerator m_Generator;
        size_t        m_StepsPerUpdate;
        bool          m_IsPaused;

    public:
        MazeGeneratorUpdater(
                std::shared_ptr<Maze2D> maze,
                size_t initial_steps = s_MinSteps,
                bool is_paused = true
        ) : m_Generator(std::make_unique<RecursiveBacktrackImpl>()),
            m_StepsPerUpdate(initial_steps),
            m_IsPaused(is_paused) {
            m_Generator->init(*maze);
        }

    public:

        void update(app::Application* app, Maze2D& maze, float delta) {

            app->get_camera_state().cam_pos.y = 15;

            if (app->is_key_down(app::Key::SPACE)) m_IsPaused = !m_IsPaused;

            if (app->is_key_down(app::Key::Q)) {
                m_StepsPerUpdate >>= 1;
            }

            if (app->is_key_down(app::Key::E)) {
                m_StepsPerUpdate <<= 1;
                m_StepsPerUpdate = std::clamp(m_StepsPerUpdate, s_MinSteps, s_MaxSteps);
            }

            if (app->is_key_down(app::Key::R)) {
                maze.reset();
                m_Generator = std::make_unique<RecursiveBacktrackImpl>();
                m_Generator->init(maze);
            }

            if (!m_Generator->is_complete() && !m_IsPaused) {
                m_Generator->step(maze, m_StepsPerUpdate);
            }
        }
    };

    //############################################################################//
    // | SHADER MANAGER |
    //############################################################################//

    class ShaderHandler : public app::GroupHandler {

    private:
        app::Application* m_App;

    public:
        ShaderHandler(app::Application* app) : m_App(app) {}

    public:

        virtual bool is_enabled(app::RenderGroup& group) override {
            return m_App != nullptr;
        }

        virtual bool update(app::RenderGroup& group, app::Vao& vao, app::Shader& shader) override {

            // Projection & View & Scale
            shader.set_uniform(app::Shader::s_ProjectionMatrixUniform, m_App->get_proj_matrix());
            shader.set_uniform(app::Shader::s_ViewMatrixUniform, m_App->get_camera_matrix());
            shader.set_uniform(app::Shader::s_ScaleMatrixUniform, m_App->get_global_scale());

            // Lighting
            auto& cam_state = m_App->get_camera_state();
            shader.set_uniform(app::Shader::s_LightPosUniform, cam_state.cam_pos);
            shader.set_uniform(app::Shader::s_LightDirUniform, glm::vec3{ 0.1, -0.5, -1 });
            shader.set_uniform(app::Shader::s_LightColourUniform, glm::vec3{ 1.0, 0.8, 1.0 });
            shader.set_uniform(app::Shader::s_AmbientUniform, 0.2F);
            shader.set_uniform(app::Shader::s_SpecularUniform, 0.4F);
            shader.set_uniform(app::Shader::s_ShininessUniform, 16.F);
            return true;
        }
    };

    //############################################################################//
    // | PLAYER FOLLOWER |
    //############################################################################//

    class PlayerEntity : public app::EntityHandler {

    private:
        app::Application* m_App;
        std::shared_ptr<Maze2D>     m_Maze;
        float                       m_Theta;
        char                        m_CurrentColliderIndex;
        app::AxisAlignedBoundingBox m_BoundingBox;

    public:
        PlayerEntity(
                app::Application* app,
                std::shared_ptr<Maze2D> maze
        ) : m_App(app),
            m_Maze(maze),
            m_Theta(),
            m_CurrentColliderIndex(),
            m_BoundingBox(glm::vec3{ 0, 0, 0 }, 0.5F) {}

    public:
        virtual bool is_enabled(app::Entity& entity) override {
            return m_App != nullptr;
        }

        virtual bool update(app::Entity& entity, app::RenderGroup& group, float delta) override {
            m_Theta += delta;
            if (m_Theta > 0.25F) {
                m_Theta                = 0.0;
                ++m_CurrentColliderIndex;
                m_CurrentColliderIndex = m_CurrentColliderIndex < 4 ? m_CurrentColliderIndex : 0;
            }

            auto& t         = entity.get_transform();
            auto& cam_state = m_App->get_camera_state();

            float collider_size   = 0.5F;
            float collider_offset = 0.9F;
//            app::AxisAlignedBoundingBox collider{ // SOUTH COLLIDER
//                    glm::vec3{ -0.9, 0, 2 },
//                    collider_size
//            };
//            app::AxisAlignedBoundingBox collider{ // NORTH COLLIDER
//                    glm::vec3{ 0.9, 0, 2 },
//                    collider_size
//            };
//            app::AxisAlignedBoundingBox collider{ // EAST COLLIDER
//                    glm::vec3{ 0.0, 0.0, 2.9 },
//                    collider_size
//            };
//            app::AxisAlignedBoundingBox collider{ // EAST COLLIDER
//                    glm::vec3{ 0.0, 0.0, 1.1 },
//                    collider_size
//            };

            t.scale = glm::vec3{ collider_size, collider_size, collider_size };

            // Convert Cam Position to Grid Position (Accounting for Grid Scale)
            float cx     = (cam_state.cam_pos.x / 2.5F);
            float cz     = (cam_state.cam_pos.z / 2.5F);
            float offset = 0.5F;
            m_BoundingBox.realign(
                    glm::vec3{
                            cx,
                            cam_state.cam_pos.y,
                            cz
                    },
                    0.1F
            );
            Index2D player_pos = Index2D{ (Index) (cx + offset), (Index) (cz + offset) };

            float px = player_pos.col;
            float py = player_pos.row;

            // North, East, South, and West (Potential Colliders)
            app::AxisAlignedBoundingBox all_colliders[4]{
                    { glm::vec3{ py - collider_offset, 0.0, px }, collider_size },
                    { glm::vec3{ py, 0.0, px + collider_offset }, collider_size },
                    { glm::vec3{ py + collider_offset, 0.0, px }, collider_size },
                    { glm::vec3{ py, 0.0, px - collider_offset }, collider_size }
            };

            // Update Entity Position
            t.pos   = all_colliders[m_CurrentColliderIndex].get_centre();
            t.pos.y = -0.75;
            entity.set_dirty();

            // If the player is out of bounds don't check for collisions
            if (!m_Maze->inbounds(player_pos)) return true;
            Cell cell = m_Maze->get_cell(player_pos);

            // Collision Detection & Primitive Resolution
            for (size_t i = 0; i < 4; ++i) {
                app::AxisAlignedBoundingBox& collider = all_colliders[i];
                Cardinal wall_dir = get_cardinal(i);
                if (is_wall(wall_dir, cell) && m_BoundingBox.intersects(collider)) {
                    cam_state.cam_pos = cam_state.cam_delta_pos;
                }
            }

            return true;
        }
    };

    //############################################################################//
    // | MAZE MANAGER |
    //############################################################################//

    class MazeManager {

    public:
        using MazePtr = std::shared_ptr<Maze2D>;

    public:
        inline static constexpr Index s_MazeSize          = 16;
        inline static std::string     s_TexturedCubeModel = "Res/Models/TexturedCube.obj";
        inline static std::string     s_MazeWallGroup     = "maze_walls_group";
        inline static std::string     s_VertexShaderSrc   = "Res/Shaders/MazeVertexShader.glsl";
        inline static std::string     s_FragShaderSrc     = "Res/Shaders/FragmentShader.glsl";

    private:
        MazePtr              m_Maze;
        MazeGeneratorUpdater m_Generator;
        GameState            m_GameState;
        float                m_Theta;

    public:
        MazeManager()
                : m_Maze(std::make_shared<Maze2D>(s_MazeSize, s_MazeSize)),
                  m_Generator(m_Maze),
                  m_GameState(GameState::MAZE_GENERATION) {
        }

    public:

        void init(app::Application* app) {

            // Load Model Files
            app::Mutable3DModel model{};
            app::model_file::read_wavefront_file(
                    s_TexturedCubeModel,
                    model
            );

            // Create Render Group
            app->create_render_group(
                    s_MazeWallGroup,
                    std::move(model),
                    s_VertexShaderSrc,
                    s_FragShaderSrc,
                    m_Maze->get_total_wall_count()
            );

            // Maze Wall Render Group
            app::RenderGroup& group = app->get_group(s_MazeWallGroup);
            group.add_group_handler<ShaderHandler>(app);

            // Individual Walls
            m_Maze->for_each_wall_unique([&](Cardinal dir, const Index2D& pos, Cell cell) {
                app::Entity entity;
                entity.add_entity_handler<MazeWall>(m_Maze, pos, dir);
                entity.update(group, 0.0);

                group.queue_entity(std::move(entity));
            });

            // Player Cube (Test)
            app::Entity player_entity;
            player_entity.add_entity_handler<PlayerEntity>(app, m_Maze);
            player_entity.get_render_attributes().colour = glm::vec3{ 1.0, 0.5, 0.0 };
            group.queue_entity(std::move(player_entity));
        }

        void update(app::Application* app, float delta) {
            m_Theta += delta;

            // Switching Game State
            if (app->is_key_down(app::Key::NUM_1)) {
                m_GameState = GameState::MAZE_GENERATION;
                app->get_camera_state().cam_pos = glm::vec3{ 0 };
                app->set_global_scale(glm::vec3{ 1 });
            }

            if (app->is_key_down(app::Key::NUM_2)) {
                m_GameState = GameState::ALGORITHM_SOLVING;
                app->get_camera_state().cam_pos = glm::vec3{ 0 };
                app->set_global_scale(glm::vec3{ 2.5, 1, 2.5 });
            }

            if (app->is_key_down(app::Key::NUM_3)) {
                m_GameState = GameState::PLAYER_SOLVING;
                app->get_camera_state().cam_pos = glm::vec3{ 0 };
                app->set_global_scale(glm::vec3{ 2.5, 1, 2.5 });
            }

            // Update Game State
            switch (m_GameState) {
                case GameState::MAZE_GENERATION: {
                    m_Generator.update(app, *m_Maze, delta);
                    break;
                }

                case GameState::ALGORITHM_SOLVING: {
                    break;
                }

                case GameState::PLAYER_SOLVING: {
                    auto& cam_state = app->get_camera_state();
                    cam_state.cam_pos.y = 0.2F;
                    break;
                }
            }
        }
    };

}

#endif //MAZEVISUALISATION_MAZEMANAGER_H
