//
// Header File: Application.h
// Date       : 16/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#ifndef MAZEVISUALISATION_APPLICATION_H
#define MAZEVISUALISATION_APPLICATION_H

#include "Window.h"

#include <string>
#include <Renderer/Renderer.h>

namespace app {

    class Application : public Window, public Renderer {

    private:
        bool m_IsRunning    = false;
        bool m_IsTerminated = false;

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
    };

} // app

#endif
