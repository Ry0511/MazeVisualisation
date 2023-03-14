//
// Created by -Ry on 14/03/2023.
//

#ifndef MAZEVISUALISATION_BOUNDINGBOX_H
#define MAZEVISUALISATION_BOUNDINGBOX_H

#include <string>
#include <format>

#include <glm/glm.hpp>

namespace app {

    class AxisAlignedBoundingBox {

    private:
        glm::vec3 m_Min;
        glm::vec3 m_Max;

    public:

        AxisAlignedBoundingBox(
                const glm::vec3& min,
                const glm::vec3& max
        ) : m_Min(min), m_Max(max) {

        }

        AxisAlignedBoundingBox(
                const glm::vec3& centre,
                const float offset
        ) : m_Min(centre.x - offset, centre.y - offset, centre.z - offset),
            m_Max(centre.x + offset, centre.y + offset, centre.z + offset) {

        }

    public:

        const glm::vec3& get_min() const {
            return m_Min;
        }

        const glm::vec3& get_max() const {
            return m_Max;
        }

        glm::vec3 get_centre() const {
            return (m_Min + m_Max) * 0.5F;
        }

    public:

        void realign(const glm::vec3& centre, const float offset) {
            m_Min = glm::vec3(centre.x - offset, centre.y - offset, centre.z - offset);
            m_Max = glm::vec3(centre.x + offset, centre.y + offset, centre.z + offset);
        }

        void realign(const glm::vec3& min, const glm::vec3& max) {
            m_Min = min;
            m_Max = max;
        }

    public:

        bool intersects(const AxisAlignedBoundingBox& o) const {
            return m_Min.x <= o.m_Max.x
                   && m_Max.x >= o.m_Min.x
                   && m_Min.y <= o.m_Max.y
                   && m_Max.y >= o.m_Min.y
                   && m_Min.z <= o.m_Max.z
                   && m_Max.z >= o.m_Min.z;
        }


    public:

        std::string to_string() const {
            return std::format(
                    "Min=( {:.2f}, {:.2f}, {:.2f} ),"
                    " Max=( {:.2f}, {:.2f}, {:.2f} )",
                    m_Min.x, m_Min.y, m_Min.z,
                    m_Max.x, m_Max.y, m_Max.z
            );
        }
    };
}

#endif //MAZEVISUALISATION_BOUNDINGBOX_H
