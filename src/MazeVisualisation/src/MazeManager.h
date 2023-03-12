//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_MAZEMANAGER_H
#define MAZEVISUALISATION_MAZEMANAGER_H

#include "CommonModelFileReaders.h"
#include "MazeConstructs.h"
#include "Renderer/Shader.h"

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
                Maze2D& maze,
                size_t initial_steps = s_MinSteps,
                bool is_paused = true
        ) : m_Generator(std::make_unique<RecursiveBacktrackImpl>()),
            m_StepsPerUpdate(initial_steps),
            m_IsPaused(is_paused) {
            m_Generator->init(maze);
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
    // | MAZE MANAGER |
    //############################################################################//

    class MazeManager {

    public:
        inline static constexpr Index s_MazeSize          = 16;
        inline static std::string     s_TexturedCubeModel = "Res/Models/TexturedCube.obj";
        inline static std::string     s_MazeWallGroup     = "maze_walls_group";
        inline static std::string     s_VertexShaderSrc   = "Res/Shaders/MazeVertexShader.glsl";
        inline static std::string     s_FragShaderSrc     = "Res/Shaders/FragmentShader.glsl";

    private:
        Maze2D               m_Maze;
        MazeGeneratorUpdater m_Generator;
        GameState            m_GameState;
        glm::mat4            m_GlobalRotate;
        glm::mat4            m_GlobalScale;
        float                m_Theta;

    public:
        MazeManager()
                : m_Maze(s_MazeSize, s_MazeSize),
                  m_Generator(m_Maze),
                  m_GameState(GameState::MAZE_GENERATION),
                  m_GlobalRotate(glm::mat4{ 1 }),
                  m_GlobalScale(glm::mat4{ 1 }) {
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
                    m_Maze.get_total_wall_count()
            );

            // Populate Render Group
            app::RenderGroup& group = app->get_group(s_MazeWallGroup);
            m_Maze.create_entities(group);

            group.add_group_functor([=](app::RenderGroup& g, app::Vao& vao, app::Shader& shader) {
                shader.set_uniform(app::Shader::s_ProjectionMatrixUniform, app->get_proj_matrix());
                shader.set_uniform(app::Shader::s_ViewMatrixUniform, app->get_camera_matrix());
                shader.set_uniform(app::Shader::s_RotateMatrixUniform, this->m_GlobalRotate);
                shader.set_uniform(app::Shader::s_ScaleMatrixUniform, this->m_GlobalScale);

                auto& cam_state = app->get_camera_state();
                shader.set_uniform(app::Shader::s_LightPosUniform, cam_state.cam_pos);
                shader.set_uniform(app::Shader::s_LightDirUniform, glm::vec3{0.1, -0.5, -1});
                shader.set_uniform(app::Shader::s_LightColourUniform, glm::vec3{ 1.0, 0.8, 1.0 });
                shader.set_uniform(app::Shader::s_AmbientUniform, 0.2F);
                shader.set_uniform(app::Shader::s_SpecularUniform, 0.4F);
                shader.set_uniform(app::Shader::s_ShininessUniform, 16.F);
                return true;
            });
        }

        void update(app::Application* app, float delta) {
            m_Theta += delta;

            // Switching Game State
            if (app->is_key_down(app::Key::NUM_1)) {
                m_GameState    = GameState::MAZE_GENERATION;
                m_GlobalScale  = glm::mat4{ 1 };
                m_GlobalRotate = glm::mat4{ 1 };
            }

            if (app->is_key_down(app::Key::NUM_2)) {
                m_GameState    = GameState::ALGORITHM_SOLVING;
                m_GlobalScale  = glm::scale(glm::mat4{ 1 }, glm::vec3{ 2.5, 1, 2.5 });
                m_GlobalRotate = glm::mat4{ 1 };
            }

            if (app->is_key_down(app::Key::NUM_3)) {
                m_GameState = GameState::PLAYER_SOLVING;
                app->get_camera_state().cam_pos.x = 0.0F;
                app->get_camera_state().cam_pos.z = 0.0F;
                m_GlobalScale  = glm::scale(glm::mat4{ 1 }, glm::vec3{ 2.5, 1, 2.5 });
                m_GlobalRotate = glm::mat4{ 1 };
            }

            // Update Game State
            switch (m_GameState) {
                case GameState::MAZE_GENERATION: {
                    m_Generator.update(app, m_Maze, delta);
                    break;
                }

                case GameState::ALGORITHM_SOLVING: {
                    float t = glm::radians(m_Theta) * 10;
                    m_GlobalRotate = glm::rotate(glm::mat4{ 1 }, t, glm::vec3{ 1, 0, 0 })
                                     * glm::rotate(glm::mat4{ 1 }, t * 1.25F, glm::vec3{ 0, 1, 0 })
                                     * glm::rotate(glm::mat4{ 1 }, t * 0.8F, glm::vec3{ 0, 0, 1 });
                    break;
                }

                case GameState::PLAYER_SOLVING: {
                    app->get_camera_state().cam_pos.y = 0.2F;
                    break;
                }
            }
        }
    };

}

#endif //MAZEVISUALISATION_MAZEMANAGER_H
