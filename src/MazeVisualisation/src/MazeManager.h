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

#include "Image.h"
#include "Renderer/Texture2D.h"
#include "Renderer/DefaultHandlers.h"

namespace maze {

    enum class GameState : char {
        MAZE_GENERATION,
        ALGORITHM_SOLVING,
        PLAYER_SOLVING
    };

    //############################################################################//
    // | PLAYER SOLVER STATE |
    //############################################################################//

    class PlayerSolver {

    public:
        inline static constexpr float s_PlayerColliderSize = 0.11F;
        inline static constexpr float s_FixedPlayerY       = 0.2F;
        inline static constexpr float s_PlayingWallScale   = 2.5F;
        inline static constexpr float s_WallColliderSize   = 0.5F;
        inline static constexpr float s_WallColliderOffset = 0.9F;


    private:
        app::AxisAlignedBoundingBox m_BoundingBox;


    public:
        PlayerSolver(

        ) : m_BoundingBox(glm::vec3{ 0, 0, 0 }, s_PlayerColliderSize) {

        }

    public:
        void update(app::Application* app, Maze2D& maze, float delta) {

            // Variables
            auto& cam_state = app->get_camera_state();
            cam_state.cam_pos.y = s_FixedPlayerY;

            // Convert Cam Position to Grid Position (Accounting for Grid Scale)
            float cx     = (cam_state.cam_pos.x / s_PlayingWallScale);
            float cz     = (cam_state.cam_pos.z / s_PlayingWallScale);
            float offset = 0.5F;
            m_BoundingBox.realign(
                    glm::vec3{
                            cx,
                            cam_state.cam_pos.y,
                            cz
                    },
                    s_PlayerColliderSize
            );
            Index2D player_pos = Index2D{ (Index) (cx + offset), (Index) (cz + offset) };

            float px = player_pos.col;
            float py = player_pos.row;

            // If the player is out of bounds don't check for collisions
            if (!maze.inbounds(player_pos)) return;
            Cell cell = maze.get_cell(player_pos);

            // North, East, South, and West (Potential Colliders)
            constexpr size_t            collider_count = 4;
            app::AxisAlignedBoundingBox all_colliders[collider_count]{
                    { glm::vec3{ py - s_WallColliderOffset, 0.0, px }, s_WallColliderSize },
                    { glm::vec3{ py, 0.0, px + s_WallColliderOffset }, s_WallColliderSize },
                    { glm::vec3{ py + s_WallColliderOffset, 0.0, px }, s_WallColliderSize },
                    { glm::vec3{ py, 0.0, px - s_WallColliderOffset }, s_WallColliderSize }
            };

            // Collision Detection & Primitive Resolution
            for (size_t i = 0; i < collider_count; ++i) {
                app::AxisAlignedBoundingBox& collider = all_colliders[i];
                Cardinal wall_dir = get_cardinal(i);
                if (is_wall(wall_dir, cell) && m_BoundingBox.intersects(collider)) {
                    cam_state.cam_pos = cam_state.cam_delta_pos;
                }
            }
        }
    };

    //############################################################################//
    // | MAZE GENERATOR UPDATER |
    //############################################################################//

    class MazeGeneratorUpdater {

    public:
        inline static constexpr size_t s_MinSteps           = 1;
        inline static constexpr size_t s_MaxSteps           = 1 << 14;
        inline static constexpr float  s_MinUpdateTimeframe = 1.0F / 30.0F;

    private:
        MazeGenerator m_Generator;
        size_t        m_StepsPerUpdate;
        bool          m_IsPaused;
        float         m_Theta = 0.0;

    public:
        MazeGeneratorUpdater(
                std::shared_ptr<Maze2D> maze,
                size_t initial_steps = s_MinSteps,
                bool is_paused = true
        ) : m_Generator(std::make_unique<RecursiveBacktrackImpl>()),
            m_StepsPerUpdate(initial_steps),
            m_IsPaused(is_paused),
            m_Theta() {
            m_Generator->init(*maze);
        }

    public:

        void update(app::Application* app, Maze2D& maze, float delta) {
            m_Theta += delta;

            app->get_camera_state().cam_pos.y = 15;

            if (app->is_key_down(app::Key::SPACE)) m_IsPaused = !m_IsPaused;

            if (app->is_key_down(app::Key::Q)) {
                m_StepsPerUpdate >>= 1;
                m_StepsPerUpdate = std::clamp(m_StepsPerUpdate, s_MinSteps, s_MaxSteps);
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

            if (m_Theta > s_MinUpdateTimeframe
                && !m_Generator->is_complete()
                && !m_IsPaused) {
                m_Generator->step(maze, m_StepsPerUpdate);
                m_Theta = 0.0F;
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
        PlayerSolver         m_PlayerSolver;
        GameState            m_GameState;
        float                m_Theta;

        // TODO: TEMPORARY
        app::Image     m_WallImage{ "Res/Textures/Texture-1.png" };
        app::Texture2D m_WallTexture{};

    public:
        MazeManager()
                : m_Maze(std::make_shared<Maze2D>(s_MazeSize, s_MazeSize)),
                  m_Generator(m_Maze),
                  m_PlayerSolver(),
                  m_GameState(GameState::MAZE_GENERATION),
                  m_Theta() {
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

            // Maze Floor
            m_Maze->for_each_cell([&](Index2D pos, Cell cell) {
                app::Entity entity;
                auto& t = entity.get_transform();
                t.pos   = glm::vec3{ (float) pos.row, -1.0F, (float) pos.col };
                t.scale = glm::vec3{ 0.5F, 0.5F, 0.5F };
                entity.add_entity_handler<app::ColourSkew>(
                        glm::vec3{ 0.54F, 0.68F, 0.78F },
                        glm::vec3{ 0.25F, 0.33F, 0.43F },
                        glm::vec3{ 1.0F, 1.0F, 1.0F }
                );
                group.queue_entity(std::move(entity));
            });

            group.set_on_bind([](auto& group) {
                GL(glEnable(GL_STENCIL_TEST));
                GL(glEnable(GL_DEPTH_TEST));
                GL(glEnable(GL_CULL_FACE));
                GL(glEnable(GL_MULTISAMPLE));
                GL(glFrontFace(GL_CCW));
                GL(glCullFace(GL_BACK));
                GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
            });

            group.set_on_unbind([](auto& group) {
                GL(glDisable(GL_STENCIL_TEST));
                GL(glDisable(GL_CULL_FACE));
                GL(glDisable(GL_MULTISAMPLE));
            });

            // TODO: TEMPORARY
            m_WallTexture.init();
            m_WallTexture.bind();
            m_WallTexture.set_texture_flags(
                    {
                            std::make_pair(app::TextureFlag::WRAP_S, GL_REPEAT),
                            std::make_pair(app::TextureFlag::WRAP_T, GL_REPEAT),
                            std::make_pair(app::TextureFlag::MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR),
                            std::make_pair(app::TextureFlag::MAG_FILTER, GL_LINEAR),
                    }
            );

            m_WallTexture.set_texture_image(m_WallImage);
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
                    m_PlayerSolver.update(app, *m_Maze, delta);
                    break;
                }
            }
        }
    };

}

#endif //MAZEVISUALISATION_MAZEMANAGER_H
