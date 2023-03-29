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
        m_Generator(std::make_unique<StandardHuntAndKill>()) {

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

        // Restart the Generator
        if (app->is_key_down(app::Key::R)) {
            maze.reset();
            m_Generator = std::make_unique<RecursiveBacktrackImpl>();
            m_Generator->init(maze);
        }

        // Update Generator
        if (m_Theta > s_MinUpdateTimeframe
            && !m_Generator->is_complete()
            && !m_IsPaused) {
            m_Generator->step(maze, m_StepsPerUpdate);
            m_Theta = 0.0F;
        }
    }

} // maze