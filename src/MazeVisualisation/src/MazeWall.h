//
// Created by -Ry on 12/03/2023.
//

#ifndef MAZEVISUALISATION_MAZEWALL_H
#define MAZEVISUALISATION_MAZEWALL_H

#include "Renderer/Entity.h"
#include "Renderer/RendererHandlers.h"
#include "MazeConstructs.h"

namespace maze {

    class MazeWall : public app::EntityHandler {

    private:
        std::shared_ptr<Maze2D> m_Maze;
        WallBase                m_WallBase;

    public:
        MazeWall(
                std::shared_ptr<Maze2D> maze,
                const Index2D& pos,
                const Cardinal dir,
                const Cell cell = 0
        ) : m_Maze(maze),
            m_WallBase(cell, pos, dir) {

        }

    public:

        virtual bool is_enabled(app::Entity& entity) override {
            return true;
        }

        virtual bool update(app::Entity& entity, app::RenderGroup& group, float delta) override {

            // If the underlying Cell has changed Update it
            Cell cell = m_Maze->get_cell(m_WallBase.get_pos());
            if (cell == m_WallBase.get_cell()) return true;

            // Update Entity & Notify of Changes
            m_WallBase.set_cell(cell);
            auto& t = entity.get_transform();
            t.pos                                 = m_WallBase.get_pos_vec();
            t.scale                               = m_WallBase.get_scale_vec();
            entity.get_render_attributes().colour = m_WallBase.get_colour();
            entity.set_dirty();

            return true;
        }
    };

}

#endif //MAZEVISUALISATION_MAZEWALL_H
