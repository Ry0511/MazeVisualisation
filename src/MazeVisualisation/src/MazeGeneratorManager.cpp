//
// Header File: MazeGeneratorManager.cpp
// Date       : 26/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "MazeGeneratorManager.h"

namespace maze {

    MazeGeneratorManager::MazeGeneratorManager(

    ) : m_StepsPerUpdate(s_MinSteps),
        m_IsPaused(true),
        m_Theta(0),
        m_Generator(s_MazeGeneratorFactories[0]()),
        m_CurrentGenerator(0) {
        HINFO("[MGM]", " # Maze Generator: '{}'", m_Generator->get_display_name());
    }

    void MazeGeneratorManager::update(app::Application* app, Maze2D& maze, float delta) {
        m_Generator->init_once(maze);

        m_Theta += delta;

        // Fix Camera Y-Position
        app->get_camera_state().cam_pos.y = 15;

        // Pausing
        if (app->is_key_down(app::Key::SPACE)) m_IsPaused = !m_IsPaused;

        // Steps Per Update
        if (app->is_key_down(app::Key::Q)) m_StepsPerUpdate >>= 1;
        if (app->is_key_down(app::Key::E)) m_StepsPerUpdate <<= 1;
        m_StepsPerUpdate = std::clamp(m_StepsPerUpdate, s_MinSteps, s_MaxSteps);

        // Update Current Algorithm
        if (app->is_key_down(app::Key::PLUS)) {
            constexpr size_t max = s_MazeGeneratorFactories.size();
            ++m_CurrentGenerator;

            if (m_CurrentGenerator >= max) {
                m_CurrentGenerator = 0;
            }

            m_Generator = s_MazeGeneratorFactories[m_CurrentGenerator]();
            maze.reset();
            m_IsPaused = true;

            HINFO("[MGM]", " # Maze Generator: '{}'", m_Generator->get_display_name());
        }

        // Restart the Generator
        if (app->is_key_down(app::Key::R)) {
            maze.reset();
            m_IsPaused = true;
            m_Generator = s_MazeGeneratorFactories[m_CurrentGenerator]();
            m_Generator->init(maze);
        }

        // Update Generator
        if (!m_IsPaused
            && !m_Generator->is_complete()
            && m_Theta > s_MinUpdateTimeframe) {
            m_Generator->step(maze, m_StepsPerUpdate);
            m_Theta = 0.0F;
        }
    }

} // maze