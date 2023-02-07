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
#include "Renderer/EntityComponentSystem.h"
#include "Camera3D.h"

#include <glm/glm.hpp>
#include <string>

namespace app {

    class Application
            : public Window,
              public Renderer,
              public EntityComponentSystem,
              public Camera3D {

    private:
        bool      m_IsRunning        = false;
        bool      m_IsTerminated     = false;
        glm::mat4 m_ProjectionMatrix = glm::perspective(glm::radians(90.F), 16.F / 9.F, 0.1F, 1000.F);

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

        bool is_running() const {
            return m_IsRunning;
        }

        bool is_terminated() const {
            return m_IsTerminated;
        }
    };

} // app

#endif
