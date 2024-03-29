//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_MAZEMANAGER_H
#define MAZEVISUALISATION_MAZEMANAGER_H

#include "PlayerManager.h"
#include "MazeGeneratorManager.h"

#include "CommonModelFileReaders.h"
#include "MazeConstructs.h"
#include "Renderer/Shader.h"
#include "MazeWall.h"
#include "BoundingBox.h"

#include "Image.h"
#include "Renderer/Texture2D.h"
#include "Renderer/DefaultHandlers.h"
#include "MazeTextureManager.h"

namespace maze {

    enum class GameState : char {
        MAZE_GENERATION,
        ALGORITHM_SOLVING,
        PLAYER_SOLVING
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
        MazeGeneratorManager m_GeneratorManager;
        PlayerManager        m_PlayerManager;
        MazeTextureManager   m_TextureManager;
        GameState            m_GameState;
        float                m_Theta;

    public:
        MazeManager()
                : m_Maze(std::make_shared<Maze2D>(s_MazeSize, s_MazeSize)),
                  m_GeneratorManager(),
                  m_PlayerManager(),
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

            // Create Wall Render Group
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
                t.pos                                 = glm::vec3{ (float) pos.row, -1.0F,
                                                                   (float) pos.col };
                t.scale                               = glm::vec3{ 0.5F, 0.5F, 0.5F };
                entity.get_render_attributes().colour = { 0.45F, 0.45F, 0.45F };
                group.queue_entity(std::move(entity));
            });

            // Renderering Flags
            group.set_on_bind([](auto& group) {
                GL(glEnable(GL_STENCIL_TEST));
                GL(glEnable(GL_DEPTH_TEST));
                GL(glEnable(GL_CULL_FACE));
                GL(glEnable(GL_MULTISAMPLE));
                GL(glFrontFace(GL_CCW));
                GL(glCullFace(GL_BACK));
                GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
            });

            // Disable these as they're specific to Walls
            group.set_on_unbind([](auto& group) {
                GL(glDisable(GL_STENCIL_TEST));
                GL(glDisable(GL_CULL_FACE));
                GL(glDisable(GL_MULTISAMPLE));
            });

            m_TextureManager.init();
        }

        void update(app::Application* app, float delta) {
            m_Theta += delta;

            m_TextureManager.update(app, delta);

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
                    m_GeneratorManager.update(app, *m_Maze, delta);
                    break;
                }

                case GameState::ALGORITHM_SOLVING: {
                    break;
                }

                case GameState::PLAYER_SOLVING: {
                    m_PlayerManager.update(app, *m_Maze, delta);
                    break;
                }
            }
        }
    };

}

#endif //MAZEVISUALISATION_MAZEMANAGER_H
