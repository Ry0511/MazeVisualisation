//
// Header File: Application.cpp
// Date       : 16/01/2023
// Project    : MazeVisualisation
// Author     : -Ry
//

#include "Application.h"
#include "Logging.h"

namespace app {

    namespace Chrono = std::chrono;
    using Clock = Chrono::system_clock;

    //############################################################################//
    // | CLASS IMPLEMENTATION |
    //############################################################################//

    Application::Application(
            const std::string& title,
            int width,
            int height
    ) : Window(title.c_str(), width, height),
        Renderer(),
        Camera3D() {
        INFO("Application Created");
    }

    void Application::start() {
        if (m_IsTerminated) {
            WARN("Can't start a terminated application...");
            throw std::exception();
        }

        if (!m_IsRunning) {
            INFO("Starting Application");
            m_IsRunning = true;

            make_context_current();
            on_create();
            auto before    = Clock::now();
            while (!should_close()) {
                auto now = Clock::now();
                Window::poll_events();
                swap_buffers();

                float delta = Chrono::duration<float>(now - before).count();
                camera_update(*this, delta);
                if (!on_update(delta)) close();

                before = now;
            }
            m_IsRunning    = false;
            m_IsTerminated = true;

        } else {
            INFO("Application Already Running...");
        }
    }

} // app