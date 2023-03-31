//
// Header File: MazeGeneratorManager.h
// Date       : 26/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_MAZEGENERATORMANAGER_H
#define MAZEVISUALISATION_MAZEGENERATORMANAGER_H

#include "Application.h"
#include "MazeConstructs.h"

namespace maze {

    class MazeGeneratorManager {

    public:
        inline static constexpr size_t s_MinSteps           = 1;
        inline static constexpr size_t s_MaxSteps           = 1 << 16;
        inline static constexpr float  s_MinUpdateTimeframe = 1.0F / 30.0F;

    private:
        MazeGenerator m_Generator;
        size_t        m_StepsPerUpdate;
        bool          m_IsPaused;
        float         m_Theta;
        size_t        m_CurrentGenerator;

    public:
        MazeGeneratorManager();

    public:
        void update(app::Application* app, Maze2D& maze, float delta);

    };

} // maze

#endif
