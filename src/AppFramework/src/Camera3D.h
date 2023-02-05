//
// Created by -Ry on 01/02/2023.
//

#ifndef MAZEVISUALISATION_CAMERA3D_H
#define MAZEVISUALISATION_CAMERA3D_H

#include "Logging.h"
#include "Window.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace app {

    //############################################################################//
    // | CONSTANT CAMERA VALUES |
    //############################################################################//

    namespace camera_constants {
        inline static constexpr float     s_InitialSpeed       = 4.F;
        inline static constexpr float     s_InitialSpeedScalar = 8.F;
        inline static constexpr float     s_FullRotation       = 360.F;
        inline static constexpr float     s_InitialYaw         = 0.0F;
        inline static constexpr float     s_InitialPitch       = 0.F;
        inline static constexpr float     s_MinPitch           = -89.f;
        inline static constexpr float     s_MaxPitch           = 89.f;
        inline static constexpr glm::vec3 s_InitialPosVec      = glm::vec3{ 0 };
        inline static constexpr glm::vec3 s_InitialFrontVec    = glm::vec3{ 0, 0, -1 };
        inline static constexpr glm::vec3 s_InitialUpVec       = glm::vec3{ 0 };
        inline static constexpr glm::vec3 s_InitialRightVec    = glm::vec3{ 0 };
        inline static constexpr glm::vec3 s_InitialWorldUpVec  = glm::vec3{ 0, 1, 0 };
    }

    //############################################################################//
    // | MUTABLE CAMERA STATE |
    //############################################################################//

    struct CameraState {
        float     cam_speed         = camera_constants::s_InitialSpeed;
        float     cam_speed_scalar  = camera_constants::s_InitialSpeedScalar;
        bool      is_scalar_enabled = false;
        glm::vec3 cam_pos           = camera_constants::s_InitialPosVec;
        glm::vec3 cam_front         = camera_constants::s_InitialFrontVec;
        glm::vec3 cam_up            = camera_constants::s_InitialUpVec;
        glm::vec3 cam_right         = camera_constants::s_InitialRightVec;
        glm::vec3 cam_world_up      = camera_constants::s_InitialWorldUpVec;
        float     cam_yaw           = camera_constants::s_InitialYaw;
        float     cam_pitch         = camera_constants::s_InitialPitch;
        bool      is_dirty          = true;
    };

    //############################################################################//
    // | CAMERA IMPLEMENTATION |
    //############################################################################//

    class Camera3D {

    private:
        CameraState m_State;
        glm::mat4   m_Matrix;

    public:
        Camera3D(
                const glm::vec3 initial_pos = camera_constants::s_InitialPosVec,
                const float yaw = camera_constants::s_InitialYaw,
                const float pitch = camera_constants::s_InitialPitch
        ) : m_State(
                CameraState{
                        .cam_pos = initial_pos,
                        .cam_yaw = yaw,
                        .cam_pitch = pitch,
                        .is_dirty = true
                }
        ), m_Matrix(glm::mat4{ 1 }) {

        }

    public:

        //############################################################################//
        // | DEFAULT CAMERA CONTROL IMPLEMENTATION |
        //############################################################################//

        virtual void camera_update(app::Window& app, float delta) {

            // Movement Speed
            float scalar = 1.0F;
            if (app.is_modifier_set(Modifier::SHIFT)) scalar = m_State.cam_speed_scalar;
            float vel = m_State.cam_speed * scalar * delta;

            // Move
            if (app.is_key_pressed(Key::W)) m_State.cam_pos += m_State.cam_front * vel;
            if (app.is_key_pressed(Key::S)) m_State.cam_pos -= m_State.cam_front * vel;
            if (app.is_key_pressed(Key::A)) m_State.cam_pos -= m_State.cam_right * vel;
            if (app.is_key_pressed(Key::D)) m_State.cam_pos += m_State.cam_right * vel;

            // Camera Controls
            if (app.is_mouse_pressed(MouseButton::MOUSE_LEFT)) {
                app.lock_cursor_to_centre(true);
                const glm::vec2 mouse_vel = app.get_mouse_vel();
                m_State.cam_yaw = glm::mod(
                        m_State.cam_yaw + mouse_vel.x,
                        camera_constants::s_FullRotation
                );

                m_State.cam_pitch = std::clamp(
                        m_State.cam_pitch - mouse_vel.y,
                        camera_constants::s_MinPitch,
                        camera_constants::s_MaxPitch
                );
            } else {
                app.lock_cursor_to_centre(false);
            }

            m_State.is_dirty = true;
        };

        //############################################################################//
        // | CAMERA STATE UPDATE |
        //############################################################################//

        void update_camera_matrix() {

            // Calculate Camera Vectors
            const float yaw   = glm::radians(m_State.cam_yaw);
            const float pitch = glm::radians(m_State.cam_pitch);

            glm::vec3 front = {
                    cos(yaw) * cos(pitch),
                    sin(pitch),
                    sin(yaw) * cos(pitch)
            };

            m_State.cam_front = glm::normalize(front);
            m_State.cam_right = glm::normalize(
                    glm::cross(
                            m_State.cam_front,
                            m_State.cam_world_up
                    )
            );

            m_State.cam_up = glm::normalize(
                    glm::cross(
                            m_State.cam_right,
                            m_State.cam_front
                    )
            );

            m_Matrix = glm::lookAt(
                    m_State.cam_pos,
                    m_State.cam_pos + m_State.cam_front,
                    m_State.cam_up
            );

            m_State.is_dirty = false;
        };

        //############################################################################//
        // | GET CAMERA STATE |
        //############################################################################//

    public:
        inline const CameraState& get_camera_state() const {
            return m_State;
        }

        inline CameraState& get_camera_state() {
            return m_State;
        }

        inline const glm::mat4& get_camera_matrix() {
            if (m_State.is_dirty) {
                update_camera_matrix();
            }
            return m_Matrix;
        }

        inline std::string to_string() const {
            return std::format(
                    "( Pos={:.2f},{:.2f},{:.2f} Yaw={:.3f}, Pitch={:.3f}, Right={:.2f},{:.2f},{:.2f} Up={:.2f},{:.2f},{:.2f} )",
                    m_State.cam_pos.x,
                    m_State.cam_pos.y,
                    m_State.cam_pos.z,
                    m_State.cam_yaw,
                    m_State.cam_pitch,
                    m_State.cam_right.x,
                    m_State.cam_right.y,
                    m_State.cam_right.z,
                    m_State.cam_up.x,
                    m_State.cam_up.y,
                    m_State.cam_up.z
            );
        }
    };
}

#endif //MAZEVISUALISATION_CAMERA3D_H
