//
// Header File: PlayerManager.h
// Date       : 26/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_PLAYERMANAGER_H
#define MAZEVISUALISATION_PLAYERMANAGER_H

#include "Application.h"
#include "BoundingBox.h"
#include "MazeConstructs.h"

#include <glm/glm.hpp>

namespace maze {

    class PlayerManager {

    public:
        inline static constexpr float s_PlayerColliderSize = 0.11F;
        inline static constexpr float s_FixedPlayerY       = 0.2F;
        inline static constexpr float s_PlayingWallScale   = 2.5F;
        inline static constexpr float s_WallColliderSize   = 0.5F;
        inline static constexpr float s_WallColliderOffset = 0.9F;

    private:
        app::AxisAlignedBoundingBox m_PlayerHitBox;

    public:
        PlayerManager(glm::vec3 player_pos = glm::vec3{ 0, 0, 0 });

    public:
        void update(app::Application* app, Maze2D& maze, float delta);
    };

} // maze

#endif
