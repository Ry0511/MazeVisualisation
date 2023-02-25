//
// Created by -Ry on 16/01/2023.
//

#include "Application.h"

#include "MazeManager.h"

#include "MazeConstructs.h"
#include "Logging.h"
#include "Renderer/GLUtil.h"

using namespace app::components;

class App : public app::Application {

private:
    float       m_Theta     = 0.0F;
    size_t      m_TickCount = 0;

private:
    maze::MazeManager m_MazeManager = {};

public:
    App() : app::Application("My App", 800, 600) {}

    virtual void camera_update(app::Window& window, float delta) override {
        Camera3D::camera_update(window, delta);
        get_camera_state().cam_pos.y = 5.0F;
    }

    virtual void on_create() override {
        INFO("[ON_CREATE]");
        set_clear_colour({ 0.1, 0.1, 0.1, 1.0 });
        GL(glEnable(GL_STENCIL_TEST));
        GL(glEnable(GL_DEPTH_TEST));
        GL(glEnable(GL_CULL_FACE));
        GL(glEnable(GL_MULTISAMPLE));
        GL(glFrontFace(GL_CCW));
        GL(glCullFace(GL_BACK));
        GL(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
        GL(glLineWidth(1));

        // Initialise Managers
        m_MazeManager.init(this);
    }

    virtual bool on_update(float delta) override {
        m_Theta += delta;

        set_title(
                std::format(
                        "Window # {}fps, Delta: {:.6f}, {}",
                        (int) (1.0 / (delta)),
                        delta,
                        Camera3D::to_string()
                ).c_str()
        );
        const glm::ivec2& size = get_window_size();
        set_viewport(0, 0, size.x, size.y);
        Renderer::clear();

        // Update Maze Manager
        m_MazeManager.update(delta, this);
        m_MazeManager.render(this);

        return true;
    }
};

int main() {
    auto app = new App();
    app->start();
    delete app;
}