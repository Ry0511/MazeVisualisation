//
// Created by -Ry on 05/02/2023.
//

#ifndef MAZEVISUALISATION_STANDARDCOMPONENTS_H
#define MAZEVISUALISATION_STANDARDCOMPONENTS_H

#include "Application.h"
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>

namespace app::components {

    //############################################################################//
    // | POSITIONAL COMPONENTS |
    //############################################################################//

    inline static constexpr glm::mat4 s_Identity = glm::mat4{ 1 };

    using Position = glm::vec3;
    using Rotation = glm::vec3;
    using Scale = glm::vec3;

    struct Transform {
        glm::vec3 pos   = glm::vec3{ 0 };
        glm::vec3 rot   = glm::vec3{ 0 };
        glm::vec3 scale = glm::vec3{ 1 };

        glm::mat4 to_matrix() {
            return glm::translate(s_Identity, pos)
                   * glm::rotate(s_Identity, rot.x, glm::vec3{ 1, 0, 0 })
                   * glm::rotate(s_Identity, rot.y, glm::vec3{ 0, 1, 0 })
                   * glm::rotate(s_Identity, rot.z, glm::vec3{ 0, 0, 1 })
                   * glm::scale(s_Identity, scale);
        }
    };

    //############################################################################//
    // | COLLISION COMPONENTS |
    //############################################################################//

    struct ArbitraryCollider {};

    struct BoxCollider {
        glm::vec3 bl;
        glm::vec3 tr;

        bool collides(const BoxCollider& o) {
            return bl.x >= o.bl.x
                   && bl.y >= o.bl.y
                   && bl.z >= o.bl.z
                   && tr.x <= o.tr.x
                   && tr.y <= o.tr.y
                   && tr.z <= o.tr.z;
        }
    };

}

#endif //MAZEVISUALISATION_STANDARDCOMPONENTS_H
