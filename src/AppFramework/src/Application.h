//
// Header File: Application.h
// Date       : 16/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_APPLICATION_H
#define MAZEVISUALISATION_APPLICATION_H

#include "Window.h"

#include "Renderer/Renderer.h"
#include "Camera3D.h"

#include <glm/glm.hpp>
#include <string>

namespace app {

    class Application
            : public Window,
              public Renderer,
              public Camera3D {

    private:
        bool      m_IsRunning        = false;
        bool      m_IsTerminated     = false;
        glm::mat4 m_ProjectionMatrix = glm::perspective(glm::radians(80.F), 16.F / 10.F, 0.1F,
                                                        1000.F);
        glm::mat4 m_GlobalScale      = glm::mat4{ 1 };

    public:
        explicit Application(
                const std::string& title = "Window",
                int width = 800,
                int height = 600
        );

    public:
        void start();

    public:
        virtual void on_create() = 0;
        virtual bool on_update(float delta) = 0;

        //############################################################################//
        // | GETTERS |
        //############################################################################//

    public:
        const glm::mat4& get_proj_matrix() const {
            return m_ProjectionMatrix;
        }

        const glm::mat4& get_global_scale() const {
            return m_GlobalScale;
        }

        bool is_running() const {
            return m_IsRunning;
        }

        bool is_terminated() const {
            return m_IsTerminated;
        }

        //############################################################################//
        // | SETTERS |
        //############################################################################//

    public:
        void set_global_scale(const glm::vec3& scale) {
            m_GlobalScale = glm::scale(glm::mat4{1}, scale);
        }

    };

} // app

#endif
