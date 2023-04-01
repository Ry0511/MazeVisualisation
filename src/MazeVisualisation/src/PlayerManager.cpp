//
// Header File: PlayerManager.cpp
// Date       : 26/03/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "PlayerManager.h"
#include "MazeConstructs.h"

namespace maze {

    PlayerManager::PlayerManager(
            glm::vec3 player_pos
    ) : m_PlayerHitBox(player_pos, s_PlayerColliderSize) {
        HINFO("[PLAYER_MANAGER]", " # Player Manager Initialised...");
    }

    void PlayerManager::update(
            app::Application* app,
            Maze2D& maze,
            float delta
    ) {
        // Variables
        auto& cam_state = app->get_camera_state();
        cam_state.cam_pos.y = s_FixedPlayerY;

        // Convert Cam Position to Grid Position (Accounting for Grid Scale)
        float cx     = (cam_state.cam_pos.x / s_PlayingWallScale);
        float cz     = (cam_state.cam_pos.z / s_PlayingWallScale);
        float offset = 0.5F;
        m_PlayerHitBox.realign(
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
            if (is_wall(wall_dir, cell) && m_PlayerHitBox.intersects(collider)) {
                cam_state.cam_pos = cam_state.cam_delta_pos;
            }
        }
    }

} // maze